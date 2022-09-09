[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/language-c++-red.svg)](https://en.cppreference.com/)
[![CMake](https://github.com/fe-dagostino/libcsv/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/fe-dagostino/libcsv/actions/workflows/cmake.yml)


## Table of Contents

* [Introduction](#introduction)
* [Why libcsv](#why-libcsv)
  * [custom input/output](#custom-input--output)
  * [Control and Normalization](#control-and-normalization)
* [Performances](#performances)
* [Build from source](#build-from-source)
* [Contributing](#contributing)
* [License](#license)

# Introduction

This is a *cross-platform* library intended to provide the tools for easily reading, manipulating and writing `csv` files and more in general `csv` data format.

`libcsv` leverage optimizations introduced starting from `C++20`, so this is the minimum requirement for the compiler in order to successful build the library.

# Why libcsv?

There are many libraries like this one that allow users to read and write `csv` files, anyway there are a couple of useful features that makes this library ideal for generic usages.

## Custom Input / Output

Both input and output operation leverage the interface offered by `cvs_device`, this means that at application level could be possible to specialize this class allowing reading and writing from/to any media eg. *files*, *sockets*, *database*, *shared memory* and so on... .
Since the common source for the `csv` is coming from files this specialization is already provided with the libray.

```cpp
  // Full name for the csv file with relative or absolute path. 
  std::string src_filename = "source.csv";

  // Options required by specific device
  std::unique_ptr<csv::csv_dev_file_options> opt_input = std::make_unique<csv::csv_dev_file_options>( 
                            src_filename,                                 // filename 
                            csv::csv_dev_file_options::openmode::read,    // access mode 
                            to_bytes<8>::MBytes                           // cache buffer size
                        );

  // Create an instance of csv::csv_dev_file 
  std::unique_ptr<csv::csv_dev_file>  dev_input = std::make_unique<csv::csv_dev_file>( 
                            std::move(opt_input),                         // change ownership for opt_input
                            nullptr                                       // pointer to csv_device_events, 
                                                                          // in this case we are not providing it
                        );

  // Here we are, we can now use dev_input to create an instance either of csv::parser or csv::reader.
  
```

Same mechanism can be applied for `csv::writer`, with the only difference that in this case our device should have writing rights.

## Control and Normalization

It is quite common to have data source that is not properly filled or where some fields are missed out, and most of the time there is a manual process to find all "incongruences" and eventually to fix them.  
With filters it is possible to detect all incongruence during the parsing phase, row by row, and for each single occurrence or use cases it is possible to implement some logic to fix such incongruence. 
For example, suppose we have a CSV file with the following three columns "date", "stock name" and "price", now when we receive the this csv with an undefined amount of rows and enough to be not manageable with tools such as *excel* or *open office calc*, we also know that in some cases, when price is not available we receive an empty field for the price, but the program we use is expecting to have -1 and not an empty field. Such kind of situations can be easily detected and fixed writing a filter in which if we have "price" empty then we replace with -1. 
Let's try to implement this filter and we will call it `replace_empty_price`.

```cpp
/**
 * This class is a specialization for the csv::csv_filter_base interface and we should
 * register an instance of such filter for the field "price"
*/
class replace_empty_price : public csv::csv_filter_base
{
public:
  virtual BreakChain_t  filter( 
      [[maybe_unused]] const std::string&      feedname,  
      [[maybe_unused]] std::size_t             rownum,   // row number, header considered
      [[maybe_unused]] std::size_t             index,    // field index in both header and row
      [[maybe_unused]] const csv::csv_header&  header,   // header containing all column labels
      [[maybe_unused]] csv::csv_row&           row       // containing all value for current row
                              )
  {
    if ( row[index].empty() == true )    // this will match "price" field
    { row[index].data() = "-1"; }        // replacing the old value with -1 as for our needs.

    return BreakChain_t::no;
  }
};
```

This is a very simple use case, it could be more complex using other field values on the same row or even more complex using information collected until this specific row is analysed.

A starting point for writing your own program can be [csv_rw.cpp](./examples/csv_rw.cpp) that you can find in the *examples* directory.

# Performances

Measurements reported in this paragraph have been done using the following hardware:

| **Item** | **Description**                                    |
|----------|----------------------------------------------------|
| OS       | Kernel 5.17.5 SMP PREEMPT x86_64 GNU/Linux         |
| CPU      | 11th Gen Intel(R) Core(TM) i7-11800H @ 2.30GHz     |
| RAM      | 32 GB DDR4                                         |
| DISK     | Samsung SSD 980 PRO 1TB                            |

Parsing a `csv`, that means reading from the file and tokenizing each single item, process data at the speed 
of **~1.2 GB/s** with all check enabled and without to be affected from file size or row size. 
Allocating `rows` and `fields` drop down processing speed to **~200 MB/s**. Such performances are measured 
with all check enabled, just disabling witespaces increase processing speed to **~280 MB/s**.

In the following table some `benchmarks` optained with `cvs_rw` example program on some random `datasets`. 
Reported time include:
* Reading from file
* Parsing the csv with all check enabled
* Allocating each field in memory

| Dataset                  | File Size |  Time |
|--------------------------|-----------|-------|
| [AGE, GENDER AND ETHNICITY](https://www.kaggle.com/datasets/nipunarora8/age-gender-and-ethnicity-face-data-csv) | 199.7 MB | 0.5467s |
| [HPA - Processed Train Dataframe With Cell-Wise RLE](https://www.kaggle.com/datasets/dschettler8845/hpa-processed-train-dataframe-with-cellwise-rle)| 2.6 GB| 7.6748s |


# Build from source

In order to build the library the following are required:
* a compiler with C++20 support
* cmake >= 3.16
* git *(optional)*  

Source code can be retrieved from github cloning the repository with `git` or downloading `.zip` archive.

```bash
# git clone https://github.com/fe-dagostino/libcsv.git
# cd libcsv
# mkdir build
# cmake ../ -DCMAKE_BUILD_TYPE=Release
# make
```

## Contributing
Anyone that would like contributing on the library will be more than welcome.

## License
This project is available under the [MIT](LICENSE) license.