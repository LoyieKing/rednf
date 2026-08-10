// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_addr_in.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_IN_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_IN_H_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include "zen_socket_addr_base.h"

namespace ZEN_OS {
    // 内联 get_host_addr()/get_host_addr_port() 需要；zen_os_adapt_socket.h 暂为未修复的桩。
    const char *socketaddr_ntop(const sockaddr *sockaddr_ptr, char *buffer, size_t buf_len);
    const char *socketaddr_ntop_ex(const sockaddr *sockaddr_ptr, char *buffer, size_t buf_len);
} // namespace ZEN_OS

// sizeof = 28
struct ZEN_Sockaddr_In : public ZEN_Sockaddr { // line 11
protected:
sockaddr_in in4_addr_;
public:
ZEN_Sockaddr_In(); // line 10
ZEN_Sockaddr_In(const sockaddr_in *arg0); // line 17
ZEN_Sockaddr_In(const char *arg0, uint16_t arg1); // line 25
ZEN_Sockaddr_In(uint32_t arg0, uint16_t arg1); // line 38
ZEN_Sockaddr_In(const ZEN_Sockaddr_In &arg0); // line 50
virtual ~ZEN_Sockaddr_In(); // line 56
virtual void set_sockaddr(sockaddr *addr, socklen_t len); // line 76
int set(const char *ip_addr_str, uint16_t port_number); // line 84
int set(uint32_t ip_addr, uint16_t port_number); // line 98
void set_port_number(uint16_t arg0) { in4_addr_.sin_port = htons(arg0); } // line 109
uint16_t get_port_number() const { return ntohs(in4_addr_.sin_port); } // line 114
const char * get_host_addr(char *arg0, int arg1) const {
    return ZEN_OS::socketaddr_ntop((const sockaddr *)&in4_addr_, arg0, arg1);
} // line 121
const char * get_host_addr() const { // line 129
    // 原实现：65 字节静态缓冲区，预置 [64]=0 后调用 socketaddr_ntop(…, 64)
    static char in4_buf[65];
    in4_buf[64] = 0;
    return ZEN_OS::socketaddr_ntop((const sockaddr *)&in4_addr_, in4_buf, 64);
}
const char * get_host_addr_port(char *arg0, int arg1) const {
    return ZEN_OS::socketaddr_ntop_ex((const sockaddr *)&in4_addr_, arg0, arg1);
} // line 141
uint32_t get_ip_address() const { return ntohl(in4_addr_.sin_addr.s_addr); } // line 102
bool is_internal(); // line 63
bool is_internet(); // line 69
bool operator==(const ZEN_Sockaddr_In &others) const; // line 114
bool operator!=(const ZEN_Sockaddr_In &others) const; // line 130
bool is_ip_equal(const ZEN_Sockaddr_In &others) const; // line 136
int get_name_info(char *host_name, size_t name_len) const; // line 150
int get_addr_info(const char *hostname, uint16_t service_port); // line 162
operator sockaddr_in() const; // line 172
operator const sockaddr_in*() const; // line 178
operator sockaddr_in*(); // line 184
};
extern const size_t BUF_LEN; // line 131

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_IN_H_H_
