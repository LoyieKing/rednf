// ZEN_Sockaddr_In::get_host_addr() const -- ORIG 0x000d9f90, 0x3c bytes, W.
//
// The original definition is the inline header body in
// src/commlib/zenlib/zen_socket_addr_in.h:129 (CU zen_socket_addr_in.cpp is
// the emitting unit in ORIG):
//
//   const char * get_host_addr() const {
//       static char in4_buf[65];
//       in4_buf[64] = 0;
//       return ZEN_OS::socketaddr_ntop((const sockaddr *)&in4_addr_,
//                                      in4_buf, 64);
//   }
//
// ORIG disassembly 0x000d9f90..0x000d9fcb is exactly that body:
//   *[GOT in4_buf + 0x40] = 0
//   socketaddr_ntop(this + 0xc, in4_buf, 0x40)
//   return the callee's pointer (eax untouched after the call)
// The local static is the 0x41-byte V symbol at 0x002c8b20
// (_ZZNK15ZEN_Sockaddr_In13get_host_addrEvE7in4_buf).
//
// We deliberately do not include the reconstructed secagent header here: its
// body is already inline, and build_server.py compiles closing/*.cpp with
// -fno-implement-inlines, which suppresses out-of-line copies of inline
// functions.  Restating the header declaration plus this one out-of-line
// definition is the same source body and emits the weak symbol directly.
//
// Layout (DWARF, sizeof 28):
//   ZEN_Sockaddr      +0x00 vptr, +0x04 sockaddr_ptr_, +0x08 sockaddr_size_
//   ZEN_Sockaddr_In   +0x0c sockaddr_in in4_addr_
// Only in4_addr_ is read by this function.  The other virtuals are declared
// to keep the vptr/base layout identical; they are not defined here.

#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>

namespace ZEN_OS {

// zen_os_adapt_socket.cpp; ORIG 0x001a46d0, T.
const char *socketaddr_ntop(const sockaddr *sockaddr_ptr, char *buffer,
                            size_t buf_len);

}  // namespace ZEN_OS

// zen_socket_addr_base.h:10, sizeof 12.
struct ZEN_Sockaddr {
    virtual ~ZEN_Sockaddr();                          // line 14
    virtual void set_sockaddr(sockaddr *arg0,
                              socklen_t arg1) = 0;   // line 19

    sockaddr *sockaddr_ptr_;                          // +0x04
    socklen_t sockaddr_size_;                         // +0x08
};

// zen_socket_addr_in.h:11, sizeof 28.
struct ZEN_Sockaddr_In : public ZEN_Sockaddr {
protected:
    sockaddr_in in4_addr_;                            // +0x0c

public:
    const char *get_host_addr() const;                // line 129
};

typedef char zen_sockaddr_in_size_check[
    sizeof(ZEN_Sockaddr_In) == 28 ? 1 : -1];

// ORIG 0x000d9f90.  Explicit weak definition keeps ORIG's W binding; with
// -fno-implement-inlines this non-inline definition is still emitted.
__attribute__((weak))
const char *ZEN_Sockaddr_In::get_host_addr() const {
    static char in4_buf[65];
    in4_buf[64] = 0;
    return ZEN_OS::socketaddr_ntop((const sockaddr *)&in4_addr_, in4_buf, 64);
}
