// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_addr_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_BASE_H_H_

#include <stddef.h>
#include <unistd.h>

// sizeof = 12
struct ZEN_Sockaddr { // line 10
public:
int (***_vptr.ZEN_Sockaddr)(...);
sockaddr *sockaddr_ptr_;
socklen_t sockaddr_size_;
ZEN_Sockaddr(const ZEN_Sockaddr &arg0);
ZEN_Sockaddr(sockaddr *arg0, int arg1); // line 6
virtual ~ZEN_Sockaddr(); // line 14
virtual void set_sockaddr(sockaddr *arg0, socklen_t arg1); // line 19
socklen_t get_size() const; // line 52
void set_size(int arg0); // line 57
void set_addr(sockaddr *arg0); // line 63
sockaddr * get_addr() const; // line 68
bool operator==(const ZEN_Sockaddr &others_sockaddr) const; // line 21
bool operator!=(const ZEN_Sockaddr &others_sockaddr) const; // line 32
const char * tostr(char *buffer, size_t buf_len) const; // line 37
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_BASE_H_H_
