


#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "csv_field.h"
#include "csv_device.h"
#include "csv_data.h"

#include <memory>
#include <functional>
#include <map>

namespace csv {
inline namespace LIB_VERSION {



class csv_parser
{
  enum class Status : uint8_t { 
    eStart = 0, 
    eReadHeader = 1,
    eReadLine = 2,
    eEnd = 3 
  };

protected:
  /***/
  csv_parser( std::unique_ptr<csv_device>&& ptrDevice );

public:
  /**
   * @brief Set field separator.
   *        Default value is comma ','.
   */
  constexpr void set_delimeter( char ch )
  { m_cSeparator = ch; }
  /**
   * @brief Get the separator. 
   * 
   * @return the character used between fields.
   */
  constexpr char get_delimeter() const
  { return m_cSeparator; }

  /**
   * @brief Set End of Line marker.
   *        Default value is '\n'.
   */
  constexpr void set_eol( char ch )
  { m_cEoL = ch; }

  /**
   * @brief Get the EoL (End of Line character). 
   * 
   * @return the character used to mark the end of current line.
   */
  constexpr char get_eol() const
  { return m_cEoL; }

  /***/
  inline    void               set_whitespaces( const std::string& whitespaces )
  { m_sWhitespaces = whitespaces; }
  /***/
  constexpr const std::string& get_whitespaces( ) const
  { return m_sWhitespaces; }

  /***/
  constexpr    void            skip_whitespaces( bool bSkip )
  { m_bSkipWhitespaces = bSkip; }
  /***/
  constexpr bool               skip_whitespaces() const
  { return m_bSkipWhitespaces; }

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
  inline bool                  set_header( csv_row&& header );
  /**
   * @brief Get the header. 
   * 
   * @return a const reference to internal header.
   */
  constexpr const csv_row&     get_header() const
  { return m_vHeader; }



protected:
  /***/
  bool        parse( csv_row& row ) const;

private:
  /***/
  csv_result  parse_row( csv_row& row ) const;

private:
  mutable Status                                     m_eState;
  char                                               m_cSeparator;
  char                                               m_cEoL;
  std::string                                        m_sWhitespaces;
  bool                                               m_bSkipWhitespaces;
  mutable csv_row                                    m_vHeader;
  std::unique_ptr<csv_device>                        m_ptrDevice;

  mutable std::array<std::byte,to_bytes<32>::KBytes> m_recvCache;
  mutable std::size_t                                m_recvCachedBytes;
  mutable std::size_t                                m_recvCacheCursor;
  mutable csv_data<char,size_t>                      m_sData;
};

} //inline namespace
} // namespace

#endif //CSV_PARSER_H
