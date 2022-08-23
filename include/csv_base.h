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

#ifndef CSV_BASE_H
#define CSV_BASE_H

#include "csv_field.h"
#include "csv_device.h"
#include "csv_events.h"
#include "csv_data.h"

#include <memory>

namespace csv {
inline namespace LIB_VERSION {



class csv_base
{
protected:
  /***/
  csv_base( core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents );
  /***/
  virtual ~csv_base();

public:
  /**
   * @brief Set field delimeter.
   *        Default value is comma ','.
   */
  constexpr inline void set_delimeter( char ch ) noexcept
  { m_cDelimeter = ch; }
  /**
   * @brief Get field delimeter. 
   * 
   * @return the character used between fields.
   */
  constexpr inline const char& get_delimeter() const noexcept
  { return m_cDelimeter; }

  /**
   * @brief Set quote delimeter.
   *        Default value is '\"'.
   */
  constexpr inline void set_quote( char ch ) noexcept
  { m_cQuote = ch; }
  /**
   * @brief Get quote delimeter. 
   * 
   * @return the character used between fields.
   */
  constexpr inline const char& get_quote() const noexcept
  { return m_cQuote; }

  /**
   * @brief Set End of Line marker.
   *        Default value is '\n'.
   */
  constexpr inline void set_eol( char ch ) noexcept
  { m_cEoL = ch; }

  /**
   * @brief Get the EoL (End of Line character). 
   * 
   * @return the character used to mark the end of current line.
   */
  constexpr inline const char& get_eol() const noexcept
  { return m_cEoL; }

protected:
  char                          m_cDelimeter;
  char                          m_cQuote;
  char                          m_cEoL;
  core::unique_ptr<csv_device>  m_ptrDevice;
  core::unique_ptr<csv_events>  m_ptrEvents;

};

} //inline namespace
} // namespace

#endif //CSV_BASE_H
