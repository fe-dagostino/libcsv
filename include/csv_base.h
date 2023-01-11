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
#include "csv_filters_chain.h"

#include <memory>

namespace csv {
inline namespace LIB_VERSION {


/**
 * @brief 
 * 
 */
class csv_base
{
protected:
  /***/
  csv_base( const std::string& feedname );
  /***/
  csv_base( const std::string& feedname, core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents );
  /***/
  virtual ~csv_base();

  /***/
  void init( core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents );

public:
  /**
   * @brief Get curent feed name.
   */
  constexpr inline const std::string& feed_name() const noexcept
  { return m_sFeedName; }

  /**
   * @brief Set field delimeter.
   *        Default value is comma ','.
   */
  constexpr inline void        set_delimeter( char ch ) noexcept
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
  constexpr inline void        set_quote( char ch ) noexcept
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
  constexpr inline void        set_eol( char ch ) noexcept
  { m_cEoL = ch; }

  /**
   * @brief Get the EoL (End of Line character). 
   * 
   * @return the character used to mark the end of current line.
   */
  constexpr inline const char& get_eol() const noexcept
  { return m_cEoL; }

  /**
   * @brief Set line comment marker.
   *        Default value is '#' at the beginning of the line.
   */
  constexpr inline void        set_comment( char ch ) noexcept
  { m_cComment = ch; }

  /**
   * @brief Get comment marker. 
   * 
   * @return the character used to mark start of a comment line.
   */
  constexpr inline const char& get_comment() const noexcept
  { return m_cComment; }

  /**
   * @brief Set the header structure. Each line should match with number of field specified in the header.
   *        Note: in case no header will be specified before a call to parse() method then the first line 
   *              will be supposed to be the header as default behaviour.
   * 
   * @param header specify header to be set. Note if the function return true parameter datas
   *               will be moved to data memeber, so its content will be empty after the call
   *               to set_header() return.
   * @return true  if the header is not already set
   * @return false if the header is already valorized, in such case parameter will be ignore.
   */
  inline bool                  set_header( csv_row&& header ) noexcept
  { return m_vHeader.init( std::move(header) ); }
  /* set_header by copy **/
  inline bool                  set_header( const csv_row& header ) noexcept
  { return m_vHeader.init( header ); }
  /**
   * @brief Get the header. 
   * 
   * @return a const reference to internal header.
   */
  constexpr const csv_header&  get_header() const noexcept
  { return m_vHeader; }

  /***/
  bool                         append_field( const csv_field_t& label, const csv_field_t& defvalue ) noexcept
  {
    if ( m_vHeader.contains( label ) == true )
      return false;

    m_vHeader.append_label( label );

    if (m_ptrEvents)
    {
      return m_ptrEvents->onAppendField( m_vHeader, label, defvalue);
    }

    return true;
  }

  /**
   * @brief Register a filters chain to be used with a specified colum
   *        of the csv data set.
   * 
   * @param filters    csv_filters_chain, if the method return true, ownership will be moved to 
   *                   the csv_parser, but in case of failure it will be released with all
   *                   filters in it.
   * @return true      filters_chain has been registered. @fileters pointer will be updated to nullptr.
   * @return false     a filters_chain with the same label has been previously registered. 
   */
  inline bool                  set_filters( core::unique_ptr<csv_filters_chain> filters ) noexcept
  {
    if ( m_filters.contains( static_cast<std::string_view>(filters->label_name()) )==true)
      return false;

    m_filters[ static_cast<std::string_view>(filters->label_name()) ] = std::move(filters);

    return true;
  }

  /**
   * @brief Clear all csv_filter_chain and release resources.
   */
  inline bool                  has_filters() const noexcept
  { return !m_filters.empty(); }

  /**
   * @brief Clear all csv_filter_chain and release resources.
   */
  inline void                  clear_filters() noexcept
  { m_filters.clear(); }

  /**
   * @brief Check if filters should be applied or not. 
   * 
   * @param row     input / output paramenter data will be directly modified by filters
   * @param row_num specify current row number. This information will be provided to each filter
   *                in order to have more precise information. Default value is 0 since we may do
   *                not have this information.
   * @return true   if filters chains have been processed
   * @return false  if filters chains are empty so not applicable.
   */
  bool                         apply_filters( csv_row& row, std::size_t row_num = 0 ) const noexcept;

protected:
  using filters_map_t = std::unordered_map<std::string_view,core::unique_ptr<csv_filters_chain>>;

  std::string                   m_sFeedName;
  char                          m_cDelimeter;
  char                          m_cQuote;
  char                          m_cEoL;
  char                          m_cComment;
  core::unique_ptr<csv_device>  m_ptrDevice;
  core::unique_ptr<csv_events>  m_ptrEvents;
  mutable csv_header            m_vHeader;
  filters_map_t                 m_filters;
};

} //inline namespace
} // namespace

#endif //CSV_BASE_H
