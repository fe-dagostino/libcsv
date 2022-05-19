
#ifndef CSV_FIELD_H
#define CSV_FIELD_H

#include "csv_common.h"
#include "csv_data.h"
#include <vector>

namespace csv {
inline namespace LIB_VERSION {


template<typename string_t>
class csv_field
{
public:
  /***/
  csv_field( csv_field&& item )
    : m_bHasQuote(item.m_bHasQuote), m_sData(std::move(item.m_sData))
  {  }

  /***/
  csv_field( const string_t& data, bool quoted )
    : m_bHasQuote(quoted), m_sData(data)
  {
  }

  /***/
  csv_field( string_t&& data, bool quoted )
    : m_bHasQuote(quoted), m_sData(std::move(data))
  {
  }

  /***/
  ~csv_field()
  {}

  /***/
  constexpr bool               hasquotes() const
  { return m_bHasQuote; }
  /***/
  constexpr const string_t& data() const
  { return m_sData; }

private:
  bool      m_bHasQuote;  
  string_t  m_sData;

};

typedef std::vector<csv_field<csv_data<char,size_t>>>                 csv_row;


} //inline namespace
} // namespace

#endif //CSV_FIELD_H
