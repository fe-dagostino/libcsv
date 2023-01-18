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

#include "csv_writer.h"

namespace csv {
inline namespace CSV_LIB_VERSION {


csv_writer::csv_writer( const std::string& feedname, core::unique_ptr<csv_device> ptrDevice, core::unique_ptr<csv_events> ptrEvents )
  : csv_base( feedname, std::move(ptrDevice), std::move(ptrEvents) )
{}

bool csv_writer::open( const csv_header& header ) 
{
  set_header( header );

  const csv_header& _out_header = get_header();
  for ( size_t ndx = 0; ndx < _out_header.size(); ++ndx )
  {
    _in_write_field( _out_header.at(ndx) );
    
    if ( ndx < _out_header.size()-1 )
      m_ptrDevice->send( reinterpret_cast<const byte*>(&get_delimeter()), sizeof(char) );
    else
      m_ptrDevice->send( reinterpret_cast<const byte*>(&get_eol()), sizeof(char) );
  }

  return true;
}

bool csv_writer::write( const csv_header& header, const csv_row& row ) 
{
  const csv_header& _out_header = get_header();
  for ( size_t ndx = 0; ndx < _out_header.size(); ++ndx )
  {
    const auto& _field_name = _out_header.at(ndx);
    auto        _src_ndx    = header.get_index(_field_name);
    if ( _src_ndx != -1 )
    {
      _in_write_field( row.at(_src_ndx) );
    }
    
    if ( ndx < _out_header.size()-1 )
      m_ptrDevice->send( reinterpret_cast<const byte*>(&get_delimeter()), sizeof(char) );
    else
      m_ptrDevice->send( reinterpret_cast<const byte*>(&get_eol()), sizeof(char) );
  }

  return true;
}

bool csv_writer::close() 
{
  m_ptrDevice->close();
  return true; 
}

bool csv_writer::_in_write_field( const csv_field_t& field )
{
  if ( field.hasquotes() )
    m_ptrDevice->send( reinterpret_cast<const byte*>(&get_quote()), field.data().type_size() );

  if ( field.data().empty() == false )
  {
    m_ptrDevice->send( reinterpret_cast<const byte*>(field.data().data()), field.data().size() );
  }

  if ( field.hasquotes() )
    m_ptrDevice->send( reinterpret_cast<const byte*>(&get_quote()), field.data().type_size() );

  return true;
}

} //inline namespace
} // namespace

