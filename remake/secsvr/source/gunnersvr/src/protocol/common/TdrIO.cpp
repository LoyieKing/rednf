// Reconstructed from gunnersvr disassembly
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/TdrIO.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/protocol/common/TdrIO.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrError.h"
#include <new>

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrOutStream::output(const char *src, size_t length) {
    if (fp_ != NULL) {
        size_t written = fwrite(src, 1, length, fp_);
        if (written < length) {
            error_ = TdrError::TDR_ERR_WRITE_FILE_FAILED;
        }
        return error_;
    }
    if (src == NULL) {
        error_ = TdrError::TDR_ERR_ARG_POINTER_IS_NULL;
        return error_;
    }
    if (length > limit_ - pos_) {
        error_ = TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        return error_;
    }
    memmove(data_ + pos_, src, length);
    pos_ += length;
    error_ = TdrError::TDR_NO_ERROR;
    return error_;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrOutStream::vtextize(const char *format, va_list ap) {
    if (fp_ != NULL) {
        int n = vfprintf(fp_, format, ap);
        if (n < 0) {
            error_ = TdrError::TDR_ERR_WRITE_FILE_FAILED;
        }
        return error_;
    }
    int n = vsnprintf(data_ + pos_, limit_ - pos_, format, ap);
    if (n < 0 || (size_t)n >= limit_ - pos_) {
        error_ = TdrError::TDR_ERR_SHORT_BUF_FOR_WRITE;
        return error_;
    }
    pos_ += n;
    error_ = TdrError::TDR_NO_ERROR;
    return error_;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrOutStream::textize(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    TdrError::ErrorType ret = vtextize(format, ap);
    va_end(ap);
    return ret;
}

tsf4g_tdr::TdrError::ErrorType tsf4g_tdr::TdrXmlFile::load(const char *file) {
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        return TdrError::TDR_ERR_OPEN_FILE_READ_FAILED;
    }
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buf_ = new (std::nothrow) char[length + 1];
    if (buf_ == NULL) {
        fclose(fp);
        return TdrError::TDR_ERR_ALLOC_MEMORY_FAILED;
    }
    if (fread(buf_, 1, length, fp) != (size_t)length) {
        fclose(fp);
        return TdrError::TDR_ERR_READ_FILE_FAILED;
    }
    buf_[length] = 0;
    fclose(fp);
    return TdrError::TDR_NO_ERROR;
}
