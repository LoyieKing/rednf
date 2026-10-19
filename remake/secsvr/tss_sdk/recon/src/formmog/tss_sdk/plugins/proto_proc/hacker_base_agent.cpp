// Semantic reconstruction from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x,
// -O2 -fPIC -fno-implement-inlines).  Not a byte-identical source drop.
//
// Original header bodies (the ORIG emits these weak copies from every CU that
// instantiates the vtables):
//
//   dp_hacker_if.h:27    IDPHacker::init(IDPHackerEventNotify*)
//                                                0x000a4770 (0x7, W)
//   dp_hacker_if.h:47    IDPHacker::hack(...) pure virtual (vtable slot 1)
//   dp_hacker_if.h:51    IDPHacker::on_login(UserInfo*)        0x000a4780 (0x7, W)
//                        IDPHacker::on_logout(UserInfo*)       0x000a4790 (0x7, W)
//                        IDPHacker::proc(time_t)               0x000a47a0 (0x7, W)
//                        IDPHacker::on_ban_mode(...)           0x000a47b0 (0x7, W)
//                        IDPHacker::on_drop_cmd(...)           0x000a47c0 (0x7, W)
//                        IDPHacker::on_verify_response(...)    0x000a47d0 (0x7, W)
//                        IDPHacker::on_verify_msg(...)         0x000a47e0 (0x7, W)
//                        IDPHacker::on_safe_mode(...)          0x000a47f0 (0x7, W)
//                        IDPHacker::on_special_control(...)    0x000a4800 (0x7, W)
//   dp_hacker_if.h:55    IDPHacker::~IDPHacker() D0          0x000a4810 (0x2e, W)
//                                                D1          0x000a4840 (0x1e, W)
//   dp_hacker_safe_if.h:21 IDPHackerSafe::~IDPHackerSafe() D0 0x000a9930 (0x60, W)
//                                                         D1 0x000a9990 (0x58, W)
//   dp_pkg_delay_hacker.h:26 DPPkgDelayHacker::~DPPkgDelayHacker()
//                                                     D0    0x000b6480 (0xa2, W)
//                                                     D1    0x000b6530 (0xaa, W)
//
// Every IDPHacker default slot is the same body: `xor eax,eax; ret`, i.e. it
// returns int 0 and touches neither arguments nor object state.  init() does
// not retain the event-notify pointer.
//
// Vtable slot order (_ZTVN9tss_dpsdk9IDPHackerE at 0x00292f60, 0x3c bytes;
// IDPHackerSafe at 0x00293180, 0x3c; DPPkgDelayHacker at 0x00293300, 0x40):
//
//   +0x00 offset-to-top (0)      +0x04 typeinfo
//   +0x08 init                   +0x0c hack (__cxa_pure_virtual)
//   +0x10 on_login              +0x14 on_logout
//   +0x18 proc                  +0x1c on_ban_mode
//   +0x20 on_drop_cmd           +0x24 on_verify_response
//   +0x28 on_verify_msg         +0x2c on_safe_mode
//   +0x30 on_special_control    +0x34 D1
//   +0x38 D0                    (DPPkgDelayHacker only:)
//   +0x3c on_timer (__cxa_pure_virtual)
//
// Class layouts from DWARF (byte_size on the definition DIEs):
//
//   IDPHacker                     4   vptr only
//   IDPHackerSafe                32   vptr +0, ZEN_Thread_Mutex locker_ +4 (28)
//   DPPkgDelayHacker             84   +0x20 std::map<unsigned, DPCommandBuf2>
//                                    +0x38 std::list<DPCommandBuf>
//                                    +0x40 timeval next_timer_
//                                    +0x48 int flag_position_
//                                    +0x4c unsigned node_wait_for_send_
//                                    +0x50 unsigned node_in_delay_
//
// Destructor call targets actually referenced by the ORIG instruction
// sequences (all remain external/standard-library boundaries here):
//
//   IDPHacker D0:        _ZTVN9tss_dpsdk9IDPHackerE+8; operator delete(void*)
//   IDPHackerSafe D0/D1: _ZTVN9tss_dpsdk13IDPHackerSafeE+8;
//                        _ZN16ZEN_Thread_MutexD1Ev(this+4);
//                        _ZTVN9tss_dpsdk9IDPHackerE+8
//   DPPkgDelayHacker D0/D1:
//                        _ZTVN9tss_dpsdk16DPPkgDelayHackerE+8;
//                        _ZNSt10_List_baseIN9tss_dpsdk16DPPkgDelayHacker12DPCommandBufESaIS2_EE8_M_clearEv
//                            (this+0x38);
//                        _ZNSt8_Rb_treeIjSt4pairIKjN9tss_dpsdk16DPPkgDelayHacker13DPCommandBuf2EESt10_Select1stIS5_ESt4lessIjESaIS5_EE8_M_eraseEPSt13_Rb_tree_nodeIS5_E
//                            (this+0x20, [this+0x28]);
//                        _ZTVN9tss_dpsdk13IDPHackerSafeE+8;
//                        _ZN16ZEN_Thread_MutexD1Ev(this+4);
//                        _ZTVN9tss_dpsdk9IDPHackerE+8;
//                        D0 additionally: operator delete(void*)(this)
//
// The base and safe destructors are inline in the original headers, so GCC
// inlines them into the derived destructor; an IDPHackerSafe::~ call therefore
// never appears in the DPPkgDelayHacker code.  The definitions below keep the
// same inline/vague-linkage shape.

#include <new>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>
#include <map>
#include <list>

#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_if.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_safe.h"

// ---------------------------------------------------------------------------
// The ORIG headers declared these bodies inline in the class, so every CU that
// instantiated the vtables emitted weak copies.  This TU is the canonical
// holder of those copies (-fkeep-inline-functions in build_server.py); the
// bodies are the ORIG `xor eax,eax; ret` slots and the empty teardown.
// ---------------------------------------------------------------------------
namespace tss_dpsdk {

inline int IDPHacker::init(IDPHackerEventNotify *) { return 0; }
inline int IDPHacker::on_login(tss_sdk_impl::UserInfo *) { return 0; }
inline int IDPHacker::on_logout(tss_sdk_impl::UserInfo *) { return 0; }
inline int IDPHacker::proc(time_t) { return 0; }
inline int IDPHacker::on_ban_mode(tss_sdk_impl::UserInfo *,
                                  const sec_proto::DpBanModeReq &) {
    return 0;
}
inline int IDPHacker::on_drop_cmd(tss_sdk_impl::UserInfo *,
                                  const sec_proto::DpDropCmdlistReq &) {
    return 0;
}
inline int IDPHacker::on_verify_response(tss_sdk_impl::UserInfo *,
                                         unsigned char *, unsigned int) {
    return 0;
}
inline int IDPHacker::on_verify_msg(tss_sdk_impl::UserInfo *,
                                    const sec_proto::DPCaptchaResultRsp &) {
    return 0;
}
inline int IDPHacker::on_safe_mode(tss_sdk_impl::UserInfo *, int) { return 0; }
inline int IDPHacker::on_special_control(tss_sdk_impl::UserInfo *,
                                         unsigned char *, unsigned int &,
                                         unsigned char *, unsigned int &) {
    return 0;
}

inline IDPHacker::~IDPHacker() {}
inline IDPHackerSafe::~IDPHackerSafe() {}

// ORIG inlines this construction into DPPkgDelayHacker::DPPkgDelayHacker
// (the base has no constructor symbol); the default argument passes NULL.
inline IDPHackerSafe::IDPHackerSafe() : locker_((const char *)0) {}

// ORIG weak D0/D1 (_ZN9tss_dpsdk16DPPkgDelayHackerD1Ev 0x000b6530,
// D0Ev 0x000b6480); the empty body is the compiler-generated list/map/base
// teardown.
inline DPPkgDelayHacker::~DPPkgDelayHacker() {}

// ORIG inline init: registers this object for logout/proc/special-control.
inline int DPPkgDelayHacker::init(IDPHackerEventNotify *event_notify) {
    event_notify->register_event(E_LOGOUT, this);
    event_notify->register_event(E_PROC, this);
    event_notify->register_event(E_SPECIAL_CONTROL, this);
    return 0;
}

}  // namespace tss_dpsdk

// The ORIG emits this as a weak out-of-line copy (0x000a99f0).  Take the
// address so this TU emits it; the vtable references it from other TUs.
namespace {
typedef int (tss_dpsdk::DPPkgDelayHacker::*TssDelayInit)(
    tss_dpsdk::IDPHackerEventNotify *);
__attribute__((used)) TssDelayInit tss_delay_init[] = {
    &tss_dpsdk::DPPkgDelayHacker::init};
}  // namespace

// Layout gates: these must equal the DWARF byte sizes, otherwise the member
// order or the external-type model is wrong.
typedef char hacker_base_size_check_IDPHacker[
    sizeof(tss_dpsdk::IDPHacker) == 4 ? 1 : -1];
typedef char hacker_base_size_check_IDPHackerSafe[
    sizeof(tss_dpsdk::IDPHackerSafe) == 32 ? 1 : -1];
typedef char hacker_base_size_check_DPPkgDelayHacker[
    sizeof(tss_dpsdk::DPPkgDelayHacker) == 84 ? 1 : -1];
typedef char hacker_base_size_check_DPCommandBuf[
    sizeof(tss_dpsdk::DPPkgDelayHacker::DPCommandBuf) == 24 ? 1 : -1];
typedef char hacker_base_size_check_DPCommandBuf2[
    sizeof(tss_dpsdk::DPPkgDelayHacker::DPCommandBuf2) == 12 ? 1 : -1];
typedef char hacker_base_size_check_ZEN_Thread_Mutex[
    sizeof(ZEN_Thread_Mutex) == 28 ? 1 : -1];
