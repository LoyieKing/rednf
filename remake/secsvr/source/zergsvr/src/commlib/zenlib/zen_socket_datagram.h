// Restored from zergsvr binary DWARF + disassembly (2026-08-10).
// Zen_Socket_DataGram: sizeof 4, inherits ZEN_Socket_Base at offset 0
// (socket_handle_). recvfrom/sendto and the ZEN_Sockaddr ctor are declared
// (DWARF decl lines 19/41/47/54/62) but have no out-of-line symbols in the
// binary (never referenced -> GC'd).
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_DATAGRAM_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_DATAGRAM_H_H_

#include <stddef.h>
#include <sys/types.h>

#include "zen_socket_base.h"
#include "zen_time_value.h"

// sizeof = 4
struct Zen_Socket_DataGram : public ZEN_Socket_Base { // line 12
public:
    Zen_Socket_DataGram();                                  // line 17
    Zen_Socket_DataGram(const ZEN_SOCKET &socket_handle);   // line 18
    Zen_Socket_DataGram(const ZEN_Sockaddr &local_addr);    // line 19
    virtual ~Zen_Socket_DataGram();                         // line 20
    int open(int protocol_family, int protocol, bool reuse_addr);                       // line 27
    int open(const ZEN_Sockaddr *local_addr, int protocol_family, int protocol, bool reuse_addr); // line 34
    ssize_t recvfrom(void *buf, size_t len, int flags, ZEN_Sockaddr *from_addr) const {  // line 41（内联，二进制中无 out-of-line 符号）
        return ::recvfrom(socket_handle_, buf, len, flags, from_addr->get_addr(),
                          &from_addr->sockaddr_size_);
    }
    ssize_t sendto(const void *buf, size_t len, int flags, const ZEN_Sockaddr *to_addr) const { // line 47（内联）
        return ::sendto(socket_handle_, buf, len, flags, to_addr->get_addr(),
                        to_addr->get_size());
    }
    ssize_t recvfrom(void *buf, size_t len, ZEN_Sockaddr *from_addr, ZEN_Time_Value *timeout, int flags) const; // line 54
    ssize_t sendto(const void *buf, size_t len, const ZEN_Sockaddr *to_addr, ZEN_Time_Value *timeout, int flags) const; // line 62
    ssize_t recvfrom_timeout(void *buf, size_t len, ZEN_Sockaddr *from_addr, ZEN_Time_Value &timeout_ztv, int flags) const; // line 69
    ssize_t sendto_timeout(const void *buf, size_t len, const ZEN_Sockaddr *to_addr, ZEN_Time_Value &timeout_ztv, int flags) const; // line 77
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SOCKET_DATAGRAM_H_H_
