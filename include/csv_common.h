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

#ifndef CSV_COMMON_H
#define CSV_COMMON_H

#define LIB_VERSION  v1_0_0

#include <stdlib.h>
#include <memory>
#include <string>

namespace csv {
inline namespace LIB_VERSION {

typedef uint64_t    csv_uint_t;
typedef uint8_t     byte;

template < csv_uint_t qnt = 1 >
struct to_bytes {
  enum : uint64_t { 
    Bytes  =    1u * qnt,
    KBytes = 1024u * qnt,
    MBytes = 1024u * 1024u * qnt,
    GBytes = 1024u * 1024u * 1024u * qnt
  };
};

enum class csv_result {
  _ok              = 0x0000,

  _not_implemented = 0x0001,
  _wrong_call      = 0x0002,

  _closed          = 0x1001,
  _no_devs         = 0x1002,
  _access          = 0x1003,
  _cfg_error       = 0x1004,
  _no_mem          = 0x1005,
  _eof             = 0x1006,

  _tx_error        = 0x0007,
  _rx_error        = 0x0008,
  _tx_timedout     = 0x0009,
  _rx_timedout     = 0x000A,
  _conn_timeout    = 0x000B,
  _bom_mismatch    = 0x000C,
  _row_items_error = 0x000D,  

  _wrong_protocol  = 0x2001,
  _missing_soh,
  _wrong_soh,
  _wrong_crc,
  _empty_sentence

};


} //inline namespace
} // namespace

#endif //CSV_COMMON_H
