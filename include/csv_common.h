
#ifndef CSV_COMMON_H
#define CSV_COMMON_H

#define LIB_VERSION  v1_0_0

#include <stdlib.h>
#include <memory>
#include <string>

namespace csv {
inline namespace LIB_VERSION {

typedef uint64_t    csv_uint_t;

template < csv_uint_t qnt = 1 >
struct to_bytes {
  enum { 
    Bytes  =    1 * qnt,
    KBytes = 1024 * qnt,
    MBytes = 1024 * 1024 * qnt,
    GBytes = 1024 * 1024 * 1024 * qnt
  };
};

} //inline namespace
} // namespace

#endif //CSV_COMMON_H
