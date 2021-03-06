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

#ifndef CSV_EVENTS_H
#define CSV_EVENTS_H

#include "csv_field.h"

namespace csv {
inline namespace LIB_VERSION {

class csv_events
{
protected:
  /***/
  constexpr csv_events()
  {}

public:
  /***/
  virtual ~csv_events() {}

  /**
   * @brief Invoked when reading/writing operation start.
   */
  virtual void onBegin() = 0;

  /**
   * @brief Invoked when header will be encountered in in r/w operations.
   *        Note: when reading if the header has been specified as external 
   *              then event will not be invoked.
   */
  virtual void onHeaders( const csv_row& header ) = 0;

  /**
   * @brief Invoked at each row on r/w operations.
   */
  virtual void onRow    ( const csv_row& header, const csv_row& row ) = 0;

  /**
   * @brief Invoked when reading/writing operation have been completed.
   */
  virtual void onEnd() = 0;

  /***/
  virtual void onError( csv_result eCode ) = 0;

};

} //inline namespace
} // namespace

#endif //CSV_EVENTS_H
