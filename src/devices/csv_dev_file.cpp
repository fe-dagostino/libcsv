/**************************************************************************************************
 * 
 * Copyright 2022 https://github.com/fe-dagostino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following 
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 *
 *************************************************************************************************/

#include "csv_dev_file.h"
#include <assert.h>
#include <cstring>
#include <fstream>

namespace csv {
inline namespace LIB_VERSION {

#define DeviceOption(p)   (static_cast<const csv_dev_file_options *>(p.get()))


/////////////////
// Byte Order Marks by encoding
// https://en.wikipedia.org/wiki/Byte_order_mark
//
// In the following table the byte[0] specify BOM length.
// 0xAA value is used as undefined value.
static std::uint8_t __BOM__[][5] = {

  { 0x00, 0xAA, 0xAA, 0xAA, 0xAA },
  { 0x03, 0xEF, 0xBB, 0xBF, 0xAA },  // UTF-8

  { 0x02, 0xFE, 0xFF, 0xAA, 0xAA },  // UTF-16 BE
  { 0x02, 0xFF, 0xFE, 0xAA, 0xAA },  // UTF-16 LE 

  { 0x04, 0x00, 0x00, 0xFE, 0xFF },  // UTF-32 BE 
  { 0x04, 0xFF, 0xFE, 0x00, 0x00 },  // UTF-32 LE 

  { 0x03, 0x2B, 0x2F, 0x76, 0xAA },  // UTF-7  
  { 0x03, 0xF7, 0x64, 0x4c, 0xAA },  // UTF-1  

  { 0x04, 0xDD, 0x73, 0x66, 0x73 },  // UTF-EBCDIC

  { 0x03, 0x0E, 0xFE, 0xFF, 0xAA },  // SCSU
  { 0x03, 0xFB, 0xEE, 0x28, 0xAA },  // BOCU-1

  { 0x04, 0x84, 0x31, 0x95, 0x33 }   // GB-18030
};

csv_dev_file::csv_dev_file( core::unique_ptr<csv_dev_file_options> ptrDeviceOptions, core::unique_ptr<csv_device_events> ptrEvents )
    : csv_device( "csv_dev_file", std::move(ptrDeviceOptions), std::move(ptrEvents) ),
      m_pFile(nullptr), m_pRxBuffer(nullptr), m_nCacheSize(0), m_nCursor(0)
{
  assert( csv_device::get_options().get() != nullptr );
}

csv_dev_file::~csv_dev_file()
{
  release();
}

csv_dev_file_options::filetype  csv_dev_file::detect_and_skip_bom() noexcept
{
  csv_dev_file_options::filetype _retVal = csv_dev_file_options::filetype::PLAIN_TEXT;
  byte                           _rxBOM[4];
  size_t                         _bom_size = fread( _rxBOM, sizeof(byte), 4, m_pFile );

  for ( uint8_t ndx = 1; ndx < static_cast<uint8_t>(csv_dev_file_options::filetype::MAX_VALUE); ++ndx )
  { 
    if ( __BOM__[ndx][0] <= _bom_size )
      if (memcmp ( _rxBOM, &__BOM__[ndx][1], __BOM__[ndx][0] ) == 0)
      { 
        _retVal = static_cast<csv_dev_file_options::filetype>(ndx); 
        fseeko64( m_pFile, __BOM__[ndx][0], SEEK_SET );
        break;
      }
  }

  if ( _retVal == csv_dev_file_options::filetype::PLAIN_TEXT )
    fseeko64( m_pFile, 0, SEEK_SET );

  return _retVal;
}

void  csv_dev_file::write_bom() noexcept
{
  switch (DeviceOption(m_ptrOptions)->get_bom())
  {

    case csv_dev_file_options::filetype::UTF_8:
    case csv_dev_file_options::filetype::UTF_16BE:
    case csv_dev_file_options::filetype::UTF_16LE:
    case csv_dev_file_options::filetype::UTF_32BE:
    case csv_dev_file_options::filetype::UTF_32LE:
    case csv_dev_file_options::filetype::UTF_7:
    case csv_dev_file_options::filetype::UTF_1:
    case csv_dev_file_options::filetype::UTF_EBCDIC:
    case csv_dev_file_options::filetype::SCSU:
    case csv_dev_file_options::filetype::BOCU_1:
    case csv_dev_file_options::filetype::GB_18030:
    {
      uint8_t ndx = static_cast<uint8_t>(DeviceOption(m_ptrOptions)->get_bom());
      send( reinterpret_cast<const byte*>(&__BOM__[ndx][1]), __BOM__[ndx][0] );
    }; break;
  
    default:
    break;
  }
  
}

csv_result csv_dev_file::open() noexcept
{
  csv_result  _retVal = csv_result::_ok;

  if ( m_pFile != nullptr )
    return _retVal;

  m_pFile        = fopen64( DeviceOption(m_ptrOptions)->get_filename().c_str(), 
                          ((DeviceOption(m_ptrOptions)->get_mode()==csv_dev_file_options::openmode::write)?"w":"r") );
  // Note that using an raw pointer instead of a smart pointer increase performances for the cached methods.
  m_pRxBuffer    = nullptr;
  if (DeviceOption(m_ptrOptions)->get_mode()==csv_dev_file_options::openmode::read)
    m_pRxBuffer  = new(std::nothrow) byte[DeviceOption(m_ptrOptions)->get_bufsize()]; 

  m_nCacheSize   = 0;
  m_nCursor      = 0;

  if ( ( m_pFile == nullptr ) || ((m_pRxBuffer == nullptr) && (DeviceOption(m_ptrOptions)->get_mode()==csv_dev_file_options::openmode::read)) )
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
  int _iRetVal = 0;
  if (DeviceOption(m_ptrOptions)->get_mode()==csv_dev_file_options::openmode::read)
    _iRetVal = setvbuf( m_pFile, nullptr, _IONBF, 0 );
  else
    _iRetVal = setvbuf( m_pFile, nullptr, _IOFBF, DeviceOption(m_ptrOptions)->get_bufsize() );

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

  ///////////////////////////////
  // Detect or Write BOM
  switch ( DeviceOption(m_ptrOptions)->get_mode() )
  {
    case csv_dev_file_options::openmode::write:
    {
      write_bom();
    }; break;
  
    default: // default csv_dev_file_options::openmode::read
    {
      auto _detected_bom = detect_and_skip_bom();
      if (
          ( DeviceOption(m_ptrOptions)->get_bom() != _detected_bom ) &&
          ( DeviceOption(m_ptrOptions)->get_bom() != csv_dev_file_options::filetype::PLAIN_TEXT )
        )
      {
        if ( m_ptrEvents != nullptr )
        {
          m_ptrEvents->onError( this, csv_result::_bom_mismatch );
        }
      }      
    }; break;
  }

  m_devStats.rx     = 0;
  m_devStats.tx     = 0;
  m_devStats.errors = 0;

  return _retVal;
}

csv_result csv_dev_file::send(const byte* pBuffer, csv_uint_t iBufferLen) noexcept
{
  // Do not allow call to send() when not in writing mode
  if ( DeviceOption(m_ptrOptions)->get_mode() != csv_dev_file_options::openmode::write )
    return csv_result::_wrong_call;

  csv_result  _retVal = open();
  if ( _retVal != csv_result::_ok )
    return _retVal;

  csv_uint_t _wbytes = fwrite((const void*)pBuffer, 1, iBufferLen, m_pFile );
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
    if ( m_ptrEvents != nullptr )
    {
      m_ptrEvents->onError( this, _retVal );
    }

    //Close socket and release resources
    close();
  }

  return _retVal;
}

csv_result csv_dev_file::recv(byte* pBuffer, csv_uint_t& nBufferLen) noexcept
{
  // Do not allow call to recv() when not in reading mode
  if ( DeviceOption(m_ptrOptions)->get_mode() != csv_dev_file_options::openmode::read )
    return csv_result::_wrong_call;

  csv_result  _retVal = open();
  if ( _retVal != csv_result::_ok )
    return _retVal;

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

  if ( (_retVal == csv_result::_eof) && (nBufferLen > 0) )
  {
    // In this condition device return last read buytes and even then number
    // bytes is less than requested. Return value will be _ok and next call will be _eof
    _retVal = csv_result::_ok;
  }
  else if (
      ( _retVal != csv_result::_ok          ) &&
      ( _retVal != csv_result::_rx_timedout )
     )
  {
    if ( m_ptrEvents != nullptr )
    {
      m_ptrEvents->onError( this, _retVal );
    }    

    //Close and release resources
    close();
  }
  
  return _retVal;
}

csv_result csv_dev_file::refresh_cache() noexcept
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

csv_result csv_dev_file::close() noexcept
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

csv_result csv_dev_file::flush() noexcept
{
  if ( m_pFile == nullptr )
    return csv_result::_closed;

  fflush(m_pFile);

  return csv_result::_ok;
}

csv_result csv_dev_file::is_valid() const noexcept
{
  if ( m_pFile == nullptr )
    return csv_result::_closed;

  return csv_result::_ok;
}

constexpr void csv_dev_file::release() noexcept
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
