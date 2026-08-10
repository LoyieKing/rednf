// Reconstructed from binary disassembly (tsf4g_tdr runtime, project copy)
#ifndef SECSVR_SRC_PROTOCOL_COMMON_TDRPARSE_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_TDRPARSE_H_H_

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include <stdint.h>

namespace tsf4g_tdr {

class TdrParse {
public:
    typedef TdrError::ErrorType (*TdrParseMacro)(int32_t &, const char *, const char *);

    static TdrError::ErrorType parseUInt8(uint8_t &dest, const char *src, unsigned int *parsed,
                                          uint8_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt8(uint8_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                          uint8_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseInt8(int8_t &dest, const char *src, unsigned int *parsed,
                                         int8_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseInt8(int8_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                         int8_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt16(uint16_t &dest, const char *src, unsigned int *parsed,
                                           uint16_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt16(uint16_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                           uint16_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseInt16(int16_t &dest, const char *src, unsigned int *parsed,
                                          int16_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseInt16(int16_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                          int16_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt32(uint32_t &dest, const char *src, unsigned int *parsed,
                                           uint32_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt32(uint32_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                           uint32_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseInt32(int32_t &dest, const char *src, unsigned int *parsed,
                                          int32_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseInt32(int32_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                          int32_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt64(uint64_t &dest, const char *src, unsigned int *parsed,
                                           uint64_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUInt64(uint64_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                           uint64_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseInt64(int64_t &dest, const char *src, unsigned int *parsed,
                                          int64_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseInt64(int64_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                          int64_t _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseChar(char &dest, const char *src, unsigned int *parsed,
                                         char _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseChar(char *dest, unsigned int count, const char *src, unsigned int *parsed,
                                         char _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUChar(unsigned char &dest, const char *src, unsigned int *parsed,
                                          unsigned char _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseUChar(unsigned char *dest, unsigned int count, const char *src, unsigned int *parsed,
                                          unsigned char _default, TdrParseMacro macro, const char *groupName);
    static TdrError::ErrorType parseFloat(float &dest, const char *src, unsigned int *parsed, float _default);
    static TdrError::ErrorType parseFloat(float *dest, unsigned int count, const char *src, unsigned int *parsed,
                                          float _default);
    static TdrError::ErrorType parseDouble(double &dest, const char *src, unsigned int *parsed, double _default);
    static TdrError::ErrorType parseDouble(double *dest, unsigned int count, const char *src, unsigned int *parsed,
                                           double _default);
    static TdrError::ErrorType parseDate(tdr_date_t &dest, const char *src, unsigned int *parsed, const char *_default);
    static TdrError::ErrorType parseDate(tdr_date_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                         const char *_default);
    static TdrError::ErrorType parseTime(tdr_time_t &dest, const char *src, unsigned int *parsed, const char *_default);
    static TdrError::ErrorType parseTime(tdr_time_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                         const char *_default);
    static TdrError::ErrorType parseIP(tdr_ip_t &dest, const char *src, unsigned int *parsed, const char *_default);
    static TdrError::ErrorType parseIP(tdr_ip_t *dest, unsigned int count, const char *src, unsigned int *parsed,
                                       const char *_default);
};

} // namespace tsf4g_tdr

#endif // SECSVR_SRC_PROTOCOL_COMMON_TDRPARSE_H_H_
