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

#ifndef CSV_FILTER_BASE_H
#define CSV_FILTER_BASE_H

#include "csv_common.h"
#include "csv_data.h"
#include <unordered_map>

namespace csv {
inline namespace LIB_VERSION {


/**
 * @brief Base class intended to be extended at appliacation level
 *        in order to implements specific filtering logic.
 *        csv_filter_base instances should registered in 
 *        csv_filter_chain.
 */
class csv_filter_base
{
public:
  /***/
  constexpr inline csv_filter_base() {}

  /***/
  constexpr inline virtual ~csv_filter_base() {}

  /**
   * @brief Perform controls and eventually modify field at specified index.
   * 
   * @param index       field index, can be used to address fields in both header and row
   * @param header      headers cotaining lables. 
   * @param row         input/output parameters intended to be updated accordingly with 
   *                    filters results.
   * 
   * @return true       to continue with next filter
   * @return false      to interrupt appling filters.
   */
  virtual bool  filter( [[maybe_unused]] std::size_t index, [[maybe_unused]] const csv_header& header, [[maybe_unused]] csv_row& row ) = 0;

};


} //inline namespace
} // namespace

#endif //CSV_FILTER_BASE_H
