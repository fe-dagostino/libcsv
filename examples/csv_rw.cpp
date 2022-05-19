
#include "csv_dev_file.h"
#include "csv_reader.h"
#include <iostream>
#include <iomanip>    // needed by setprecision
#include <chrono>
#include <memory>



using namespace std;
using namespace csv;
typedef chrono::steady_clock::time_point tp;

void print_duration( tp ts, tp te )
{
  std::chrono::duration<double> duration =  (te - ts);
  cout << setprecision(9) << "Duration = " << duration.count() << "s ≈ " 
  << (duration.count() * 1000      ) << "ms ≈ "  
  << (duration.count() * 1000000   ) << "µs ≈ "
  << (duration.count() * 1000000000) << "ns   " << endl;
}


int main( int argc, char* argv[] )
{
  if (argc < 2 )
  {
    printf( "Usage:                         \n" );
    printf( "      ./csv_rw <csv filename>  \n" );
    printf( "                               \n" );
    return 0;
  }

  // Get filename to be parsed from command line arguments
  std::string filename = argv[1];

  // timestamp
  auto ts = chrono::steady_clock::now();


  unique_ptr<csv_dev_file_options> opt = std::make_unique<csv_dev_file_options>( filename, 
                                                                                 csv_dev_file_options::openmode::read, 
                                                                                 to_bytes<8>::MBytes );
  unique_ptr<csv_dev_file>         dev = std::make_unique<csv_dev_file>( std::move(opt),nullptr);

  std::vector<csv_row*>  rows;
  csv_reader reader( std::move(dev) );

  reader.skip_whitespaces(false);
  
  csv_row*    row = new csv_row();

  while ( reader.read( *row ) )
  {
    rows.push_back( row );
    row = new csv_row();
  }

  auto te = chrono::steady_clock::now();
  print_duration( ts, te );
  
  cout << rows.size() << endl;
  
  for( auto& field : reader.get_header() )
  {
    cout <<  field.data().c_str() << ",";
  }
  cout << endl;

  return 0;
}
