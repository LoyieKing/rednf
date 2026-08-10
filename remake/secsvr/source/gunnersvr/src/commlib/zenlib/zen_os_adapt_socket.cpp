// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_socket.cpp
// Original quirks preserved (verified against the oracle at 0x08090950-0x08091e90):
//  - getaddrinfo_in6ary/_inary only fill the output array when getaddrinfo FAILS;
//    on success they return 0 with the array untouched.
//  - gethostbyaddr_in6/_in pass the whole sockaddr (not the address field).
//  - recvn_timeout returns 0 (not the partial count) on select timeout.
//  - sendn_timeout2 uses SO_RCVTIMEO (not SO_SNDTIMEO).
//  - sendto_timeout2/sendto_timeout ignore the ZEN_Time_Value argument.
//  - handle_ready sets errno=ETIMEDOUT when select returns 0.

#include "src/commlib/zenlib/zen_os_adapt_socket.h"
#include "src/commlib/zenlib/zen_time_value.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

namespace ZEN_OS {

int socket_init(int version_high, int version_low) {
    (void)version_high;
    (void)version_low;
    return 0;
}

int socket_fini() {
    return 0;
}

bool is_in6_addr_v4mapped(const in6_addr *in6) {
    return IN6_IS_ADDR_V4MAPPED(in6);
}

int mapped_in6_to_in(const in6_addr *src, in_addr *dst) {
    if (ZEN_OS::is_in6_addr_v4mapped(src)) {
        dst->s_addr = src->s6_addr32[3];
        return 0;
    }
    errno = EINVAL;
    return -1;
}

int mapped_sockin6_to_sockin(const sockaddr_in6 *src, sockaddr_in *dst) {
    return ZEN_OS::mapped_in6_to_in(&src->sin6_addr, &dst->sin_addr);
}

int inaddr_map_inaddr6(const in_addr *src, in6_addr *dst) {
    dst->s6_addr32[0] = 0;
    dst->s6_addr32[1] = 0;
    dst->s6_addr32[2] = 0;
    dst->s6_addr32[3] = 0;
    dst->s6_addr[10] = 0xff;
    dst->s6_addr[11] = 0xff;
    dst->s6_addr32[3] = src->s_addr;
    return 0;
}

int sockin_map_sockin6(const sockaddr_in *src, sockaddr_in6 *dst) {
    return ZEN_OS::inaddr_map_inaddr6(&src->sin_addr, &dst->sin6_addr);
}

int getnameinfo(const sockaddr *sa, socklen_t salen, char *host, size_t hostlen,
                char *serv, size_t servlen, int flags) {
    return ::getnameinfo(sa, salen, host, hostlen, serv, servlen, flags);
}

int getnameinfo_in6(const sockaddr_in6 *sock_addr6, char *host_name,
                    size_t name_len) {
    return ZEN_OS::getnameinfo((const sockaddr *)sock_addr6, sizeof(sockaddr_in6),
                               host_name, name_len, NULL, 0, NI_NUMERICHOST);
}

int getnameinfo_in(const sockaddr_in *sock_addr, char *host_name, size_t name_len) {
    return ZEN_OS::getnameinfo((const sockaddr *)sock_addr, sizeof(sockaddr_in),
                               host_name, name_len, NULL, 0, NI_NUMERICHOST);
}

bool is_internal(const sockaddr_in *sock_addr_ipv4) {
    uint32_t ip_addr = ntohl(sock_addr_ipv4->sin_addr.s_addr);
    return (ip_addr & 0xff000000) == 0x0a000000     // 10.0.0.0/8
        || (ip_addr & 0xfff00000) == 0xac100000     // 172.16.0.0/12
        || (ip_addr & 0xffff0000) == 0xc0a80000     // 192.168.0.0/16
        || ip_addr == 0;
}

void freeaddrinfo(addrinfo *result) {
    ::freeaddrinfo(result);
}

int getaddrinfo(const char *hostname, const char *service, const addrinfo *hints,
                addrinfo **result) {
    return ::getaddrinfo(hostname, service, hints, result);
}

int getaddrinfo_in6ary(const char *hostname, uint16_t service_port,
                       size_t *ary_addr6_num, sockaddr_in6 *ary_sock_addr6) {
    addrinfo hints;
    addrinfo *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    int ret = ZEN_OS::getaddrinfo(hostname, NULL, &hints, &result);
    if (ret == 0) {
        // Oracle quirk: success leaves the array untouched and returns 0.
        return ret;
    }
    if (result == NULL) {
        errno = EINVAL;
        return -1;
    }
    size_t i = 0;
    addrinfo *prc_node = result;
    while (i < *ary_addr6_num && prc_node != NULL) {
        memcpy(&ary_sock_addr6[i], prc_node->ai_addr, prc_node->ai_addrlen);
        ary_sock_addr6[i].sin6_port = htons(service_port);
        prc_node = prc_node->ai_next;
        ++i;
    }
    *ary_addr6_num = i;
    ZEN_OS::freeaddrinfo(result);
    return 0;
}

int getaddrinfo_inary(const char *hostname, uint16_t service_port,
                      size_t *ary_addr_num, sockaddr_in *ary_sock_addr) {
    addrinfo hints;
    addrinfo *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    int ret = ZEN_OS::getaddrinfo(hostname, NULL, &hints, &result);
    if (ret == 0) {
        // Oracle quirk: success leaves the array untouched and returns 0.
        return ret;
    }
    if (result == NULL) {
        errno = EINVAL;
        return -1;
    }
    size_t i = 0;
    addrinfo *prc_node = result;
    while (i < *ary_addr_num && prc_node != NULL) {
        memcpy(&ary_sock_addr[i], prc_node->ai_addr, prc_node->ai_addrlen);
        ary_sock_addr[i].sin_port = htons(service_port);
        prc_node = prc_node->ai_next;
        ++i;
    }
    *ary_addr_num = i;
    ZEN_OS::freeaddrinfo(result);
    return 0;
}

hostent *gethostbyaddr(const void *addr, socklen_t len, int family) {
    return ::gethostbyaddr(addr, len, family);
}

int gethostbyaddr_in6(const sockaddr_in6 *sock_addr6, char *host_name,
                      size_t name_len) {
    // Oracle quirk: the whole sockaddr_in6 is passed as the address.
    hostent *hostent_ptr = ZEN_OS::gethostbyaddr(
        sock_addr6, sizeof(sockaddr_in6), sock_addr6->sin6_family);
    if (hostent_ptr == NULL) {
        return -1;
    }
    strncpy(host_name, hostent_ptr->h_name, name_len);
    return 0;
}

int gethostbyaddr_in(const sockaddr_in *sock_addr, char *host_name,
                     size_t name_len) {
    // Oracle quirk: the whole sockaddr_in is passed as the address.
    hostent *hostent_ptr = ZEN_OS::gethostbyaddr(
        sock_addr, sizeof(sockaddr_in), sock_addr->sin_family);
    if (hostent_ptr == NULL) {
        return -1;
    }
    strncpy(host_name, hostent_ptr->h_name, name_len);
    return 0;
}

int gethostbyname_in6ary(const char *hostname, uint16_t service_port,
                         size_t *ary_addr6_num, sockaddr_in6 *ary_sock_addr6) {
    hostent *hostent_ptr = ::gethostbyname(hostname);
    if (hostent_ptr == NULL) {
        return -1;
    }
    if (hostent_ptr->h_addrtype != AF_INET6) {
        errno = EINVAL;
        return -1;
    }
    size_t i = 0;
    char **addr_pptr = hostent_ptr->h_addr_list;
    while (i < *ary_addr6_num && *addr_pptr != NULL) {
        ary_sock_addr6[i].sin6_family = AF_INET6;
        memcpy(&ary_sock_addr6[i].sin6_addr, *addr_pptr, hostent_ptr->h_length);
        ary_sock_addr6[i].sin6_port = htons(service_port);
        ++addr_pptr;
        ++i;
    }
    *ary_addr6_num = i;
    return 0;
}

int gethostbyname_inary(const char *hostname, uint16_t service_port,
                        size_t *ary_addr_num, sockaddr_in *ary_sock_addr) {
    hostent *hostent_ptr = ::gethostbyname(hostname);
    if (hostent_ptr == NULL) {
        return -1;
    }
    if (hostent_ptr->h_addrtype != AF_INET) {
        errno = EINVAL;
        return -1;
    }
    size_t i = 0;
    char **addr_pptr = hostent_ptr->h_addr_list;
    while (i < *ary_addr_num && *addr_pptr != NULL) {
        ary_sock_addr[i].sin_family = AF_INET;
        memcpy(&ary_sock_addr[i].sin_addr, *addr_pptr, hostent_ptr->h_length);
        ary_sock_addr[i].sin_port = htons(service_port);
        ++addr_pptr;
        ++i;
    }
    *ary_addr_num = i;
    return 0;
}

hostent *gethostbyname(const char *hostname) {
    return ::gethostbyname(hostname);
}

hostent *gethostbyname2(const char *hostname, int af) {
    return ::gethostbyname2(hostname, af);
}

const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len) {
    return ::inet_ntop(family, addrptr, strptr, len);
}

const char *socketaddr_ntop_ex(const sockaddr *sock_addr, char *str_ptr,
                               size_t str_len) {
    uint16_t addr_port;
    const char *ret_str;
    size_t add_str_len;
    const size_t PORT_LEN = 7; // ':' + up to 5 digits + NUL
    if (sock_addr->sa_family == AF_INET) {
        const sockaddr_in *sockadd_ipv4 = (const sockaddr_in *)sock_addr;
        addr_port = ntohs(sockadd_ipv4->sin_port);
        ret_str = ZEN_OS::inet_ntop(AF_INET, &sockadd_ipv4->sin_addr,
                                    str_ptr, str_len);
        if (ret_str == NULL) {
            return NULL;
        }
        add_str_len = strlen(str_ptr);
        if (str_len < add_str_len + PORT_LEN) {
            return NULL;
        }
        snprintf(str_ptr + add_str_len, str_len - add_str_len, ":%u", addr_port);
        return str_ptr;
    }
    if (sock_addr->sa_family == AF_INET6) {
        const sockaddr_in6 *sockadd_ipv6 = (const sockaddr_in6 *)sock_addr;
        addr_port = ntohs(sockadd_ipv6->sin6_port);
        ret_str = ZEN_OS::inet_ntop(AF_INET6, &sockadd_ipv6->sin6_addr,
                                    str_ptr, str_len);
        if (ret_str == NULL) {
            return NULL;
        }
        add_str_len = strlen(str_ptr);
        if (str_len < add_str_len + PORT_LEN) {
            return NULL;
        }
        snprintf(str_ptr + add_str_len, str_len - add_str_len, ":%u", addr_port);
        return str_ptr;
    }
    errno = EAFNOSUPPORT;
    return NULL;
}

const char *socketaddr_ntop(const sockaddr *sock_addr, char *str_ptr,
                            size_t str_len) {
    if (sock_addr->sa_family == AF_INET) {
        return ZEN_OS::inet_ntop(
            AF_INET, &((const sockaddr_in *)sock_addr)->sin_addr, str_ptr, str_len);
    }
    if (sock_addr->sa_family == AF_INET6) {
        return ZEN_OS::inet_ntop(
            AF_INET6, &((const sockaddr_in6 *)sock_addr)->sin6_addr,
            str_ptr, str_len);
    }
    errno = EAFNOSUPPORT;
    return NULL;
}

int inet_pton(int family, const char *strptr, void *addrptr) {
    return ::inet_pton(family, strptr, addrptr);
}

ssize_t sendto_timeout2(ZEN_SOCKET handle, const void *buf, size_t len,
                        const sockaddr *addr, int addrlen,
                        ZEN_Time_Value &arg5, int flags) {
    (void)arg5;
    return ::sendto(handle, buf, len, flags, addr, addrlen);
}

ssize_t recvn(ZEN_SOCKET handle, void *buf, size_t len, int flags) {
    ssize_t bytes_recv = 0;
    if (len == 0) {
        return 0;
    }
    while (bytes_recv < len) {
        ssize_t onetime_recv = ::recv(
            handle, (char *)buf + bytes_recv, len - bytes_recv, flags);
        if (onetime_recv > 0) {
            bytes_recv += onetime_recv;
        } else {
            break;
        }
    }
    return bytes_recv;
}

int handle_ready(ZEN_SOCKET handle, ZEN_Time_Value *timeout_ztv,
                 ZEN_OS::HANDLE_READY_TODO ready_todo) {
    fd_set handle_set;
    FD_ZERO(&handle_set);
    FD_SET(handle, &handle_set);

    fd_set *p_set_read = NULL;
    fd_set *p_set_write = NULL;
    fd_set *p_set_exception = NULL;
    if (ready_todo == HANDLE_READY_READ) {
        p_set_read = &handle_set;
    } else if (ready_todo == HANDLE_READY_WRITE) {
        p_set_write = &handle_set;
    } else if (ready_todo == HANDLE_READY_EXCEPTION) {
        p_set_exception = &handle_set;
    }

    timeval *timeout_tv = NULL;
    if (timeout_ztv != NULL) {
        timeout_tv = *timeout_ztv;
    }

    int select_width = handle + 1;
    int result = ::select(select_width, p_set_read, p_set_write, p_set_exception,
                          timeout_tv);
    if (result == 0) {
        errno = ETIMEDOUT;
    }
    return result;
}

ssize_t recvfrom_timeout(ZEN_SOCKET handle, void *buf, size_t len, sockaddr *from,
                         socklen_t *from_len, ZEN_Time_Value &timeout_tv,
                         int flags) {
    int ret = ZEN_OS::handle_ready(handle, &timeout_tv, HANDLE_READY_READ);
    if (ret != 1) {
        if (ret == 0) {
            errno = ETIMEDOUT;
        }
        return -1;
    }
    return ::recvfrom(handle, buf, len, flags | MSG_DONTWAIT, from, from_len);
}

ssize_t sendn_timeout(ZEN_SOCKET handle, const void *buf, size_t len,
                      ZEN_Time_Value &timeout_ztv, int flags) {
    const int new_flags = flags | MSG_DONTWAIT;
    ssize_t bytes_send = 0;
    if (len == 0) {
        return 0;
    }
    while (bytes_send < len) {
        ssize_t onetime_send = ::send(
            handle, (const char *)buf + bytes_send, len - bytes_send, new_flags);
        if (onetime_send > 0) {
            bytes_send += onetime_send;
        } else if (onetime_send == 0) {
            return onetime_send;
        } else {
            if (errno != EAGAIN) {
                return onetime_send;
            }
            int ret = ZEN_OS::handle_ready(handle, &timeout_ztv, HANDLE_READY_WRITE);
            if (ret != 1) {
                if (ret == 0) {
                    errno = ETIMEDOUT;
                }
                break;
            }
        }
    }
    return bytes_send;
}

int sock_disable(ZEN_SOCKET handle, int flags) {
    int val = fcntl(handle, F_GETFL, 0);
    if (val == -1) {
        return -1;
    }
    val &= ~flags;
    return fcntl(handle, F_SETFL, val) == -1 ? -1 : 0;
}

int sock_enable(ZEN_SOCKET handle, int flags) {
    int val = fcntl(handle, F_GETFL, 0);
    if (val == -1) {
        return -1;
    }
    val |= flags;
    return fcntl(handle, F_SETFL, val) == -1 ? -1 : 0;
}

ssize_t sendmsg(ZEN_SOCKET handle, const msghdr *msg, int flags) {
    return ::sendmsg(handle, msg, flags);
}

ssize_t recvmsg(ZEN_SOCKET handle, msghdr *msg, int flags) {
    return ::recvmsg(handle, msg, flags);
}

ssize_t readv(ZEN_SOCKET handle, iovec *buffers, int iovcnt) {
    return ::readv(handle, buffers, iovcnt);
}

ssize_t writev(ZEN_SOCKET handle, const iovec *buffers, int iovcnt) {
    return ::writev(handle, buffers, iovcnt);
}

ssize_t sendto_timeout(ZEN_SOCKET handle, const void *buf, size_t len,
                       const sockaddr *addr, int addrlen,
                       ZEN_Time_Value &arg5, int flags) {
    (void)arg5;
    return ::sendto(handle, buf, len, flags, addr, addrlen);
}

ssize_t recvfrom_timeout2(ZEN_SOCKET handle, void *buf, size_t len, sockaddr *addr,
                          socklen_t *addrlen, ZEN_Time_Value &timeout_ztv,
                          int flags) {
    int ret = ::setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, &timeout_ztv,
                           sizeof(timeval));
    (void)ret;
    return ::recvfrom(handle, buf, len, flags, addr, addrlen);
}

ssize_t recvn_timeout2(ZEN_SOCKET handle, void *buf, size_t len,
                       ZEN_Time_Value &timeout_ztv, int flags) {
    timeval timeout_tv = (timeval)timeout_ztv;
    int ret = ::setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, &timeout_tv,
                           sizeof(timeval));
    if (ret != 0) {
        return -1;
    }
    ssize_t bytes_recv = 0;
    if (len == 0) {
        return 0;
    }
    while (bytes_recv < len) {
        ssize_t onetime_recv = ::recv(
            handle, (char *)buf + bytes_recv, len - bytes_recv, flags);
        if (onetime_recv > 0) {
            bytes_recv += onetime_recv;
        } else {
            return onetime_recv;
        }
    }
    return bytes_recv;
}

ssize_t sendn(ZEN_SOCKET handle, const void *buf, size_t len, int flags) {
    ssize_t bytes_send = 0;
    if (len == 0) {
        return 0;
    }
    while (bytes_send < len) {
        ssize_t onetime_send = ::send(
            handle, (const char *)buf + bytes_send, len - bytes_send, flags);
        if (onetime_send > 0) {
            bytes_send += onetime_send;
        } else {
            return onetime_send;
        }
    }
    return bytes_send;
}

ssize_t recvn_timeout(ZEN_SOCKET handle, void *buf, size_t len,
                      ZEN_Time_Value &timeout_ztv, int flags) {
    const int new_flags = flags | MSG_DONTWAIT;
    ssize_t bytes_recv = 0;
    if (len == 0) {
        return 0;
    }
    while (bytes_recv < len) {
        int ret = ZEN_OS::handle_ready(handle, &timeout_ztv, HANDLE_READY_READ);
        if (ret != 1) {
            if (ret == 0) {
                errno = ETIMEDOUT;
                // Oracle quirk: returns 0, not the partial count.
                return 0;
            }
            return bytes_recv;
        }
        ssize_t onetime_recv = ::recv(
            handle, (char *)buf + bytes_recv, len - bytes_recv, new_flags);
        if (onetime_recv > 0) {
            bytes_recv += onetime_recv;
        } else if (onetime_recv == 0) {
            return 0;
        } else if (errno != EAGAIN) {
            return onetime_recv;
        }
    }
    return bytes_recv;
}

ssize_t sendn_timeout2(ZEN_SOCKET handle, void *buf, size_t len,
                       ZEN_Time_Value &timeout_ztv, int flags) {
    timeval timeout_tv = (timeval)timeout_ztv;
    // Oracle quirk: uses SO_RCVTIMEO even for the send path.
    int ret = ::setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, &timeout_tv,
                           sizeof(timeval));
    if (ret != 0) {
        return -1;
    }
    ssize_t bytes_send = 0;
    if (len == 0) {
        return 0;
    }
    while (bytes_send < len) {
        ssize_t onetime_send = ::send(
            handle, (const char *)buf + bytes_send, len - bytes_send, flags);
        if (onetime_send > 0) {
            bytes_send += onetime_send;
        } else {
            return onetime_send;
        }
    }
    return bytes_send;
}

int handle_multi_ready(ZEN_SOCKET handle, ZEN_Time_Value *timeout_ztv,
                       int multi_ready_todo, int *multiready_occur) {
    fd_set handle_set_read;
    fd_set handle_set_write;
    fd_set handle_set_exeception;
    FD_ZERO(&handle_set_read);
    FD_ZERO(&handle_set_write);
    FD_ZERO(&handle_set_exeception);

    fd_set *p_set_read = NULL;
    fd_set *p_set_write = NULL;
    fd_set *p_set_exception = NULL;
    *multiready_occur = 0;
    if (multi_ready_todo & 0x1) {
        FD_SET(handle, &handle_set_read);
        p_set_read = &handle_set_read;
    }
    if (multi_ready_todo & 0x2) {
        FD_SET(handle, &handle_set_write);
        p_set_write = &handle_set_write;
    }
    if (multi_ready_todo & 0x4) {
        FD_SET(handle, &handle_set_exeception);
        p_set_exception = &handle_set_exeception;
    }

    timeval *timeout_tv = NULL;
    if (timeout_ztv != NULL) {
        timeout_tv = *timeout_ztv;
    }

    int select_width = handle + 1;
    int result = ::select(select_width, p_set_read, p_set_write, p_set_exception,
                          timeout_tv);
    if (result == 0) {
        errno = ETIMEDOUT;
        return result;
    }
    if (result < 0) {
        return result;
    }
    if (p_set_read != NULL && FD_ISSET(handle, p_set_read)) {
        *multiready_occur |= 0x1;
    }
    if (p_set_write != NULL && FD_ISSET(handle, p_set_write)) {
        *multiready_occur |= 0x2;
    }
    if (p_set_exception != NULL && FD_ISSET(handle, p_set_exception)) {
        *multiready_occur |= 0x4;
    }
    return result;
}

} // namespace ZEN_OS
