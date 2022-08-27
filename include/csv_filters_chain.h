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

#ifndef CSV_FILTER_CHAIN_H
#define CSV_FILTER_CHAIN_H

#include "csv_filter_base.h"
#include <map>

namespace csv {
inline namespace LIB_VERSION {

/**
 * @brief Each cvs_field_t can be subject to consistency check and / or manipulations.
 *        Primarily intended to be used with cvs_parser, moving the logic of each filter
 *        to the specific application, it allows having one filters chain for each 
 *        column in the csv data set. Data will be read from csv_device and then all 
 *        filters will be applied.
 */
class csv_filters_chain 
{
public:
  using filter_type    = csv_filter_base;
  using filter_ptr     = core::unique_ptr<filter_type>;
  using filters_type   = std::vector<filter_ptr>;

  /***/
  inline csv_filters_chain( const csv_data_t& field_name )
    : m_sFieldName(field_name)
  {}

  /***/
  inline virtual ~csv_filters_chain()
  {}

  /**
   * @brief Return field name matching with the column name in the cvs 
   *        data source. 
   */
  constexpr inline const csv_data_t&  label_name() const noexcept
  { return m_sFieldName; }

  /**
   * @brief Append a new filter. There is no "identity" control for the
   *        filter, so application level can one, two or more instances
   *        of the same filter and will be applied N-times ad for the 
   *        registered instances. 
   *        Note: filters will be applied in the same sequence they have
   *              been appended.
   * 
   * @param filter   unique pointer to filter instance. Ownership will 
   *                 be moved to csv_filter_chain an that instance.
   */
  inline void                         append( filter_ptr&& filter )
  { m_filters.emplace_back( std::move(filter) ); }

  /**
   * @brief Check if cvs_filter_chain is empty.
   * 
   * @return true   if empty
   * @return false  if at least one filter is stored.
   */
  inline bool                         empty()
  { return m_filters.empty(); }

  /**
   * @brief Clear all filter and release instances.
   *        After this call a call empty() will return true.
   */
  inline void                         clear()
  { m_filters.clear(); }

  /**
   * @brief Apply all registred filters in sequence, so (N+1) filter will work on data 
   *        already manipulated or just checked by filter (N).
   * 
   * @param index       field index, can be used to address fields in both header and row
   * @param header      headers cotaining lables. 
   * @param row         input/output parameters intended to be updated accordingly with 
   *                    filters results.
   */
  virtual void                        apply( [[maybe_unused]] std::size_t index, [[maybe_unused]] const csv_header& header, [[maybe_unused]] csv_row& row )
  {
    for ( auto& filter_base : m_filters )
    {
      if ( filter_base->filter( index, header, row ) == false )
        break;
    }
  }

private:
  const csv_data_t m_sFieldName;
  filters_type     m_filters;
};


} //inline namespace
} // namespace

#endif //CSV_FILTER_BASE_H
