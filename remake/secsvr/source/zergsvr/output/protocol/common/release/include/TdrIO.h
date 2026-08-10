// Reconstructed from binary disassembly (tsf4g_tdr runtime, project copy)
#ifndef SECSVR_SRC_PROTOCOL_COMMON_TDRIO_H_H_
#define SECSVR_SRC_PROTOCOL_COMMON_TDRIO_H_H_

#include "src/protocol/common/TdrError.h"
#include <stdarg.h>
#include <stdio.h>

namespace tsf4g_tdr {

class TdrOutStream {
public:
    // +0 FILE* fp_; +4 char* data_; +8 size_t pos_; +0xc size_t limit_;
    // +0x10 bool allocated_; +0x14 TdrError error_; sizeof = 0x18
    TdrOutStream() : fp_(NULL), data_(NULL), pos_(0), limit_(0), error_(TdrError::TDR_NO_ERROR) {}
    TdrOutStream(FILE *fp) : fp_(fp), data_(NULL), pos_(0), limit_(0), error_(TdrError::TDR_NO_ERROR) {}
    TdrOutStream(char *buf, size_t size)
        : fp_(NULL), data_(buf), pos_(0), limit_(size), allocated_(false), error_(TdrError::TDR_NO_ERROR) {}

    TdrError::ErrorType output(const char *src, size_t length);
    TdrError::ErrorType vtextize(const char *format, va_list ap);
    TdrError::ErrorType textize(const char *format, ...);

    TdrError::ErrorType error() const { return error_; }

    FILE *fp_;
    char *data_;
    size_t pos_;
    size_t limit_;
    bool allocated_;
    TdrError::ErrorType error_;
};

class TdrXmlFile {
public:
    TdrXmlFile() : buf_(NULL) {}
    ~TdrXmlFile() {
        if (buf_ != NULL) {
            delete[] buf_;
            buf_ = NULL;
        }
    }

    TdrError::ErrorType load(const char *file);
    char *buf() const { return buf_; }
    char *content() const { return buf_; }

private:
    char *buf_;
};

} // namespace tsf4g_tdr

#endif // SECSVR_SRC_PROTOCOL_COMMON_TDRIO_H_H_
