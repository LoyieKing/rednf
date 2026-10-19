// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_thread_error_info.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_ERROR_INFO_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_ERROR_INFO_H_H_

#include <bits/pthreadtypes.h>

// sizeof = 4
struct CThreadErrorInfo { // line 11
    // sizeof = 260
    struct SErrorInfo {
    public:
    int error_code;
    char error_msg[];
    };
public:
static const unsigned int ERR_MSG_MAX_LEN;
private:
pthread_key_t m_key;
public:
CThreadErrorInfo(); // line 13
~CThreadErrorInfo(); // line 14
CThreadErrorInfo::SErrorInfo * last_error_info(); // line 26
void set_last_error_msg(const char *arg0, ...); // line 29
const char * get_last_error_msg(); // line 32
const int get_last_error_code(); // line 35
};
extern const unsigned int ERR_MSG_MAX_LEN; // line 17
namespace CThreadErrorInfo {
    // sizeof = 260
    struct SErrorInfo { // line 20
    public:
    int error_code;
    char error_msg[];
    };
} // namespace CThreadErrorInfo

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_ERROR_INFO_H_H_
