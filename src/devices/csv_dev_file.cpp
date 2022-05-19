
#include "csv_dev_file.h"
#include <assert.h>
#include <cstring>
#include <fstream>

namespace csv {
inline namespace LIB_VERSION {

#define DeviceOption(p)   (static_cast<const csv_dev_file_options *>(p.get()))

csv_dev_file::csv_dev_file( std::unique_ptr<csv_dev_file_options> ptrDeviceOptions, std::unique_ptr<csv_device_events> ptrEvents )
    : csv_device( "csv_dev_file", std::move(ptrDeviceOptions), std::move(ptrEvents) ),
      m_pFile(nullptr), m_pRxBuffer(nullptr), m_nCacheSize(0), m_nCursor(0)
{
  assert( csv_device::get_options().get() != nullptr );
}

csv_dev_file::~csv_dev_file()
{
  release();
}

csv_result csv_dev_file::open()
{
  csv_result  _retVal = csv_result::_ok;

  if ( m_pFile != nullptr )
    return _retVal;

  m_pFile        = fopen64( DeviceOption(m_ptrOptions)->get_filename().c_str(), 
                          ((DeviceOption(m_ptrOptions)->get_mode()==csv_dev_file_options::openmode::write)?"w":"r") );
  // Note that using an raw pointer instead of a smart pointer increase performances for the cached methods.
  m_pRxBuffer    = new(std::nothrow) std::byte[DeviceOption(m_ptrOptions)->get_bufsize()]; 
                                                                                                            
  m_nCacheSize   = 0;
  m_nCursor      = 0;

  if (( m_pFile == nullptr ) || ( m_pRxBuffer == nullptr ))
  {
    release();

    _retVal = csv_result::_no_mem;

    if ( m_ptrEvents != nullptr )
    {
      m_ptrEvents->onError( this, _retVal );
    }

    return _retVal;
  }

  /*
  *  _IOFBF	Full buffering: On output, data is written once the buffer is full (or flushed).
  *                         On Input, the buffer is filled when an input operation is requested and the buffer is empty.
  *  _IOLBF	Line buffering: On output, data is written when a newline character is inserted into the stream or when the buffer is full (or flushed), whatever happens first. 
  *                         On Input, the buffer is filled up to the next newline character when an input operation is requested and the buffer is empty.
  *  _IONBF	No buffering: No buffer is used. Each I/O operation is written as soon as possible. In this case, the buffer and size parameters are ignored.
  */
  int _iRetVal = setvbuf( m_pFile, nullptr, _IONBF, 0 );
  if ( _iRetVal != 0 )
  {
    release();

    _retVal = csv_result::_access;

    if ( m_ptrEvents != nullptr )
    {
      m_ptrEvents->onError( this, _retVal );
    }

    return _retVal;
  }

  if ( m_ptrEvents != nullptr )
  {
    m_ptrEvents->onOpened( this );
  }

  m_devStats.rx     = 0;
  m_devStats.tx     = 0;
  m_devStats.errors = 0;

  return _retVal;
}

csv_result csv_dev_file::send(const std::byte* pBuffer, csv_uint_t iBufferLen)
{
  csv_result  _retVal = open();
  if ( _retVal != csv_result::_ok )
    return _retVal;

  csv_uint_t _wbytes = fwrite((const void*)pBuffer, iBufferLen, 1, m_pFile );
  // An error occurs writing data.
  if ( _wbytes != iBufferLen )
  {
    m_devStats.errors++;

    _retVal = csv_result::_tx_error;
  }
  else
  {
    m_devStats.tx += iBufferLen;
  }

  if ( _retVal != csv_result::_ok )
  {
    //Close socket and release resources
    close();
  }

  return _retVal;
}

csv_result csv_dev_file::recv(std::byte* pBuffer, csv_uint_t& nBufferLen)
{
  if ( open() != csv_result::_ok )
    return csv_result::_rx_error;

  csv_result _retVal        = csv_result::_ok;
  csv_uint_t _nWishedBytes  = nBufferLen;
  csv_uint_t _nBufCursor    = 0;

  while ((_nWishedBytes > 0) && (_retVal == csv_result::_ok))
  {
    const csv_uint_t _nAvailableBytes = m_nCacheSize - m_nCursor;

    if ( _nAvailableBytes == 0 )
    {
      _retVal = refresh_cache();
    }
    // We handle the case in which the nWishedBytes can't satisfied with current
    // amount of data in the cache.
    else if ( _nAvailableBytes < _nWishedBytes )
    {
      memcpy( &pBuffer[_nBufCursor], &m_pRxBuffer[m_nCursor], _nAvailableBytes );
      m_nCursor      += _nAvailableBytes;
      _nBufCursor    += _nAvailableBytes;
      _nWishedBytes  -= _nAvailableBytes;      
    }
    // Check if there are enough bytes in the internal buffer to satisfy the request
    else if ( _nAvailableBytes >= _nWishedBytes )
    {
      memcpy( &pBuffer[_nBufCursor], &m_pRxBuffer[m_nCursor], _nWishedBytes );
      m_nCursor     += _nWishedBytes;
      _nWishedBytes -= _nWishedBytes;
    }
  }

  // WishedBytes can be positive if there are not enough bytes to satisfy original request.
  // In such case pBuffer contain only partial data and nBufferLen must be updated accordigly.
  nBufferLen -= _nWishedBytes;

  if (
      ( _retVal != csv_result::_ok          ) &&
      ( _retVal != csv_result::_rx_timedout )
     )
  {
    //Close and release resources
    close();
  }

  return _retVal;
}

csv_result csv_dev_file::refresh_cache()
{
  m_nCacheSize = fread( m_pRxBuffer, 1, DeviceOption(m_ptrOptions)->get_bufsize(), m_pFile );
  m_nCursor    = 0;

  if ( m_nCacheSize > 0 )
  {
    m_devStats.rx += m_nCacheSize;
  }
  else if ( feof( m_pFile ) != 0 )
  {
    return csv_result::_eof;
  }
  else if ( ferror( m_pFile ) != 0 )
  {
    m_devStats.errors++;
    return csv_result::_rx_error;
  }

  return csv_result::_ok;
}

csv_result csv_dev_file::close()
{
  if ( m_pFile == nullptr )
    return csv_result::_closed;

  release();

  if ( m_ptrEvents != nullptr )
  {
    m_ptrEvents->onClosed( this );
  }

  return csv_result::_ok;
}

csv_result csv_dev_file::flush()
{
  if ( m_pFile == nullptr )
    return csv_result::_closed;

  fflush(m_pFile);

  return csv_result::_ok;
}

csv_result csv_dev_file::is_valid() const
{
  if ( m_pFile == nullptr )
    return csv_result::_closed;

  return csv_result::_ok;
}

void csv_dev_file::release()
{
  if( m_pFile != nullptr ) 
  {
    fclose(m_pFile);
    m_pFile = nullptr;
  }

  if ( m_pRxBuffer != nullptr )
  {
    delete [] m_pRxBuffer;
    m_pRxBuffer = nullptr;
  }

  m_nCacheSize = 0;
  m_nCursor    = 0;
}


} //inline namespace
} // namespace
