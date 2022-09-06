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

#include "csv_base.h"
#include <cstring>

namespace csv {
inline namespace LIB_VERSION {

csv_base::csv_base( const std::string& feedname )
  : csv_base( feedname, nullptr, nullptr )
{
}

csv_base::csv_base( const std::string& feedname, core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents )
  : m_sFeedName(feedname),
    m_cDelimeter(','), m_cQuote('\"'), m_cEoL('\n'), m_cComment('#'),
    m_ptrDevice( std::move(ptrDevice) ),
    m_ptrEvents( std::move(ptrEvents) ),
    m_vHeader()
{
}

csv_base::~csv_base()
{
}       

void csv_base::init( core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents )
{
  m_ptrDevice = std::move(ptrDevice);
  m_ptrEvents = std::move(ptrEvents);
}

bool csv_base::apply_filters( csv_row& row, std::size_t row_num ) const noexcept
{
  // Check if filters should be applied or not. 
  // Can be possible to have a filters chain for each field,
  // where the number of filters is defined at application level.
  if ( m_filters.empty() == true ) 
    return false;
  
  for ( std::size_t ndx = 0; ndx < m_vHeader.size(); ++ndx )
  {
    const csv_field_t& label   = m_vHeader.get_field(ndx);
    if ( m_filters.contains(label.data()) == false )
      continue;
    
    csv_filters_chain* filters = m_filters.at(label.data()).get();
    filters->apply( feed_name(), row_num, ndx, m_vHeader, row );
  }

  return true;
}

} //inline namespace
} // namespace
