// CThreadErrorInfo, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_thread_error_info.cpp
//
// ORIG DWARF: sizeof(CThreadErrorInfo) == 4, single member
//   pthread_key_t m_key;   // +0x00
// The key's per-thread block is 0x104 bytes: an int error code at +0x00
// followed by a 256-byte message buffer at +0x04.
#ifndef TSS_RECON_ZEN_THREAD_ERROR_INFO_H_
#define TSS_RECON_ZEN_THREAD_ERROR_INFO_H_

#include <pthread.h>

class CThreadErrorInfo {
public:
    struct SErrorInfo {
        int error_code;
        char error_msg[256];
    };

    CThreadErrorInfo();
    ~CThreadErrorInfo();

    SErrorInfo *last_error_info();
    int get_last_error_code();
    char *get_last_error_msg();
    void set_last_error_msg(const char *fmt, ...);

private:
    pthread_key_t m_key_;
};

#endif // TSS_RECON_ZEN_THREAD_ERROR_INFO_H_
