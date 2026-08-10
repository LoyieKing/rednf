// Reconstructed from gunnersvr disassembly
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/TdrBufUtil.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"

#include <stdio.h>
#include <string.h>

namespace tsf4g_tdr {

// line 22
TdrError::ErrorType TdrBufUtil::printMultiStr(TdrWriteBuf &buf, const char *str, int times) {
    TdrError::ErrorType ret = TdrError::TDR_NO_ERROR;
    for (int i = 0; i < times; ++i) {
        size_t len = strlen(str);
        if (str == NULL) {
            return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        }
        if (buf.limit_ - buf.pos_ <= len) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        memmove(buf.data_ + buf.pos_, str, len);
        buf.pos_ += len;
    }
    return ret;
}

// line 42
TdrError::ErrorType TdrBufUtil::printVariable(TdrWriteBuf &buf, int indent, char sep,
                                              const char *variable, bool withSep) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (withSep) {
        ret = buf.textize("%s%c", variable, sep);
    } else {
        ret = buf.textize("%s: ", variable);
    }
    return ret;
}

// line 66
TdrError::ErrorType TdrBufUtil::printVariable(TdrWriteBuf &buf, int indent, char sep,
                                              const char *variable, unsigned int arrIdx, bool withSep) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (withSep) {
        ret = buf.textize("%s[%u]%c", variable, arrIdx, sep);
    } else {
        ret = buf.textize("%s[%u]: ", variable, arrIdx);
    }
    return ret;
}

// line 90
TdrError::ErrorType TdrBufUtil::printVariable(TdrWriteBuf &buf, int indent, char sep,
                                              const char *variable, const char *format, ...) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s: ", variable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    va_list va;
    va_start(va, format);
    int n = vsnprintf(buf.data_ + buf.pos_, buf.limit_ - buf.pos_, format, va);
    va_end(va);
    if (n < 0 || (size_t)n >= buf.limit_ - buf.pos_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.pos_ += n;
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 123
TdrError::ErrorType TdrBufUtil::printVariable(TdrWriteBuf &buf, int indent, char sep,
                                              const char *variable, unsigned int arrIdx,
                                              const char *format, ...) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s[%u]: ", variable, arrIdx);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    va_list va;
    va_start(va, format);
    int n = vsnprintf(buf.data_ + buf.pos_, buf.limit_ - buf.pos_, format, va);
    va_end(va);
    if (n < 0 || (size_t)n >= buf.limit_ - buf.pos_) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.pos_ += n;
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 156
TdrError::ErrorType TdrBufUtil::printArray(TdrWriteBuf &buf, int indent, char sep,
                                           const char *variable, int64_t count) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s[0 : %lld]:", variable, (long long)(count - 1));
    return ret;
}

// line 174
TdrError::ErrorType TdrBufUtil::printString(TdrWriteBuf &buf, int indent, char sep,
                                            const char *variable, const char *str) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s: ", variable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s", str);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 202
TdrError::ErrorType TdrBufUtil::printString(TdrWriteBuf &buf, int indent, char sep,
                                            const char *variable, unsigned int arrIdx, const char *str) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s[%u]: ", variable, arrIdx);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s", str);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 230
TdrError::ErrorType TdrBufUtil::printWString(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, const tdr_wchar_t *str) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s: ", variable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    size_t strLen = TdrTypeUtil::wstrlen(str);
    size_t i = 0;
    do {
        ret = buf.textize(" 0x%04x", str[i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ++i;
    } while (i != strLen + 1);
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 266
TdrError::ErrorType TdrBufUtil::printWString(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, unsigned int arrIdx,
                                             const tdr_wchar_t *str) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s[%u]: ", variable, arrIdx);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    size_t strLen = TdrTypeUtil::wstrlen(str);
    size_t i = 0;
    do {
        ret = buf.textize(" 0x%04x", str[i]);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        ++i;
    } while (i != strLen + 1);
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 302
TdrError::ErrorType TdrBufUtil::printTdrIP(TdrWriteBuf &buf, int indent, char sep,
                                           const char *variable, tdr_ip_t tdrIp) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s: ", variable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = TdrTypeUtil::tdrIp2Str(buf, tdrIp);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 330
TdrError::ErrorType TdrBufUtil::printTdrIP(TdrWriteBuf &buf, int indent, char sep,
                                           const char *variable, unsigned int arrIdx, tdr_ip_t tdrIp) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s[%u]: ", variable, arrIdx);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = TdrTypeUtil::tdrIp2Str(buf, tdrIp);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 358
TdrError::ErrorType TdrBufUtil::printTdrTime(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, tdr_time_t tdrTime) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s: ", variable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    TdrTime t;
    memcpy(&t, &tdrTime, sizeof(t));
    ret = buf.textize("%02d:%02d:%02d", (int)(int16_t)t.hour, t.minute, t.second);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 386
TdrError::ErrorType TdrBufUtil::printTdrTime(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, unsigned int arrIdx,
                                             tdr_time_t tdrTime) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s[%u]: ", variable, arrIdx);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    TdrTime t;
    memcpy(&t, &tdrTime, sizeof(t));
    ret = buf.textize("%02d:%02d:%02d", (int)(int16_t)t.hour, t.minute, t.second);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 414
TdrError::ErrorType TdrBufUtil::printTdrDate(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, tdr_date_t tdrDate) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s: ", variable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    TdrDate d;
    memcpy(&d, &tdrDate, sizeof(d));
    ret = buf.textize("%4d-%02d-%02d", (int)(int16_t)d.year, d.month, d.day);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 442
TdrError::ErrorType TdrBufUtil::printTdrDate(TdrWriteBuf &buf, int indent, char sep,
                                             const char *variable, unsigned int arrIdx,
                                             tdr_date_t tdrDate) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s[%u]: ", variable, arrIdx);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    TdrDate d;
    memcpy(&d, &tdrDate, sizeof(d));
    ret = buf.textize("%4d-%02d-%02d", (int)(int16_t)d.year, d.month, d.day);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 470
TdrError::ErrorType TdrBufUtil::printTdrDateTime(TdrWriteBuf &buf, int indent, char sep,
                                                 const char *variable, tdr_datetime_t tdrDateTime) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s: ", variable);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    TdrDateTime dt;
    memcpy(&dt, &tdrDateTime, sizeof(dt));
    ret = buf.textize("%4d-%02d-%02d", (int)(int16_t)dt.year, dt.month, dt.day);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 0) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = ' ';
    ret = buf.textize("%02d:%02d:%02d", (int)(int16_t)dt.hour, dt.minute, dt.second);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

// line 498
TdrError::ErrorType TdrBufUtil::printTdrDateTime(TdrWriteBuf &buf, int indent, char sep,
                                                 const char *variable, unsigned int arrIdx,
                                                 tdr_datetime_t tdrDateTime) {
    TdrError::ErrorType ret = printMultiStr(buf, "    ", indent);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    ret = buf.textize("%s[%u]: ", variable, arrIdx);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    TdrDateTime dt;
    memcpy(&dt, &tdrDateTime, sizeof(dt));
    ret = buf.textize("%4d-%02d-%02d", (int)(int16_t)dt.year, dt.month, dt.day);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 0) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = ' ';
    ret = buf.textize("%02d:%02d:%02d", (int)(int16_t)dt.hour, dt.minute, dt.second);
    if (ret != TdrError::TDR_NO_ERROR) {
        return ret;
    }
    if (buf.limit_ - buf.pos_ <= 1) {
        return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
    }
    buf.data_[buf.pos_++] = sep;
    buf.data_[buf.pos_] = '\0';
    return TdrError::TDR_NO_ERROR;
}

} // namespace tsf4g_tdr
