
#ifndef CSV_READER_H
#define CSV_READER_H

#include "csv_common.h"
#include "csv_parser.h"

namespace csv {
inline namespace LIB_VERSION {

/***/
class csv_reader : public csv_parser
{
public:
  /***/
  csv_reader( std::unique_ptr<csv_device> ptrDevice );

  /***/
  bool open();

  /***/
  bool read( csv_row& row ) const;

  /***/
  bool close();

private:
  

};

} //inline namespace
} // namespace

#endif //CSV_READER_H
