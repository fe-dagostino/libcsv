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

#ifndef CSV_DEV_FILE_H
#define CSV_DEV_FILE_H

#include "csv_device.h"
#include <iostream>

namespace csv {
inline namespace LIB_VERSION {

class csv_dev_file_options : public csv_device_options
{
public:
  enum class openmode : char {
    read = 'r',                 // read: Open file for input operations. The file must exist. 
    write = 'w'                 // write: Create an empty file for output operations. If a file with the 
                                // same name already exists, its contents are discarded and the file is 
                                // treated as a new empty file.
  };

  /**
   * File Device options.
   *
   * @param sFilename    filename.
   */
  explicit csv_dev_file_options( std::string sFilename, openmode modeFlags, csv_uint_t buf_size = to_bytes<1>::MBytes )
  : csv_device_options(), m_sFilename(sFilename), m_modeFlags(modeFlags),
    m_bufSize( buf_size )
  {
  }

  /***/
  virtual ~csv_dev_file_options()
  {}

  /***/
  constexpr const std::string&   get_filename() const
  { return m_sFilename; }

  /***/
  inline openmode                get_mode() const
  { return m_modeFlags; }

  /***/
  inline csv_uint_t              get_bufsize() const
  { return m_bufSize; }

private:
  std::string       m_sFilename;
  openmode          m_modeFlags;
  const csv_uint_t   m_bufSize;
};

class csv_dev_file : public csv_device
{
public:
  /**
   * @brief csv_dev_file Constructs a csv_dev_file
   */
  explicit csv_dev_file( std::unique_ptr<csv_dev_file_options> ptrDeviceOptions, std::unique_ptr<csv_device_events> ptrEvents = nullptr);
  
  /***/
  virtual ~csv_dev_file();

  /**
   * \return _ok            Device open successfully or already open.
   *                        Also a call to a opened device will return the same value.
   * \return _no_devs       Specified device not found.
   * \return _access        Unable to access specified device.
   * \return _cfg_error     Error appling configuration on specified device.
   */
  virtual csv_result open() override;
  virtual csv_result send(const std::byte* pBuffer, csv_uint_t nBufferLen) override;
  /**
   * \return _rx_error
   * \return _rx_timedout
   */
  virtual csv_result recv( std::byte* pBuffer, csv_uint_t& nBufferLen) override;
  
  virtual csv_result close() override;

  /**
   * \return _ok                In case of success.
   */
  virtual csv_result flush() override;

  /**
   * \return _ok                Device is valid.
   */
  virtual csv_result is_valid() const override;

private:
  /***/
  csv_result       refresh_cache();
  /***/
  void             release();

private:
  FILE*        m_pFile;
  std::byte*   m_pRxBuffer;
  csv_uint_t   m_nCacheSize;
  csv_uint_t   m_nCursor;

};


} //inline namespace
} // namespace

#endif // CSV_DEV_FILE_H
