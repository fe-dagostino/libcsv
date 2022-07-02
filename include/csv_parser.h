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

#include <memory>
#include <functional>
#include <map>

namespace csv {
inline namespace LIB_VERSION {



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
  csv_parser( std::unique_ptr<csv_device> ptrDevice, std::unique_ptr<csv_events> ptrEvents );

public:

  /**
   * @brief Set the whitespaces to be avoided. Default values are "\a\b\t\v\f\r\n".
   *        Note: if the list include also ' ' then all spaces will be removed
   *              and resulting string will without any space so " abc def " will
   *              be "abcdef", instead    
   * 
   * @param whitespaces 
   */
  inline    void               set_whitespaces( const std::string& whitespaces ) noexcept
  { m_sWhitespaces = whitespaces; }
  /***/
  constexpr const std::string& get_whitespaces( ) const noexcept
  { return m_sWhitespaces; }

  /***/
  constexpr void               skip_whitespaces( bool bSkip ) noexcept
  { m_bSkipWhitespaces = bSkip; }
  /***/
  constexpr bool               skip_whitespaces() const noexcept
  { return m_bSkipWhitespaces; }

  /***/
  constexpr void               trim_all( bool trim_all ) noexcept
  { m_bTrimAll = trim_all; }
  /***/
  constexpr bool               trim_all() const noexcept
  { return m_bTrimAll;   }
    
  
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
  constexpr const csv_row&     get_header() const noexcept
  { return m_vHeader; }

protected:
  /***/
  csv_result  parse( csv_row& row ) const noexcept;

private:
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
  mutable Status                                     m_eState;
  std::string                                        m_sWhitespaces;
  bool                                               m_bSkipWhitespaces;
  bool                                               m_bTrimAll;
  mutable csv_row                                    m_vHeader;

  mutable std::array<byte,to_bytes<32>::KBytes>      m_recvCache;
  mutable std::size_t                                m_recvCachedBytes;
  mutable std::size_t                                m_recvCacheCursor;
  mutable csv_data<char,size_t>                      m_sData;
};

} //inline namespace
} // namespace

#endif //CSV_PARSER_H
