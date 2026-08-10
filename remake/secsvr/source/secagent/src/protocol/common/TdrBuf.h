// Reconstructed from binary disassembly (tsf4g_tdr runtime, project copy)
// TdrWriteBuf / TdrReadBuf: fixed-buffer big-endian serialization.
// Layout (confirmed from inlined code):
//   TdrWriteBuf: +0 char* data_; +4 size_t pos_; +8 size_t limit_; +0xc bool allocated_
//   TdrReadBuf : +0 const char* data_; +4 size_t pos_; +8 size_t limit_
#ifndef SECSVR_SRC_PROTOCOL_COMMON_TDRBUF_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_TDRBUF_H_H_

#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrPal.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace tsf4g_tdr {

class TdrWriteBuf {
public:
    friend class TdrBufUtil;

    TdrWriteBuf() : data_(NULL), pos_(0), limit_(0), allocated_(false) {}

    TdrWriteBuf(char *buf, size_t size)
        : data_(buf), pos_(0), limit_(size), allocated_(false) {}

    ~TdrWriteBuf() {
        if (allocated_ && data_ != NULL) {
            delete[] data_;
            data_ = NULL;
        }
    }

    TdrError::ErrorType attach(char *buf, size_t size) {
        if (buf == NULL) {
            return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        }
        data_ = buf;
        pos_ = 0;
        limit_ = size;
        allocated_ = false;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeChar(char v) {
        if (limit_ - pos_ <= 0) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos_++] = v;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeUInt8(uint8_t v) {
        if (limit_ - pos_ <= 0) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos_++] = (char)v;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeUInt16(uint16_t v) {
        if (limit_ - pos_ <= 1) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos_++] = (char)(v >> 8);
        data_[pos_++] = (char)(v & 0xff);
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeUInt32(uint32_t v) {
        if (limit_ - pos_ <= 3) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        data_[pos_++] = (char)(v >> 24);
        data_[pos_++] = (char)((v >> 16) & 0xff);
        data_[pos_++] = (char)((v >> 8) & 0xff);
        data_[pos_++] = (char)(v & 0xff);
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeUInt64(uint64_t v) {
        if (limit_ - pos_ <= 7) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        for (int i = 7; i >= 0; --i) {
            data_[pos_++] = (char)((v >> (i * 8)) & 0xff);
        }
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeFloat(float v) {
        union {
            float f;
            uint32_t u;
        } u;
        u.f = v;
        return writeUInt32(u.u);
    }

    TdrError::ErrorType writeDouble(double v) {
        union {
            double d;
            uint64_t u;
        } u;
        u.d = v;
        return writeUInt64(u.u);
    }

    TdrError::ErrorType writeString(const char *str) {
        if (str == NULL) {
            return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        }
        size_t len = strlen(str) + 1;
        if (limit_ - pos_ <= 3) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        size_t start = pos_ + 4;
        pos_ = start;
        if (limit_ - pos_ < len) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        memmove(data_ + pos_, str, len);
        pos_ += len;
        uint32_t be = (uint32_t)(((uint32_t)len >> 24) & 0xff) | ((((uint32_t)len >> 16) & 0xff) << 8) |
                      ((((uint32_t)len >> 8) & 0xff) << 16) | (((uint32_t)len & 0xff) << 24);
        memcpy(data_ + start - 4, &be, 4);
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType writeWString(const tdr_wchar_t *str) {
        if (str == NULL) {
            return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        }
        size_t len = 0;
        while (str[len] != 0) {
            ++len;
        }
        size_t bytes = (len + 1) * 2;
        if (limit_ - pos_ <= 3) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        size_t start = pos_ + 4;
        pos_ = start;
        if (limit_ - pos_ < bytes) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        for (size_t i = 0; i <= len; ++i) {
            data_[pos_++] = (char)(str[i] >> 8);
            data_[pos_++] = (char)(str[i] & 0xff);
        }
        uint32_t be = (uint32_t)(((uint32_t)bytes >> 24) & 0xff) | ((((uint32_t)bytes >> 16) & 0xff) << 8) |
                      ((((uint32_t)bytes >> 8) & 0xff) << 16) | (((uint32_t)bytes & 0xff) << 24);
        memcpy(data_ + start - 4, &be, 4);
        return TdrError::TDR_NO_ERROR;
    }

    // line 177 (original header): out-of-line weak symbol in the binary
    TdrError::ErrorType textize(const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        int n = vsnprintf(data_ + pos_, limit_ - pos_, format, ap);
        va_end(ap);
        if (n < 0 || (size_t)n >= limit_ - pos_) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        }
        pos_ += n;
        return TdrError::TDR_NO_ERROR;
    }

    size_t size() const { return pos_; }
    size_t capacity() const { return limit_; }
    const char *data() const { return data_; }

private:
    char *data_;
    size_t pos_;
    size_t limit_;
    bool allocated_;
};

class TdrReadBuf {
public:
    TdrReadBuf() : data_(NULL), pos_(0), limit_(0) {}

    TdrReadBuf(const char *buf, size_t size)
        : data_(buf), pos_(0), limit_(size) {}

    TdrError::ErrorType attach(const char *buf, size_t size) {
        if (buf == NULL) {
            return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        }
        data_ = buf;
        pos_ = 0;
        limit_ = size;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readChar(char &v) {
        if (limit_ - pos_ <= 0) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        v = data_[pos_++];
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readUInt8(uint8_t &v) {
        if (limit_ - pos_ <= 0) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        v = (uint8_t)data_[pos_++];
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readUInt16(uint16_t &v) {
        if (limit_ - pos_ <= 1) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        v = (uint16_t)(((uint8_t)data_[pos_] << 8) | (uint8_t)data_[pos_ + 1]);
        pos_ += 2;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readUInt32(uint32_t &v) {
        if (limit_ - pos_ <= 3) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        v = ((uint32_t)(uint8_t)data_[pos_] << 24) |
            ((uint32_t)(uint8_t)data_[pos_ + 1] << 16) |
            ((uint32_t)(uint8_t)data_[pos_ + 2] << 8) |
            (uint32_t)(uint8_t)data_[pos_ + 3];
        pos_ += 4;
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readUInt64(uint64_t &v) {
        if (limit_ - pos_ <= 7) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        v = 0;
        for (int i = 0; i < 8; ++i) {
            v = (v << 8) | (uint8_t)data_[pos_++];
        }
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readFloat(float &v) {
        uint32_t u;
        TdrError::ErrorType ret = readUInt32(u);
        if (ret == TdrError::TDR_NO_ERROR) {
            union {
                float f;
                uint32_t u;
            } x;
            x.u = u;
            v = x.f;
        }
        return ret;
    }

    TdrError::ErrorType readDouble(double &v) {
        uint64_t u;
        TdrError::ErrorType ret = readUInt64(u);
        if (ret == TdrError::TDR_NO_ERROR) {
            union {
                double d;
                uint64_t u;
            } x;
            x.u = u;
            v = x.d;
        }
        return ret;
    }

    TdrError::ErrorType readString(char *dest) {
        if (limit_ - pos_ <= 3) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        uint32_t len;
        TdrError::ErrorType ret = readUInt32(len);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (limit_ - pos_ < len) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        if (len > 0x800) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        if (len == 0) {
            return TdrError::TDR_ERR_STR_LEN_TOO_SMALL;
        }
        if (dest == NULL) {
            return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        }
        memmove(dest, data_ + pos_, len);
        pos_ += len;
        if (dest[len - 1] != 0) {
            return TdrError::TDR_ERR_STR_LEN_CONFLICT;
        }
        if (strlen(dest) + 1 != len) {
            return TdrError::TDR_ERR_STR_LEN_CONFLICT;
        }
        return TdrError::TDR_NO_ERROR;
    }

    TdrError::ErrorType readWString(tdr_wchar_t *dest) {
        if (limit_ - pos_ <= 3) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        uint32_t len;
        TdrError::ErrorType ret = readUInt32(len);
        if (ret != TdrError::TDR_NO_ERROR) {
            return ret;
        }
        if (limit_ - pos_ < len) {
            return TdrError::TDR_ERR_SHORT_BUF_FOR_READ;
        }
        if (len > 0x800) {
            return TdrError::TDR_ERR_STR_LEN_TOO_BIG;
        }
        if (len == 0) {
            return TdrError::TDR_ERR_STR_LEN_TOO_SMALL;
        }
        if (dest == NULL) {
            return TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        }
        size_t count = len / 2;
        for (size_t i = 0; i < count; ++i) {
            dest[i] = (tdr_wchar_t)(((uint8_t)data_[pos_] << 8) | (uint8_t)data_[pos_ + 1]);
            pos_ += 2;
        }
        if (dest[count - 1] != 0) {
            return TdrError::TDR_ERR_STR_LEN_CONFLICT;
        }
        return TdrError::TDR_NO_ERROR;
    }

    size_t size() const { return pos_; }
    size_t capacity() const { return limit_; }
    const char *data() const { return data_; }

private:
    const char *data_;
    size_t pos_;
    size_t limit_;
};

} // namespace tsf4g_tdr

#endif // SECSVR_SRC_PROTOCOL_COMMON_TDRBUF_H_H_
