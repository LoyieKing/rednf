// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_thread_error_info.cpp
// Address range 0x001ad390-0x001ad59c.
//
// Per-thread slot layout (0x104 bytes, malloc'ed lazily):
//   +0x00 int  last error code
//   +0x04 char msg[256]
// pthread_key_create is called with free() as the destructor -- the ORIG
// loads the GOT entry resolved to free@GLIBC_2.0 (0x00294484).
#include "src/commlib/zenlib/zen_thread_error_info.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

namespace ZEN_OS {
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
int pthread_key_delete(pthread_key_t key);
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
}

CThreadErrorInfo::CThreadErrorInfo() {
    ZEN_OS::pthread_key_create(&m_key_, free);
}

CThreadErrorInfo::~CThreadErrorInfo() {
    ZEN_OS::pthread_key_delete(m_key_);
}

CThreadErrorInfo::SErrorInfo *CThreadErrorInfo::last_error_info() {
    void *info = ZEN_OS::pthread_getspecific(m_key_);
    if (info == NULL) {
        info = malloc(0x104);
        if (info != NULL) {
            memset(info, 0, 0x104);
            ZEN_OS::pthread_setspecific(m_key_, info);
        }
    }
    return (CThreadErrorInfo::SErrorInfo *)info;
}
int CThreadErrorInfo::get_last_error_code() {
    return last_error_info()->error_code;
}

char *CThreadErrorInfo::get_last_error_msg() {
    return last_error_info()->error_msg;
}
void CThreadErrorInfo::set_last_error_msg(const char *fmt, ...) {
    char *info = last_error_info()->error_msg;
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(info, 0xff, fmt, ap);
    va_end(ap);
    if ((unsigned int)(ret - 1) <= 0xfe) {
        info[ret] = '\0';
    }
}

