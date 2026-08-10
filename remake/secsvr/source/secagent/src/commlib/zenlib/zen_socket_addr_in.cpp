// 还原自 gunnersvr 二进制（DWARF 行号与 zen_socket_addr_in.cpp 对应）
// ZEN_Sockaddr_In：IPv4 sockaddr_in 封装。

#include "zen_socket_addr_in.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>

namespace ZEN_OS {
    // 声明见 zen_os_adapt_socket.h（该头文件尚为未修复的 DWARF 桩，故本地声明）
    int getaddrinfo_inary(const char *hostname, uint16_t service_port,
                          unsigned int *only_one_addr, sockaddr_in *addr);
    int getnameinfo(const sockaddr *addr, size_t addr_len,
                    char *host, size_t host_len, char *serv, size_t serv_len, int flags);
    bool is_internal(const sockaddr_in *addr);
}

// line 131：原文件中定义的缓冲区长度常量（二进制中未被引用，值不可观测）
const size_t BUF_LEN = 128;

// line 10
ZEN_Sockaddr_In::ZEN_Sockaddr_In()
    : ZEN_Sockaddr((sockaddr *)&in4_addr_, sizeof(sockaddr_in)) {
}

// line 17
ZEN_Sockaddr_In::ZEN_Sockaddr_In(const sockaddr_in *arg0)
    : ZEN_Sockaddr((sockaddr *)&in4_addr_, sizeof(sockaddr_in)) {
    in4_addr_ = *arg0;
}

// line 25
ZEN_Sockaddr_In::ZEN_Sockaddr_In(const char *ip_addr_str, uint16_t port_number)
    : ZEN_Sockaddr((sockaddr *)&in4_addr_, sizeof(sockaddr_in)) {
    in4_addr_.sin_family = AF_INET;
    in4_addr_.sin_port = htons(port_number);
    in4_addr_.sin_addr.s_addr = inet_addr(ip_addr_str);
}

// line 38
ZEN_Sockaddr_In::ZEN_Sockaddr_In(uint32_t ip_addr, uint16_t port_number)
    : ZEN_Sockaddr((sockaddr *)&in4_addr_, sizeof(sockaddr_in)) {
    in4_addr_.sin_family = AF_INET;
    in4_addr_.sin_port = htons(port_number);
    in4_addr_.sin_addr.s_addr = htonl(ip_addr);
}

// line 50
ZEN_Sockaddr_In::ZEN_Sockaddr_In(const ZEN_Sockaddr_In &arg0)
    : ZEN_Sockaddr((sockaddr *)&in4_addr_, sizeof(sockaddr_in)) {
    in4_addr_ = arg0.in4_addr_;
}

// line 56
ZEN_Sockaddr_In::~ZEN_Sockaddr_In() {
}

// line 76
void ZEN_Sockaddr_In::set_sockaddr(sockaddr *addr, socklen_t len) {
    memcpy(&in4_addr_, addr, sizeof(sockaddr_in));
    sockaddr_size_ = len;
}

// line 84
int ZEN_Sockaddr_In::set(const char *ip_addr_str, uint16_t port_number) {
    in4_addr_.sin_family = AF_INET;
    in4_addr_.sin_port = htons(port_number);
    in4_addr_.sin_addr.s_addr = inet_addr(ip_addr_str);
    if (in4_addr_.sin_addr.s_addr == INADDR_NONE) {
        return -1;
    }
    return 0;
}

// line 98
int ZEN_Sockaddr_In::set(uint32_t ip_addr, uint16_t port_number) {
    in4_addr_.sin_family = AF_INET;
    in4_addr_.sin_port = htons(port_number);
    in4_addr_.sin_addr.s_addr = htonl(ip_addr);
    return 0;
}

// line 102
// （get_ip_address / set_port_number / get_port_number / get_host_addr /
//   get_host_addr_port 为头文件内联实现，见 zen_socket_addr_in.h）

// line 114
bool ZEN_Sockaddr_In::operator==(const ZEN_Sockaddr_In &others) const {
    return in4_addr_.sin_family == others.in4_addr_.sin_family
        && in4_addr_.sin_addr.s_addr == others.in4_addr_.sin_addr.s_addr
        && in4_addr_.sin_port == others.in4_addr_.sin_port;
}

// line 130
bool ZEN_Sockaddr_In::operator!=(const ZEN_Sockaddr_In &others) const {
    return !(*this == others);
}

// line 136
bool ZEN_Sockaddr_In::is_ip_equal(const ZEN_Sockaddr_In &others) const {
    return in4_addr_.sin_family == others.in4_addr_.sin_family
        && in4_addr_.sin_addr.s_addr == others.in4_addr_.sin_addr.s_addr;
}

// line 150
int ZEN_Sockaddr_In::get_name_info(char *host_name, size_t name_len) const {
    return ZEN_OS::getnameinfo((const sockaddr *)&in4_addr_, sizeof(sockaddr_in),
                               host_name, name_len, 0, 0, NI_NAMEREQD);
}

// line 162
int ZEN_Sockaddr_In::get_addr_info(const char *hostname, uint16_t service_port) {
    unsigned int only_one_addr = 1;
    return ZEN_OS::getaddrinfo_inary(hostname, service_port, &only_one_addr, &in4_addr_);
}

// line 172
ZEN_Sockaddr_In::operator sockaddr_in() const {
    return in4_addr_;
}

// line 178
ZEN_Sockaddr_In::operator const sockaddr_in*() const {
    return &in4_addr_;
}

// line 184
ZEN_Sockaddr_In::operator sockaddr_in*() {
    return &in4_addr_;
}

// line 63
bool ZEN_Sockaddr_In::is_internal() {
    return ZEN_OS::is_internal(&in4_addr_);
}

// line 69
bool ZEN_Sockaddr_In::is_internet() {
    return !ZEN_OS::is_internal(&in4_addr_);
}
