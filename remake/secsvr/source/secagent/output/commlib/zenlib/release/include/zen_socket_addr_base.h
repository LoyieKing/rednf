// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_addr_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_BASE_H_H_

#include <stddef.h>
#include <sys/socket.h>
#include <unistd.h>

// sizeof = 12
struct ZEN_Sockaddr { // line 10
public:
// 注：DWARF 桩曾显式声明 _vptr 成员（非法语法）；虚函数由编译器自动生成 vptr，
// 去掉显式成员后布局不变（vptr + sockaddr_ptr_ + sockaddr_size_ = 12 字节）。
sockaddr *sockaddr_ptr_;
socklen_t sockaddr_size_;
ZEN_Sockaddr(const ZEN_Sockaddr &arg0);
ZEN_Sockaddr(sockaddr *arg0, int arg1); // line 6
virtual ~ZEN_Sockaddr(); // line 14
// 原声明为纯虚函数（vtable 槽位指向 __cxa_pure_virtual），DWARF 桩丢失了 "= 0"。
virtual void set_sockaddr(sockaddr *arg0, socklen_t arg1) = 0; // line 19
socklen_t get_size() const { return sockaddr_size_; } // line 52
void set_size(int arg0) { sockaddr_size_ = arg0; } // line 57
void set_addr(sockaddr *arg0) { sockaddr_ptr_ = arg0; } // line 63
sockaddr * get_addr() const { return sockaddr_ptr_; } // line 68
bool operator==(const ZEN_Sockaddr &others_sockaddr) const; // line 21
bool operator!=(const ZEN_Sockaddr &others_sockaddr) const; // line 32
const char * tostr(char *buffer, size_t buf_len) const; // line 37
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_BASE_H_H_
