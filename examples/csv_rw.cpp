
#include "csv_dev_file.h"
#include "csv_reader.h"
#include "csv_writer.h"
#include "csv_filters_chain.h"
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
  /***/  
  virtual void onBegin() override
  {
    std::cout << "BEGIN PARSING" << std::endl;
  }

  /***/
  virtual void                    onHeader      ( const csv_header& header ) override
  {
    std::cout << "  HEADER - columns = " << header.size() << std::endl;
  }

  /***/
  virtual csv_unique_ptr<csv_row> onRow         ( const csv_header& header, csv_unique_ptr<csv_row> row ) override
  {
    csv_field_t _label_second{ csv_data_t("second column"),false};

    std::cout << header.get_index( _label_second );
    std::cout << "  ROW  header size = " << header.size() << " - row_size = " << row->size() << std::endl;

    return row;
  }

  /***/
  virtual csv_unique_ptr<csv_row> onFilteredRow ( const csv_header& header, csv_unique_ptr<csv_row> row ) override
  {
    csv_field_t _label_second{ csv_data_t("second column"),false};

    std::cout << header.get_index( _label_second );
    std::cout << "  FILTERED ROW  header size = " << header.size() << " - row_size = " << row->size() << std::endl;

    for ( auto& field : (*row) )
    {
      std::cout << field.data().c_str() << " ";
    }
    std::cout << std::endl;

    return row;
  }

  /***/
  virtual void                    onEnd() override
  {
    std::cout << "END PARSING" << std::endl;
  }

  /***/
  virtual void                    onError( csv_result eCode ) override
  {
    std::cout << "ERROR CODE [" << (uint32_t)eCode << "]\n";
  }
};

class replace_bool_filter : public csv_filter_base
{
public:
  virtual BreakChain_t  filter( 
                                [[maybe_unused]] const std::string& feedname,
                                [[maybe_unused]] std::size_t        rownum,
                                [[maybe_unused]] std::size_t        index, 
                                [[maybe_unused]] const csv_header&  header, 
                                [[maybe_unused]] csv_row&           row 
                              )
  {
    if ( row[index].data() == "1" )
    { row[index].data() = "true"; }
    else
    if ( row[index].data() == "0" )
    { row[index].data() = "false"; }

    return BreakChain_t::no;
  }
};

class replace_empty_filter : public csv_filter_base
{
public:
  virtual BreakChain_t  filter( 
                                [[maybe_unused]] const std::string& feedname,
                                [[maybe_unused]] std::size_t        rownum,
                                [[maybe_unused]] std::size_t        index, 
                                [[maybe_unused]] const csv_header&  header, 
                                [[maybe_unused]] csv_row&           row 
                              )
  {
    if ( row[index].data().empty() )
    { row[index].data() = "empty"; }
    else
    if ( row[index].data() == "true" )
    {
      row[index].data() = "has been replaced";
    }

    return BreakChain_t::no;
  }
};

#include "devices/csv_dev_ostream.h"

int main( int argc, char* argv[] )
{
  /*
  unique_ptr<csv_dev_ostream_options<char>> _optOutput = std::make_unique<csv_dev_ostream_options<char>>( std::cout );
  unique_ptr<csv_dev_ostream<char>>         _devOutput = std::make_unique<csv_dev_ostream<char>>( std::move(_optOutput),nullptr);

_devOutput->send( reinterpret_cast<const unsigned char*>("ciao\n"), 5 );

return 0;
*/
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
  csv_reader reader( "csv reader", std::move(devInput), new reader_events() );

  reader.skip_whitespaces(true);
  reader.set_eol('\n');
  reader.set_quote('\"');
  reader.allow_comments(true);
  
  // Create filter chain for the "first column" in ./example/data/test.csv
  csv_filters_chain*  chain_first = new csv_filters_chain( csv_data_t("first column") ) ;
  chain_first->append( new replace_bool_filter()   );

  // Create filter chain for the "second column" in ./example/data/test.csv
  // note that replace_empty_filter will be called after replace_bool_filter
  // so that will work on data alredy altered by the first filter.
  csv_filters_chain*  chain_second = new csv_filters_chain( csv_data_t("second column") );
  chain_second->append( new replace_bool_filter()  );
  chain_second->append( new replace_empty_filter() );

  reader.set_filters( chain_first );
  reader.set_filters( chain_second );

  csv_row*    row = new csv_row();

  reader.open();
  while ( reader.read( *row ) )
  {
    reader.apply_filters( *row );

    for ( auto& field : (*row) )
    {
      std::cout << (field.data().empty()?"":field.data().c_str()) << " ";
    }
    std::cout << std::endl;

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
  csv_writer writer( "csv writer", std::move(devOutput), nullptr );

  writer.open( reader.get_header() );
  for ( const auto& row : vect_rows )
  {
    // in this specific case destination header matches with source header, but
    // in general destination header can be different in terms of fields and order in
    // which such filed are in the csv file.
    writer.write( reader.get_header(), *row );
  }
  writer.close();

  auto tew = chrono::steady_clock::now();
  print_duration( tsw, tew );
  cout << "-----------------END WRITING------------------" << endl << endl;
  
  return 0;
}
