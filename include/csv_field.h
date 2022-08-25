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

#ifndef CSV_FIELD_H
#define CSV_FIELD_H

#include "csv_common.h"
#include "csv_data.h"
#include <vector>
#include <concepts>
#include <type_traits>

namespace csv {
inline namespace LIB_VERSION {

template<typename T>
concept String_t = requires (){
  std::is_object_v<T>;
};


template<String_t string_t>
class csv_field
{
public:
  /***/
  constexpr inline csv_field( csv_field&& item ) noexcept
    : m_bHasQuote(item.m_bHasQuote), m_sData(std::move(item.m_sData))
  { }

  /***/
  constexpr inline csv_field( string_t&& data, bool quoted ) noexcept
    : m_bHasQuote(quoted), m_sData(std::move(data))
  { }

  /***/
  constexpr inline csv_field( const string_t& data, bool quoted ) noexcept
    : m_bHasQuote(quoted), m_sData(data)
  { }

  /***/
  constexpr inline ~csv_field() noexcept
  {}

  /***/
  constexpr inline void             hasquotes( bool quoted ) noexcept
  { m_bHasQuote = quoted; }
  /***/
  constexpr inline bool             hasquotes() const noexcept
  { return m_bHasQuote; }
  /***/
  constexpr inline const string_t&  data() const noexcept
  { return m_sData; }
  /***/
  constexpr inline operator const string_t&() const noexcept
  { return m_sData; }

private:
  bool      m_bHasQuote;  
  string_t  m_sData;

};

typedef csv_data<char,size_t>         csv_data_t;
typedef csv_field<csv_data_t>         csv_field_t;
typedef std::vector<csv_field_t>      csv_row;


} //inline namespace
} // namespace

#endif //CSV_FIELD_H
