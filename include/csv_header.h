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

#ifndef CSV_HEADER_H
#define CSV_HEADER_H

#include "csv_row.h"
#include <unordered_map>


namespace csv {
inline namespace LIB_VERSION {


class csv_header : public csv_row
{
public:
  using field_index_t   = int32_t;
  using field_map_t     = std::unordered_map<std::string_view,field_index_t>;

  /***/
  csv_header()
  {}

  /**
   * @brief Initialize the header with specified cvs_row.
   *        Row is assuming to contains column labels as 
   *        for the first row in a CSV.
   * 
   * @param labels  column labels, usually the first line in the csv.
   * @return true   if the header was previously initialized.
   * @return false  if the header was empty.
   */
  inline bool init( csv_row&& labels ) noexcept
  {
    bool  ret = empty();

    static_cast<csv_row&>(*this) = std::move(labels);
    
    m_label_2_index.clear();
    for ( size_t ndx = 0; ndx < size(); ++ndx )
    {
      m_label_2_index[ this->at(ndx).data() ] = ndx;  
    }

    return ret;
  }

  /**
   * @brief Check if specified label is part of the header.
   * 
   * @return true   if the label is present.
   * @return false  if the label is not present.
   */
  inline bool          contains( const csv_data_t& label ) noexcept
  { return m_label_2_index.contains(label); }

  /**
   * @brief Get the column index for the specified label.
   *        This method check if the label is present.
   * 
   * @param label  the column label
   * @return zero based index if the label exist, -1 if the 
   *         do not exist.  
   */
  inline field_index_t get_index( const csv_data_t& label ) const noexcept
  { 
    if (m_label_2_index.contains(label)==false)
      return -1;

    return m_label_2_index.at(label); 
  }

  /**
   * @brief Same as method get_index() but with two main differences
   *        - there is no boundary check, so if the label exist or not 
   *          user will get a value back 
   *        - calling the operator on an missing label have a side effect
   *          to create a new entry in the map.
   *         
   * NOTE: this operatator is intended for performance and must be used only
   *       if the caller know that label is part of the header.
   */
  inline field_index_t operator[](const csv_data_t& label)
  { return m_label_2_index[label]; }

private:
  field_map_t     m_label_2_index;

};

} //inline namespace
} // namespace

#endif //CSV_HEADER_H
