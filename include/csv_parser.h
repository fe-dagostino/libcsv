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

#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "csv_base.h"
#include "csv_field.h"
#include "csv_data.h"
#include "csv_header.h"
#include "csv_filters_chain.h"

#include <memory>
#include <functional>


namespace csv {
inline namespace LIB_VERSION {

/**
 * @brief csv_parser support [RFC4180](https://datatracker.ietf.org/doc/rfc4180/)
 *        that de facto is the standard for CSV format, default behaviour can be
 *        modified changing parsing settings.
 *        Also comments on a line are supported, but disabled by default since
 *        RFC4180 do not take in account the presence of any comments inside the 
 *        csv file. There are different data source, most of them scientific data
 *        that come with line of comments inside the CVS, tipically such comments
 *        are identified with a '#' at the start of a new line and the whole line
 *        is considered to be a comment. 
 */
class csv_parser : public csv_base
{
  enum class Status : uint8_t { 
    eStart = 0, 
    eReadHeader = 1,
    eReadRows = 2,
    eEnd = 3 
  };

protected:
  /***/
  csv_parser( const std::string& feedname );

  /***/
  csv_parser( const std::string& feedname, core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents );

public:

  /**
   * @brief Set the whitespaces to be avoided. Default values are "\a\b\t\v\f\r\n".
   *        Note: if the list include also ' ' then all spaces will be removed
   *              and resulting string will without any space so " abc def " will
   *              be "abcdef", instead    
   * 
   * @param whitespaces 
   */
  inline           void               set_whitespaces( const std::string& whitespaces ) noexcept
  { m_sWhitespaces = whitespaces; }
  /***/
  constexpr inline const std::string& get_whitespaces( ) const noexcept
  { return m_sWhitespaces; }

  /***/
  constexpr inline void               skip_whitespaces( bool bSkip ) noexcept
  { m_bSkipWhitespaces = bSkip; }
  /***/
  constexpr inline bool               skip_whitespaces() const noexcept
  { return m_bSkipWhitespaces; }

  /***/
  constexpr inline void               trim_all( bool trim_all ) noexcept
  { m_bTrimAll = trim_all; }
  /***/
  constexpr inline bool               trim_all() const noexcept
  { return m_bTrimAll;   }

  /***/  
  constexpr inline void               allow_comments( bool bAllowed ) noexcept
  { m_bAllowComments = bAllowed; }
  /***/  
  constexpr inline bool               allow_comments() const noexcept
  { return m_bAllowComments; }

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
  inline bool                  set_header( csv_row&& header ) noexcept;
  /**
   * @brief Get the header. 
   * 
   * @return a const reference to internal header.
   */
  constexpr const csv_header&  get_header() const noexcept
  { return m_vHeader; }

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
    if ( m_filters.contains(filters->label_name())==true)
      return false;

    m_filters[filters->label_name()] = std::move(filters);

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

protected:
  /***/
  csv_result  parse( ) const noexcept;
  /***/
  csv_result  parse( csv_row& row ) const noexcept;
  /**
   * @brief Check if filters should be applied or not. 
   * 
   * @param row     input / output paramenter data will be directly modified by filters
   * @return true   if filters chains have been processed
   * @return false  if filters chains are empty so not applicable.
   */
  bool        apply_filters( csv_row& row ) const noexcept;

private:
  /***/
  csv_result  parse( csv_row* row ) const noexcept;
  /***/
  csv_result  parse_row( csv_row& row ) const noexcept;

  /**
   * @brief 
   * 
   * @param pFirst pointer to the first element. 
   *               Pointer will be updated the the first valid element.
   * @param pLast  pointer to the last element
   *               Pointer will be updated the the first valid element.
   * @param length current length
   * @return size_t new length of the string once all spaces are removed. 
   *         If the length returned is the same of current lenght no trim have beed done.
   */
  void        trim_all ( const char* & pFirst, const char* & pLast, size_t& length ) const noexcept;
  /***/
  bool        is_quoted( const char* & pFirst, const char* & pLast, size_t& length ) const noexcept;

private:
  using filters_map_t = std::unordered_map<std::string_view,core::unique_ptr<csv_filters_chain>>;

  mutable Status                                 m_eState;
  std::string                                    m_sWhitespaces;
  bool                                           m_bSkipWhitespaces;
  bool                                           m_bTrimAll;
  bool                                           m_bAllowComments;
  mutable csv_header                             m_vHeader;
  filters_map_t                                  m_filters;


  mutable std::array<byte,to_bytes<32>::KBytes>  m_recvCache;
  mutable std::size_t                            m_recvCachedBytes;
  mutable std::size_t                            m_recvCacheCursor;
  mutable csv_data<char,size_t>                  m_sData;
};

} //inline namespace
} // namespace

#endif //CSV_PARSER_H
