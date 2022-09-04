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

#ifndef CSV_ROW_H
#define CSV_ROW_H

#include "csv_field.h"
#include "csv_header.h"


namespace csv {
inline namespace LIB_VERSION {



class csv_row : public csv_row_base_t
{
public:
  enum class flags_t : uint32_t {
    deleted  = 0x00000001,          // mark row as deleted 

  };
 
  csv_row() noexcept
    : m_flags(0)
  {}
  
  /***/
  constexpr inline uint32_t   get_flags() const noexcept
  { return m_flags; }

  /***/
  constexpr inline bool       test_flag ( flags_t flag ) const noexcept
  { return (m_flags & static_cast<uint32_t>(flag)); }

  /***/
  constexpr inline void       set_flag  ( flags_t flag ) noexcept
  { m_flags |= static_cast<uint32_t>(flag); }

  /***/
  constexpr inline void       unset_flag( flags_t flag ) noexcept
  { m_flags &= ~static_cast<uint32_t>(flag); }

  /***/
  inline const csv_field_t&  get_field(std::size_t index) const noexcept
  { return at(index); }
  /***/
  inline csv_field_t&        get_field(std::size_t index) noexcept
  { return at(index); }

  /***/
  inline csv_row& operator=( const csv_row& row ) noexcept 
  {
    this->clear();

    m_flags = row.m_flags;

    for ( const auto& item : row )
    { this->push_back( item ); }

    return *this;
  }

  /***/
  inline csv_row& operator=( csv_row&& row ) noexcept 
  {
    m_flags = row.m_flags;
    
    static_cast<csv_row_base_t&>(*this) = std::move(row);
    
    return *this;
  }
private:
  uint32_t      m_flags;

};

} //inline namespace
} // namespace

#endif //CSV_ROW_H
