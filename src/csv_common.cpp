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

#include "csv_common.h"

#include <unordered_map>

namespace csv {
inline namespace CSV_LIB_VERSION {

// lookup table for presence_t
static const std::unordered_map<csv_result, std::string> s_mapResults = { 
                                                  { csv_result::_ok              , "ok"                       },

                                                  { csv_result::_not_implemented , "not implemented"          },
                                                  { csv_result::_wrong_call      , "wrong call sequence"      },

                                                  { csv_result::_closed          , "closed"                   },
                                                  { csv_result::_no_devs         , "no device"                },
                                                  { csv_result::_access          , "access"                   },
                                                  { csv_result::_cfg_error       , "configuration error"      },
                                                  { csv_result::_no_mem          , "no enough memory"         },
                                                  { csv_result::_eof             , "eof"                      },

                                                  { csv_result::_tx_error        , "write error"              },
                                                  { csv_result::_rx_error        , "read error"               },
                                                  { csv_result::_tx_timedout     , "write timeout"            },
                                                  { csv_result::_rx_timedout     , "read timeout"             },
                                                  { csv_result::_conn_timeout    , "connection timeout"       },
                                                  { csv_result::_bom_mismatch    , "BOM mismatch"             },
                                                  { csv_result::_row_items_error , "inconsistent items count" },

                                                  { csv_result::_wrong_protocol  , "wrong protocol"           },
                                                  { csv_result::_missing_soh     , "missing soh"              },
                                                  { csv_result::_wrong_soh       , "wrong soh"                },
                                                  { csv_result::_wrong_crc       , "wrong crc"                },
                                                  { csv_result::_empty_sentence  , "empty sentence"           }
                                              };

std::string  result2string( csv_result res ) noexcept
{
  if ( s_mapResults.contains(res) )
    return s_mapResults.at(res);

  return std::to_string( static_cast<uint16_t>(res) );
}

} //inline namespace
} // namespace
