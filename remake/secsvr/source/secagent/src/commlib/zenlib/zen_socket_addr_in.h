// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_socket_addr_in.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_IN_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_IN_H_H_

#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

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
void set_port_number(uint16_t arg0); // line 109
uint16_t get_port_number() const; // line 114
const char * get_host_addr(char *arg0, int arg1) const; // line 121
const char * get_host_addr() const; // line 129
const char * get_host_addr_port(char *arg0, int arg1) const; // line 141
uint32_t get_ip_address() const; // line 102
bool is_internal(); // line 63
bool is_internet(); // line 69
bool operator==(const ZEN_Sockaddr_In &others) const; // line 114
bool operator!=(const ZEN_Sockaddr_In &others) const; // line 130
bool is_ip_equal(const ZEN_Sockaddr_In &others) const; // line 136
int get_name_info(char *host_name, size_t name_len) const; // line 150
int get_addr_info(const char *hostname, uint16_t service_port); // line 162
sockaddr_in operator sockaddr_in() const; // line 172
const sockaddr_in * operator const sockaddr_in*() const; // line 178
sockaddr_in * operator sockaddr_in*(); // line 184
};
extern short unsigned int __v; // line 116
extern short unsigned int __x; // line 116
extern const size_t BUF_LEN; // line 131

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_ADDR_IN_H_H_
