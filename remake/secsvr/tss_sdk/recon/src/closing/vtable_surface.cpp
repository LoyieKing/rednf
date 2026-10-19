// Vtable/typeinfo emitters for the three abstract classes whose vtables were
// absent from the reconstructed object.
//
// Target: /opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394.
//
//   vtable for tss_dpsdk::IDPHacker   0x00292f60, 0x3c bytes, V
//   typeinfo for tss_dpsdk::IDPHacker 0x00292f48 (already emitted; the copy
//                                     here is COMDAT/weak, identical)
//   vtable for CVMBase                0x00293a40, 0x40 bytes, V
//   vtable for IAntibotInterface      0x00292d20, 0x50 bytes, V
//   typeinfo for IAntibotInterface    0x00292d04, 8 bytes, V
//   typeinfo name for IAntibotInterface 0x002190c9, "17IAntibotInterface", V
//
// The slots were read from ORIG .rel.data.rel.ro, not guessed:
//   IDPHacker  +0x00 0, +0x04 &typeinfo, +0x08 init, +0x0c
//   __cxa_pure_virtual (hack), +0x10 on_login, +0x14 on_logout, +0x18 proc,
//   +0x1c on_ban_mode, +0x20 on_drop_cmd, +0x24 on_verify_response,
//   +0x28 on_verify_msg, +0x2c on_safe_mode, +0x30 on_special_control,
//   +0x34 ~IDPHacker D1, +0x38 ~IDPHacker D0.  size 0x3c.
//   CVMBase    16 words: 0, &typeinfo, then 14 __cxa_pure_virtual.
//   IAntibotInterface 20 words: 0, &typeinfo, then 18 __cxa_pure_virtual.
//
// Key function rule (Itanium C++ ABI 5.2.3): the key function is the first
// non-pure virtual member function declared in the class that is not inline at
// the point of class definition.  These three ORIG classes are header-inline
// with no key function (IDPHacker has only inline defaults plus one pure
// slot; CVMBase/IAntibotInterface are all-pure), which is why ORIG emits their
// vtables as weak COMDAT objects from several CUs.  A modern compiler does not
// emit a vtable here because nothing constructs a complete object.  To emit
// it without assembling slots by hand we give each class a real out-of-line
// constructor; the compiler then generates the vtable from the class
// declaration.  These constructors are hidden (not in the dynamic symbol
// table).  IDPHacker's ctor is never called.  IAntibotInterface's ctor is
// also the base constructor invoked by AntibotInterface, whose own ctor then
// overwrites the vptr with the derived table.  IDPHacker's ctor writes the
// same vptr store that ORIG's derived-class constructors did.
//
// The class bodies below (and the shared iantibotinterface.h included above)
// are the reconstructed declarations of
//   dp_hacker_if.h:27..55       (hacker_base_agent.cpp)
//   VMBase.h:19                 (VMCore/vm.h)
//   iantibotinterface.h:82     (original DWARF line; shared adapter header)
// with the exact virtual declaration order.

#include <stddef.h>
#include <time.h>

// Shared complete IAntibotInterface declaration (same header used by the
// adapter implementation and factory TUs).
#include "../formmog/tss_sdk/adapter/iantibotinterface.h"

// ---------------------------------------------------------------------------
// tss_dpsdk::IDPHacker (4 bytes: vptr only).  Slot order from dp_hacker_if.h.
// ---------------------------------------------------------------------------
namespace tss_sdk_impl {
struct UserInfo;
}

namespace sec_proto {
struct DpBanModeReq;
struct DpDropCmdlistReq;
struct DPCaptchaResultRsp;
}

namespace tss_dpsdk {

class IDPHackerEventNotify;

struct __attribute__((visibility("default"))) IDPHacker {
    // Emitter-only; hidden so it is not part of the exported symbol surface.
    __attribute__((visibility("hidden"))) IDPHacker();

    // dp_hacker_if.h:27.  All ten non-pure defaults are `return 0` and read no
    // argument (ORIG 0xa4770..0xa4806).
    virtual int init(IDPHackerEventNotify *event_notify) { return 0; }

    // dp_hacker_if.h:47, pure; vtable +0x0c is __cxa_pure_virtual.
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len) = 0;

    virtual int on_login(tss_sdk_impl::UserInfo *user) { return 0; }
    virtual int on_logout(tss_sdk_impl::UserInfo *user) { return 0; }
    virtual int proc(time_t now) { return 0; }
    virtual int on_ban_mode(tss_sdk_impl::UserInfo *user,
                            const sec_proto::DpBanModeReq &pkg) { return 0; }
    virtual int on_drop_cmd(tss_sdk_impl::UserInfo *user,
                            const sec_proto::DpDropCmdlistReq &pkg) {
        return 0;
    }
    virtual int on_verify_response(tss_sdk_impl::UserInfo *user,
                                   unsigned char *data,
                                   unsigned int data_len) { return 0; }
    virtual int on_verify_msg(tss_sdk_impl::UserInfo *user,
                              const sec_proto::DPCaptchaResultRsp &pkg) {
        return 0;
    }
    virtual int on_safe_mode(tss_sdk_impl::UserInfo *user,
                             int safe_mode_level) { return 0; }
    virtual int on_special_control(tss_sdk_impl::UserInfo *user,
                                   unsigned char *buf, unsigned int &buf_len,
                                   unsigned char *out,
                                   unsigned int &out_len) { return 0; }

    virtual ~IDPHacker() {}  // dp_hacker_if.h:55
};

__attribute__((visibility("hidden"))) IDPHacker::IDPHacker() {
}

}  // namespace tss_dpsdk

typedef char vtable_size_check_IDPHacker[
    sizeof(tss_dpsdk::IDPHacker) == 4 ? 1 : -1];

// ---------------------------------------------------------------------------
// CVMBase (8 bytes: vptr + the unnamed-enum VMError member).  VMBase.h:19.
// All 14 virtual slots are pure.
// ---------------------------------------------------------------------------
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned char *PBYTE;
typedef unsigned short *PWORD;
typedef unsigned int UINT32;
typedef void *PVOID;
typedef unsigned int SIZE_T;

class __attribute__((visibility("default"))) CVMBase {
public:
    __attribute__((visibility("hidden"))) CVMBase();

    enum {
        NoError            = 0,
        MallocMemoryErr    = 1,
        InvalidInstruction = 2,
        NullPointer        = 3,
        InvalidHeader      = 4,
        RunTimeError       = 5,
        KeyLinkError       = 6,
        KeyMatrixError     = 7,
        KeyTreeError       = 8,
        PackageCheckErr    = 9,
        NoByteCode         = 10,
        BinCodeError       = 11
    } VMError;

    virtual BOOL Init() = 0;
    virtual void Release() = 0;
    virtual BOOL GetDecodeCode(BYTE *pbyCodeBuff, PBYTE &pbyDecodeCode,
                               WORD &wCodeLen) = 0;
    virtual BOOL GetEncodeCode(BYTE *pbyCodeBuff, PBYTE &pbyEncodeCode,
                               WORD &wCodeLen) = 0;
    virtual void GetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen,
                           PWORD pwDataLen) = 0;
    virtual void SetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen,
                           PWORD pwDataLen) = 0;
    virtual void GetDataBuffer(PBYTE &pData, UINT32 &dwLen) = 0;
    virtual BOOL SetKeyTree(PVOID pTree, UINT32 dwSize) = 0;
    virtual BOOL SetKeyMatrix(PVOID pMatrix, UINT32 X, UINT32 Y) = 0;
    virtual BOOL SetKeyLink(PVOID pLink, UINT32 dwSize) = 0;
    virtual BOOL Run() = 0;
    virtual BOOL Load(PVOID pCodeBuffer, SIZE_T CodeBufferSize,
                      PVOID pData, SIZE_T DataSize) = 0;
    virtual int GetLastError() = 0;
    virtual void SetError(int nErrorCode) = 0;
};

__attribute__((visibility("hidden"))) CVMBase::CVMBase() {
}

typedef char vtable_size_check_CVMBase[
    sizeof(CVMBase) == 8 ? 1 : -1];

// ---------------------------------------------------------------------------
// IAntibotInterface (4 bytes: vptr only, no virtual destructor).
// Complete declaration: formmog/tss_sdk/adapter/iantibotinterface.h.
// 18 pure slots in DWARF declaration order, matching ORIG 0x00292d20.
// The hidden out-of-line constructor below is the vtable/typeinfo emitter
// anchor (the all-pure class has no key function); it is not exported.
// ---------------------------------------------------------------------------

__attribute__((visibility("hidden"))) IAntibotInterface::IAntibotInterface() {
}

typedef char vtable_size_check_IAntibotInterface[
    sizeof(IAntibotInterface) == 4 ? 1 : -1];
