// Reconstructed from binary disassembly (tsf4g_tdr runtime, project copy)
#ifndef SECSVR_SRC_PROTOCOL_COMMON_TDRTIME_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_TDRTIME_H_H_

#include "src/protocol/common/TdrPal.h"

namespace tsf4g_tdr {

// TdrDate wire value (uint32): year | month<<16 | day<<24
struct TdrDate { // sizeof = 4
    uint16_t year;
    uint8_t month;
    uint8_t day;

    bool isValid() const;
};

// TdrTime wire value (uint32): hour | minute<<16 | second<<24
struct TdrTime { // sizeof = 4
    uint16_t hour;
    uint8_t minute;
    uint8_t second;

    bool isValid() const;
};

// TdrDateTime wire value (uint64):
//   year | month<<16 | day<<24 | hour<<32 | minute<<40 | second<<48
struct TdrDateTime { // sizeof = 8
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint16_t hour;
    uint8_t minute;
    uint8_t second;

    bool isValid() const;
};

} // namespace tsf4g_tdr

#endif // SECSVR_SRC_PROTOCOL_COMMON_TDRTIME_H_H_
