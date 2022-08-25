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
 
  csv_row()
    : m_flags(0)
  {}
  
  /***/
  constexpr inline uint32_t   get_flags() const
  { return m_flags; }

  /***/
  constexpr inline bool       test_flag ( flags_t flag ) const
  { return (m_flags & static_cast<uint32_t>(flag)); }

  /***/
  constexpr inline void       set_flag  ( flags_t flag )
  { m_flags |= static_cast<uint32_t>(flag); }

  /***/
  constexpr inline void       unset_flag( flags_t flag )
  { m_flags &= ~static_cast<uint32_t>(flag); }

private:
  uint32_t      m_flags;


};

} //inline namespace
} // namespace

#endif //CSV_ROW_H
