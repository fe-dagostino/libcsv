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

csv_base::csv_base( core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents )
  : m_cDelimeter(','), m_cQuote('\"'), m_cEoL('\n'),
    m_ptrDevice( std::move(ptrDevice) ),
    m_ptrEvents( std::move(ptrEvents) )
{
}

csv_base::~csv_base()
{
  if ( dynamic_cast<csv_base*>(m_ptrDevice.get()) ==  this )
  {
    m_ptrDevice.release();
  }

  if ( dynamic_cast<csv_base*>(m_ptrEvents.get()) ==  this )
  {
    m_ptrEvents.release();
  }
}       


} //inline namespace
} // namespace
