// Reconstructed from gunnersvr disassembly
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/TdrParse.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/protocol/common/TdrParse.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrTime.h"

#include <stdlib.h>
#include <string.h>

namespace tsf4g_tdr {

namespace {

// TdrParse 系列的分隔符（二进制 rodata：0x20 0x0d 0x0a 0x09 0x00）
const char TDR_PARSE_DELIM[] = " \r\n\t";

inline bool isNumberStart(char c) {
    return c >= '0' && c <= '9';
}

inline bool isSign(char c) {
    return c == '+' || c == '-';
}

} // namespace

TdrError::ErrorType TdrParse::parseIP(tdr_ip_t *dest, unsigned int count, const char *src,
                                      unsigned int *parsed, const char *_default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    unsigned int i;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        if (_default != NULL) {
            ret = TdrTypeUtil::str2TdrIP(dest[0], _default);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            for (i = 1; i < count; ++i) {
                dest[i] = dest[0];
            }
        } else {
            for (i = 0; i < count; ++i) {
                dest[i] = 0;
            }
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        ret = TdrTypeUtil::str2TdrIP(dest[parsedCount], segment);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (segment == NULL && parsedCount < count) {
        if (_default != NULL) {
            ret = TdrTypeUtil::str2TdrIP(dest[parsedCount], _default);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            for (i = parsedCount + 1; i < count; ++i) {
                dest[i] = dest[parsedCount];
            }
        } else {
            for (i = parsedCount; i < count; ++i) {
                dest[i] = 0;
            }
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseIP(tdr_ip_t &dest, const char *src, unsigned int *parsed,
                                      const char *_default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;

    if (segment != NULL) {
        ret = TdrTypeUtil::str2TdrIP(dest, segment);
        if (ret == TdrError::TDR_NO_ERROR) {
            parsedCount = 1;
        }
    }
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        if (_default != NULL) {
            return TdrTypeUtil::str2TdrIP(dest, _default);
        }
        dest = 0;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseTime(tdr_time_t *dest, unsigned int count, const char *src,
                                        unsigned int *parsed, const char *_default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    unsigned int i;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        if (_default != NULL) {
            ret = TdrTypeUtil::str2TdrTime(dest[0], _default);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            for (i = 1; i < count; ++i) {
                dest[i] = dest[0];
            }
        } else {
            for (i = 0; i < count; ++i) {
                dest[i] = 0;
            }
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        ret = TdrTypeUtil::str2TdrTime(dest[parsedCount], segment);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (segment == NULL && parsedCount < count) {
        if (_default != NULL) {
            ret = TdrTypeUtil::str2TdrTime(dest[parsedCount], _default);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            for (i = parsedCount + 1; i < count; ++i) {
                dest[i] = dest[parsedCount];
            }
        } else {
            for (i = parsedCount; i < count; ++i) {
                dest[i] = 0;
            }
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseTime(tdr_time_t &dest, const char *src, unsigned int *parsed,
                                        const char *_default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;

    if (segment != NULL) {
        ret = TdrTypeUtil::str2TdrTime(dest, segment);
        if (ret == TdrError::TDR_NO_ERROR) {
            parsedCount = 1;
        }
    }
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        if (_default != NULL) {
            return TdrTypeUtil::str2TdrTime(dest, _default);
        }
        dest = 0;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseDate(tdr_date_t *dest, unsigned int count, const char *src,
                                        unsigned int *parsed, const char *_default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    unsigned int i;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        if (_default != NULL) {
            ret = TdrTypeUtil::str2TdrDate(dest[0], _default);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            for (i = 1; i < count; ++i) {
                dest[i] = dest[0];
            }
        } else {
            for (i = 0; i < count; ++i) {
                dest[i] = 0;
            }
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        ret = TdrTypeUtil::str2TdrDate(dest[parsedCount], segment);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (segment == NULL && parsedCount < count) {
        if (_default != NULL) {
            ret = TdrTypeUtil::str2TdrDate(dest[parsedCount], _default);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            for (i = parsedCount + 1; i < count; ++i) {
                dest[i] = dest[parsedCount];
            }
        } else {
            for (i = parsedCount; i < count; ++i) {
                dest[i] = 0;
            }
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseDate(tdr_date_t &dest, const char *src, unsigned int *parsed,
                                        const char *_default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;

    if (segment != NULL) {
        ret = TdrTypeUtil::str2TdrDate(dest, segment);
        if (ret == TdrError::TDR_NO_ERROR) {
            parsedCount = 1;
        }
    }
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        if (_default != NULL) {
            return TdrTypeUtil::str2TdrDate(dest, _default);
        }
        dest = 0;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseDouble(double *dest, unsigned int count, const char *src,
                                          unsigned int *parsed, double _default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    double value;

    while (parsedCount < count && segment != NULL) {
        temp = NULL;
        value = strtod(segment, &temp);
        if (temp == NULL || temp == segment) {
            return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
        }
        dest[parsedCount] = value;
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        for (unsigned int i = parsedCount; i < count; ++i) {
            dest[i] = _default;
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseInt32(int32_t *dest, unsigned int count, const char *src,
                                         unsigned int *parsed, int32_t _default,
                                         TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int64_t int64bit;
    int32_t int32bit;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        for (unsigned int i = 0; i < count; ++i) {
            dest[i] = _default;
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            int64bit = strtoll(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
            if (int64bit < (-0x7FFFFFFF - 1) || int64bit > 0x7FFFFFFF) {
                return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
            }
            dest[parsedCount] = (int32_t)int64bit;
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            dest[parsedCount] = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        for (unsigned int i = parsedCount; i < count; ++i) {
            dest[i] = _default;
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUInt64(uint64_t *dest, unsigned int count, const char *src,
                                          unsigned int *parsed, uint64_t _default,
                                          TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    uint64_t value;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        for (unsigned int i = 0; i < count; ++i) {
            dest[i] = _default;
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtoull(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
            dest[parsedCount] = value;
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            dest[parsedCount] = (int64_t)int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        for (unsigned int i = parsedCount; i < count; ++i) {
            dest[i] = _default;
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUInt8(uint8_t *dest, unsigned int count, const char *src,
                                         unsigned int *parsed, uint8_t _default,
                                         TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        memset(dest, _default, count);
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)value > 0xFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest[parsedCount] = (uint8_t)value;
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        memset(dest + parsedCount, _default, count - parsedCount);
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUInt64(uint64_t &dest, const char *src, unsigned int *parsed,
                                          uint64_t _default, TdrParseMacro macro,
                                          const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    uint64_t value;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtoull(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
            dest = value;
            parsedCount = 1;
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            dest = (int64_t)int32bit;
            parsedCount = 1;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseDouble(double &dest, const char *src, unsigned int *parsed,
                                          double _default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    double value;

    if (segment != NULL) {
        temp = NULL;
        value = strtod(segment, &temp);
        if (temp == NULL || temp == segment) {
            return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
        }
        dest = value;
        parsedCount = 1;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseFloat(float *dest, unsigned int count, const char *src,
                                         unsigned int *parsed, float _default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    double value;

    while (parsedCount < count && segment != NULL) {
        temp = NULL;
        value = strtod(segment, &temp);
        if (temp == NULL || temp == segment) {
            return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
        }
        dest[parsedCount] = (float)value;
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        for (unsigned int i = parsedCount; i < count; ++i) {
            dest[i] = _default;
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseFloat(float &dest, const char *src, unsigned int *parsed,
                                         float _default) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    double value;

    if (segment != NULL) {
        temp = NULL;
        value = strtod(segment, &temp);
        if (temp == NULL || temp == segment) {
            return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
        }
        dest = (float)value;
        parsedCount = 1;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUInt32(uint32_t &dest, const char *src, unsigned int *parsed,
                                          uint32_t _default, TdrParseMacro macro,
                                          const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int64_t int64bit;
    int32_t int32bit;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            int64bit = strtoll(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
            if (int64bit < 0 || (uint64_t)int64bit > 0xFFFFFFFFULL) {
                return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
            }
            dest = (uint32_t)int64bit;
            parsedCount = 1;
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            dest = (uint32_t)int32bit;
            parsedCount = 1;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseInt32(int32_t &dest, const char *src, unsigned int *parsed,
                                         int32_t _default, TdrParseMacro macro,
                                         const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int64_t int64bit;
    int32_t int32bit;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            int64bit = strtoll(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
            if (int64bit < (-0x7FFFFFFF - 1) || int64bit > 0x7FFFFFFF) {
                return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
            }
            dest = (int32_t)int64bit;
            parsedCount = 1;
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            dest = int32bit;
            parsedCount = 1;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUInt32(uint32_t *dest, unsigned int count, const char *src,
                                          unsigned int *parsed, uint32_t _default,
                                          TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int64_t int64bit;
    int32_t int32bit;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        for (unsigned int i = 0; i < count; ++i) {
            dest[i] = _default;
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            int64bit = strtoll(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
            if (int64bit < 0 || (uint64_t)int64bit > 0xFFFFFFFFULL) {
                return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
            }
            dest[parsedCount] = (uint32_t)int64bit;
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            dest[parsedCount] = (uint32_t)int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        for (unsigned int i = parsedCount; i < count; ++i) {
            dest[i] = _default;
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseInt64(int64_t *dest, unsigned int count, const char *src,
                                         unsigned int *parsed, int64_t _default,
                                         TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    int64_t value;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        for (unsigned int i = 0; i < count; ++i) {
            dest[i] = _default;
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtoll(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
            dest[parsedCount] = value;
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            dest[parsedCount] = (int64_t)int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        for (unsigned int i = parsedCount; i < count; ++i) {
            dest[i] = _default;
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseInt64(int64_t &dest, const char *src, unsigned int *parsed,
                                         int64_t _default, TdrParseMacro macro,
                                         const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    int64_t value;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtoll(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
            dest = value;
            parsedCount = 1;
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            dest = (int64_t)int32bit;
            parsedCount = 1;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseInt16(int16_t &dest, const char *src, unsigned int *parsed,
                                         int16_t _default, TdrParseMacro macro,
                                         const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)(value + 0x8000) > 0xFFFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest = (int16_t)value;
        parsedCount = 1;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUInt16(uint16_t *dest, unsigned int count, const char *src,
                                          unsigned int *parsed, uint16_t _default,
                                          TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        for (unsigned int i = 0; i < count; ++i) {
            dest[i] = _default;
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)value > 0xFFFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest[parsedCount] = (uint16_t)value;
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        for (unsigned int i = parsedCount; i < count; ++i) {
            dest[i] = _default;
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseInt16(int16_t *dest, unsigned int count, const char *src,
                                         unsigned int *parsed, int16_t _default,
                                         TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        for (unsigned int i = 0; i < count; ++i) {
            dest[i] = _default;
        }
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)(value + 0x8000) > 0xFFFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest[parsedCount] = (int16_t)value;
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        for (unsigned int i = parsedCount; i < count; ++i) {
            dest[i] = _default;
        }
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUInt8(uint8_t &dest, const char *src, unsigned int *parsed,
                                         uint8_t _default, TdrParseMacro macro,
                                         const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)value > 0xFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest = (uint8_t)value;
        parsedCount = 1;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUInt16(uint16_t &dest, const char *src, unsigned int *parsed,
                                          uint16_t _default, TdrParseMacro macro,
                                          const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)value > 0xFFFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest = (uint16_t)value;
        parsedCount = 1;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseChar(char *dest, unsigned int count, const char *src,
                                        unsigned int *parsed, char _default,
                                        TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        memset(dest, (int)(unsigned char)_default, count);
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)(value + 0x80) > 0xFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest[parsedCount] = (char)value;
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        memset(dest + parsedCount, (int)(unsigned char)_default, count - parsedCount);
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseChar(char &dest, const char *src, unsigned int *parsed,
                                        char _default, TdrParseMacro macro,
                                        const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)(value + 0x80) > 0xFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest = (char)value;
        parsedCount = 1;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUChar(unsigned char *dest, unsigned int count, const char *src,
                                         unsigned int *parsed, unsigned char _default,
                                         TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        memset(dest, _default, count);
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)value > 0xFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest[parsedCount] = (unsigned char)value;
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        memset(dest + parsedCount, _default, count - parsedCount);
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseUChar(unsigned char &dest, const char *src, unsigned int *parsed,
                                         unsigned char _default, TdrParseMacro macro,
                                         const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)value > 0xFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest = (unsigned char)value;
        parsedCount = 1;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseInt8(int8_t *dest, unsigned int count, const char *src,
                                        unsigned int *parsed, int8_t _default,
                                        TdrParseMacro macro, const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment == NULL || count == 0) {
        parsedCount = 0;
        if (parsed != NULL) {
            *parsed = 0;
            return TdrError::TDR_NO_ERROR;
        }
        if (count == 0) {
            return TdrError::TDR_NO_ERROR;
        }
        memset(dest, (int)(unsigned char)_default, count);
        return TdrError::TDR_NO_ERROR;
    }

    while (parsedCount < count && segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)(value + 0x80) > 0xFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest[parsedCount] = (int8_t)value;
        ++parsedCount;
        segment = strtok_r(NULL, TDR_PARSE_DELIM, &context);
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount < count) {
        memset(dest + parsedCount, (int)(unsigned char)_default, count - parsedCount);
    }
    return TdrError::TDR_NO_ERROR;
}

TdrError::ErrorType TdrParse::parseInt8(int8_t &dest, const char *src, unsigned int *parsed,
                                        int8_t _default, TdrParseMacro macro,
                                        const char *groupName) {
    unsigned int parsedCount = 0;
    char *context = NULL;
    char *segment = strtok_r((char *)src, TDR_PARSE_DELIM, &context);
    char *temp;
    int32_t int32bit;
    long value;
    TdrError::ErrorType ret;

    if (segment != NULL) {
        char c = segment[0];
        if (isNumberStart(c) || isSign(c)) {
            temp = NULL;
            value = strtol(segment, &temp, 0);
            if (temp == NULL || temp == segment) {
                return TdrError::TDR_ERR_FAILED_TO_PARSE_NUMERIC;
            }
        } else if (macro != NULL) {
            int32bit = 0;
            ret = macro(int32bit, segment, groupName);
            if (ret != TdrError::TDR_NO_ERROR) {
                return ret;
            }
            value = int32bit;
        } else {
            return TdrError::TDR_ERR_UNDEFINED_MACRO_NAME;
        }
        if ((uint32_t)(value + 0x80) > 0xFF) {
            return TdrError::TDR_ERR_VALUE_UNDER_OR_OVER_FLOW;
        }
        dest = (int8_t)value;
        parsedCount = 1;
    }
    if (parsed != NULL) {
        *parsed = parsedCount;
        return TdrError::TDR_NO_ERROR;
    }
    if (parsedCount == 0) {
        dest = _default;
    }
    return TdrError::TDR_NO_ERROR;
}

} // namespace tsf4g_tdr
