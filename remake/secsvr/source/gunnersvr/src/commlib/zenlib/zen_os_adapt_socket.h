// Restored header for zen_os_adapt_socket.cpp (gunnersvr oracle).
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SOCKET_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SOCKET_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"

#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>

struct ZEN_Time_Value;

namespace ZEN_OS {
    enum HANDLE_READY_TODO {
        HANDLE_READY_READ = 1,
        HANDLE_READY_ACCEPT = 1,
        HANDLE_READY_WRITE = 2,
        HANDLE_READY_CONNECT_SUCCESS = 2,
        HANDLE_READY_EXCEPTION = 4,
        HANDLE_READY_CONNECT_FAIL = 1,
    };

    extern int socket_init(int version_high, int version_low);
    extern int socket_fini();
    extern bool is_in6_addr_v4mapped(const in6_addr *in6);
    extern int mapped_in6_to_in(const in6_addr *src, in_addr *dst);
    extern int mapped_sockin6_to_sockin(const sockaddr_in6 *src, sockaddr_in *dst);
    extern int inaddr_map_inaddr6(const in_addr *src, in6_addr *dst);
    extern int sockin_map_sockin6(const sockaddr_in *src, sockaddr_in6 *dst);
    extern int getnameinfo(const sockaddr *sa, socklen_t salen, char *host,
                           size_t hostlen, char *serv, size_t servlen, int flags);
    extern int getnameinfo_in6(const sockaddr_in6 *sock_addr6, char *host_name,
                               size_t name_len);
    extern int getnameinfo_in(const sockaddr_in *sock_addr, char *host_name,
                              size_t name_len);
    extern bool is_internal(const sockaddr_in *sock_addr_ipv4);
    extern void freeaddrinfo(addrinfo *result);
    extern int getaddrinfo(const char *hostname, const char *service,
                           const addrinfo *hints, addrinfo **result);
    extern int getaddrinfo_in6ary(const char *hostname, uint16_t service_port,
                                  size_t *ary_addr6_num, sockaddr_in6 *ary_sock_addr6);
    extern int getaddrinfo_inary(const char *hostname, uint16_t service_port,
                                 size_t *ary_addr_num, sockaddr_in *ary_sock_addr);
    extern hostent *gethostbyaddr(const void *addr, socklen_t len, int family);
    extern int gethostbyaddr_in6(const sockaddr_in6 *sock_addr6, char *host_name,
                                 size_t name_len);
    extern int gethostbyaddr_in(const sockaddr_in *sock_addr, char *host_name,
                                size_t name_len);
    extern int gethostbyname_in6ary(const char *hostname, uint16_t service_port,
                                    size_t *ary_addr6_num, sockaddr_in6 *ary_sock_addr6);
    extern int gethostbyname_inary(const char *hostname, uint16_t service_port,
                                   size_t *ary_addr_num, sockaddr_in *ary_sock_addr);
    extern hostent *gethostbyname(const char *hostname);
    extern hostent *gethostbyname2(const char *hostname, int af);
    extern const char *inet_ntop(int family, const void *addrptr, char *strptr,
                                 size_t len);
    extern const char *socketaddr_ntop_ex(const sockaddr *sock_addr, char *str_ptr,
                                          size_t str_len);
    extern const char *socketaddr_ntop(const sockaddr *sock_addr, char *str_ptr,
                                       size_t str_len);
    extern int inet_pton(int family, const char *strptr, void *addrptr);
    extern ssize_t sendto_timeout2(ZEN_SOCKET handle, const void *buf, size_t len,
                                   const sockaddr *addr, int addrlen,
                                   ZEN_Time_Value &arg5, int flags);
    extern ssize_t recvn(ZEN_SOCKET handle, void *buf, size_t len, int flags);
    extern int handle_ready(ZEN_SOCKET handle, ZEN_Time_Value *timeout_ztv,
                            ZEN_OS::HANDLE_READY_TODO ready_todo);
    extern ssize_t recvfrom_timeout(ZEN_SOCKET handle, void *buf, size_t len,
                                    sockaddr *from, socklen_t *from_len,
                                    ZEN_Time_Value &timeout_tv, int flags);
    extern ssize_t sendn_timeout(ZEN_SOCKET handle, const void *buf, size_t len,
                                 ZEN_Time_Value &timeout_ztv, int flags);
    extern int sock_disable(ZEN_SOCKET handle, int flags);
    extern int sock_enable(ZEN_SOCKET handle, int flags);
    extern ssize_t sendmsg(ZEN_SOCKET handle, const msghdr *msg, int flags);
    extern ssize_t recvmsg(ZEN_SOCKET handle, msghdr *msg, int flags);
    extern ssize_t readv(ZEN_SOCKET handle, iovec *buffers, int iovcnt);
    extern ssize_t writev(ZEN_SOCKET handle, const iovec *buffers, int iovcnt);
    extern ssize_t sendto_timeout(ZEN_SOCKET handle, const void *buf, size_t len,
                                  const sockaddr *addr, int addrlen,
                                  ZEN_Time_Value &arg5, int flags);
    extern ssize_t recvfrom_timeout2(ZEN_SOCKET handle, void *buf, size_t len,
                                     sockaddr *addr, socklen_t *addrlen,
                                     ZEN_Time_Value &timeout_ztv, int flags);
    extern ssize_t recvn_timeout2(ZEN_SOCKET handle, void *buf, size_t len,
                                  ZEN_Time_Value &timeout_ztv, int flags);
    extern ssize_t sendn(ZEN_SOCKET handle, const void *buf, size_t len, int flags);
    extern ssize_t recvn_timeout(ZEN_SOCKET handle, void *buf, size_t len,
                                 ZEN_Time_Value &timeout_ztv, int flags);
    extern ssize_t sendn_timeout2(ZEN_SOCKET handle, void *buf, size_t len,
                                  ZEN_Time_Value &timeout_ztv, int flags);
    extern int handle_multi_ready(ZEN_SOCKET handle, ZEN_Time_Value *timeout_ztv,
                                  int multi_ready_todo, int *multiready_occur);
} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_SOCKET_H_H_
