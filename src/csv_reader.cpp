

#include "csv_reader.h"

namespace csv {
inline namespace LIB_VERSION {


csv_reader::csv_reader( std::unique_ptr<csv_device> ptrDevice )
  : csv_parser( std::move(ptrDevice) )
{}

bool csv_reader::open()
{
  return true; 
}

bool csv_reader::read( csv_row& row ) const
{
  return parse( row );
}

bool csv_reader::close()
{
  return true; 
}


} //inline namespace
} // namespace

