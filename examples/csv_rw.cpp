
#include "csv_dev_file.h"
#include "csv_reader.h"
#include "csv_writer.h"
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

class reader_events : public csv_events
{
  virtual void onBegin() override
  {
    std::cout << "BEGIN PARSING" << std::endl;
  }

  virtual void onHeader( const csv_header& header ) override
  {
    std::cout << "  HEADER - columns = " << header.size() << std::endl;
  }

  virtual void onRow   ( const csv_header& header, const csv_row& row ) override
  {
    csv_field_t _label_second{{"second column",13},false};

    std::cout << header.get_index( _label_second );
    std::cout << "  ROW  header size = " << header.size() << " - row_size = " << row.size() << std::endl;
  }

  virtual void onEnd() override
  {
    std::cout << "END PARSING" << std::endl;
  }

  /***/
  virtual void onError( csv_result eCode ) override
  {
    std::cout << "ERROR CODE [" << (uint32_t)eCode << "]\n";
  }
};


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
  cout << "----------------------------------------------" << endl;
  cout << "-------------------READING--------------------" << endl;
  auto ts = chrono::steady_clock::now();

  unique_ptr<csv_dev_file_options> optInput = std::make_unique<csv_dev_file_options>( filename, 
                                                                                      csv_dev_file_options::openmode::read, 
                                                                                      to_bytes<8>::MBytes );
  unique_ptr<csv_dev_file>         devInput = std::make_unique<csv_dev_file>( std::move(optInput),nullptr);

  std::vector<csv_row*>  vect_rows;
  csv_reader reader( std::move(devInput), new reader_events() );

  reader.skip_whitespaces(true);
  reader.set_eol('\n');
  reader.set_quote('\"');
  reader.allow_comments(true);
  
  csv_row*    row = new csv_row();

  reader.open();
  while ( reader.read( *row ) )
  {
    vect_rows.push_back( row );
    row = new csv_row();
  }
  reader.close();

  auto te = chrono::steady_clock::now();
  print_duration( ts, te );
  
  cout << endl;
  cout << "Retrieved [" << vect_rows.size() << "] rows" << endl;
  cout << "-----------------END READING------------------" << endl << endl;
  cout << "----------------------------------------------" << endl;
  cout << "-------------------WRITING--------------------" << endl;
  auto tsw = chrono::steady_clock::now();

  unique_ptr<csv_dev_file_options> optOutput = std::make_unique<csv_dev_file_options>( filename + ".output", 
                                                                                      csv_dev_file_options::openmode::write, 
                                                                                      to_bytes<8>::MBytes,
                                                                                      csv_dev_file_options::filetype::UTF_8 );
  unique_ptr<csv_dev_file>         devOutput = std::make_unique<csv_dev_file>( std::move(optOutput),nullptr);
  csv_writer writer( std::move(devOutput), nullptr );

  writer.open( reader.get_header() );
  for ( auto row: vect_rows )
  {
    writer.write( *row );
  }
  writer.close();

  auto tew = chrono::steady_clock::now();
  print_duration( tsw, tew );
  cout << "-----------------END WRITING------------------" << endl << endl;
  
  return 0;
}
