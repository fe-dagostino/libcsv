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

#ifndef CSV_DEV_OSTREAM_H
#define CSV_DEV_OSTREAM_H

#include "csv_common.h"
#include "csv_device.h"
#include <iostream>
#include <assert.h>

namespace csv {
inline namespace CSV_LIB_VERSION {

template<typename char_t>
class csv_dev_ostream_options : public csv_device_options
{
public:
  using ostream_t = std::basic_ostream<char_t>;

  /**
   * Console Device options.
   */
  explicit csv_dev_ostream_options( ostream_t& ostream ) noexcept
  : csv_device_options(), m_ostream(ostream) 
  {}

  /***/
  virtual ~csv_dev_ostream_options() noexcept
  {}

  /***/
  inline ostream_t&  ostream() noexcept
  { return m_ostream; }

private:
  ostream_t&  m_ostream;
};

template<typename char_t>
class csv_dev_ostream : public csv_device
{
public:
  /**
   * @brief csv_dev_file Constructs a csv_dev_file
   */
  explicit csv_dev_ostream( core::unique_ptr<csv_dev_ostream_options<char_t>> ptrDeviceOptions, core::unique_ptr<csv_device_events> ptrEvents = nullptr)
      : csv_device( "csv_dev_ostream", std::move(ptrDeviceOptions), std::move(ptrEvents) )
  { assert( csv_device::get_options().get() != nullptr ); }  

  /***/
  virtual ~csv_dev_ostream()
  { flush(); }

  /**
   * \return _ok            Device open successfully or already open.
   *                        Also a call to a opened device will return the same value.
   * \return _no_devs       Specified device not found.
   * \return _access        Unable to access specified device.
   * \return _cfg_error     Error appling configuration on specified device.
   */
  virtual csv_result open() noexcept override
  { 
    m_devStats.rx     = 0;
    m_devStats.tx     = 0;
    m_devStats.errors = 0;

    return csv_result::_ok;
  }

  /***/
  virtual csv_result send(const byte* pBuffer, csv_uint_t nBufferLen) noexcept override
  {
    static_cast<csv_dev_ostream_options<char_t> *>(m_ptrOptions.get())
                        ->ostream().write( reinterpret_cast<const char_t*>(pBuffer), nBufferLen );
    m_devStats.tx += nBufferLen;
    return csv_result::_ok;
  }

  /**
   * \return _rx_error
   * \return _rx_timedout
   */
  virtual csv_result recv( [[maybe_unused]] byte* pBuffer, [[maybe_unused]] csv_uint_t& nBufferLen) noexcept override
  { return csv_result::_not_implemented; }

  virtual csv_result close() noexcept override
  { return csv_result::_ok; }

  /**
   * \return _ok                In case of success.
   */
  virtual csv_result flush() noexcept override
  {
    static_cast<csv_dev_ostream_options<char_t> *>(m_ptrOptions.get())
                        ->ostream().flush();
    return csv_result::_ok;
  }

  /**
   * \return _ok                Device is valid.
   */
  virtual csv_result is_valid() const noexcept override
  { return csv_result::_ok; }

private:

};


} //inline namespace
} // namespace

#endif // CSV_DEV_OSTREAM_H
