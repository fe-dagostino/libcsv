
#include "csv_dev_file.h"
#include "csv_reader.h"
#include <iostream>
#include <iomanip>    // needed by setprecision
#include <chrono>
#include <memory>


#include <csv_data.h>



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


int main(/* int argc, char* argv[]*/ )
{
  const size_t      _nStringLength = to_bytes<30>::KBytes;
  const size_t      _nRepeat       = 100000;
  const char        _ch            = 'A';


  auto ts_data = chrono::steady_clock::now();
  {
    csv_data<char,size_t,to_bytes<512>::Bytes> test_tmp;
    test_tmp.reserve(4048);
    csv_data<char,size_t,to_bytes<512>::Bytes> test( std::move(test_tmp) );

    for ( size_t _counter = 0; _counter < _nRepeat; ++_counter )
    {
      test.clear();
      for ( size_t _cntPush = 0; _cntPush < _nStringLength; ++_cntPush )
      {
        test.push_back(_ch);
      }
    }
  }
  auto te_data = chrono::steady_clock::now();

  auto ts_string = chrono::steady_clock::now();
  {
    std::string test;

    for ( size_t _counter = 0; _counter < _nRepeat; ++_counter )
    {
      test.clear();
      for ( size_t _cntPush = 0; _cntPush < _nStringLength; ++_cntPush )
      {
        test.push_back(_ch);
      }
    }
  }
  auto te_string = chrono::steady_clock::now();

  cout << "Benchmark with csv_data" << endl;
  print_duration(ts_data,te_data);
  cout << "Benchmark with std::string" << endl;
  print_duration(ts_string,te_string);

  return 0;
}
