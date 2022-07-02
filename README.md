
## Table of Contents

*    [Introduction](#introduction)
*    [Performances](#performances)
*    [Build from source](#build-from-source)
*    [Contributing](#contributing)
*    [License](#license)

# Introduction

This is a *cross-platform* library intended to provide the tools for reading, manipulating and writing `csv` files easily.

`libcsv` leverage optimizations introduced starting from `C++20`, so this is the minimum requirement for the compiler in order to successful build the library.

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