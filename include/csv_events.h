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

#include "csv_header.h"

namespace csv {
inline namespace CSV_LIB_VERSION {

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
  virtual void                    onBegin       () = 0;

  /**
   * @brief Invoked when header will be encountered in in r/w operations.
   *        Note: when reading if the header has been specified as external 
   *              then event will not be invoked.
   */
  virtual void                    onHeader      ( const csv_header& header ) = 0;

  /**
   * @brief   Invoked at each row on r/w operations.
   * @return  should be the row if not processed by the applicaiton, on return this buffer will
   *          be processed with filters if any have been registered.
   *          If we want to interrup processing discarding current row just return nullptr.
   */
  virtual csv_unique_ptr<csv_row> onRow         ( const csv_header& header, csv_unique_ptr<csv_row> row ) = 0;

  /**
   * @brief Invoked at each row but only if filters are enabled.
   */
  virtual csv_unique_ptr<csv_row> onFilteredRow ( const csv_header& header, csv_unique_ptr<csv_row> row ) = 0;

  /**
   * @brief Invoked when reading/writing operation have been completed.
   */
  virtual void                    onEnd         () = 0;

  /***/
  virtual void                    onError       ( csv_result eCode ) = 0;

  /**
   * @brief Invoked from append_field() method.  
   */
  virtual bool                    onAppendField ( const csv_header& header, const csv_field_t& label, const csv_field_t& defvalue ) = 0;

};

} //inline namespace
} // namespace

#endif //CSV_EVENTS_H
