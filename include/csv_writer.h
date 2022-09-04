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

#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "csv_base.h"

namespace csv {
inline namespace LIB_VERSION {

/***/
class csv_writer : public csv_base
{
public:
  /**
   * @brief Construct a new csv writer object
   * 
   * @param feedname   name used to identify the feed.
   * @param ptrDevice  output device used for writing.
   * @param ptrEvents  events for callback events and details on each single operation. 
   *                   This parameter can be nullptr but in such case the application rely only
   *                   on return values.
   */
  csv_writer( const std::string& feedname, core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents );

  /**
   * @brief 
   * 
   * @param out_header 
   * @return true 
   * @return false 
   */
  bool open( const csv_header& out_header );

  /***/
  bool write( const csv_header& header, const csv_row& row );

  /***/
  bool close();

private:
  /***/
  bool _in_write_field( const csv_field_t& field );

};

} //inline namespace
} // namespace

#endif //CSV_WRITER_H
