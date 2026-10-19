// Semantic reconstruction of tss_dpsdk::DPHackMgr from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// Original translation unit:
//   src/formmog/tss_sdk/plugins/proto_proc/dp_hack_mgr.cpp
// Original headers (DWARF references):
//   src/formmog/tss_sdk/plugins/proto_proc/dp_hack_mgr.h
//   src/formmog/tss_sdk/plugins/proto_proc/dp_hack_event_notify.h
//   src/formmog/tss_sdk/plugins/proto_proc/dp_hacker_if.h
//
// ORIG ranges (virtual addresses, sizes from nm -S):
//   DPHackMgr::hack(UserInfo*, int, unsigned int, unsigned char*&, unsigned int&)
//                                                  0x000aac90..0x000aaf58 (0x2c8)
//   DPHackMgr::init()                              0x000ab370..0x000aba63 (0x6f3)
//   DPHackMgr::DPHackMgr() C1                      0x000aba70..0x000abac4 (0x54)
//   DPHackMgr::~DPHackMgr() D2                     0x000abad0..0x000abb32 (0x62)
//   DPHackMgr::DPHackMgr() C2                      0x000abb40..0x000abb94 (0x54)
//   DPHackMgr::~DPHackMgr() D1                     0x000abba0..0x000abc02 (0x62)
//
// Only these six DPHackMgr symbols exist in the target (nm -C).  The public
// getters/event dispatchers (on_login, on_logout, proc, on_ban_mode,
// on_drop_cmd, on_verify_response, on_verify_msg, on_safe_mode,
// on_special_control, get_filter) are header-inline and have no out-of-line
// symbols; they are reconstructed as inline members below from their inlined
// call sites in DPInterface (0xbfb2d/0xbfca9/0xc0f3c/0xc02d5 and the
// on_recv_msg dispatch at 0xc0f..0xc1f region).
//
// IDPHackerEventNotify::~IDPHackerEventNotify D2/D1 also exist as weak
// symbols (0x000abfc0/0x000ac010) and are inlined into ~DPHackMgr; the class
// body below reproduces those semantics.
//
// The hacker plugin classes are partial externs: only the vptr, the exact
// object sizes and the fields touched by DPHackMgr are declared.  Their
// vtables are deliberately left undefined (declared non-inline virtual hack
// is the key function), so construction references the real
// _ZTVN9tss_dpsdk...E objects from the target instead of fabricating copies.
// Same for the out-of-line DPPkgDelayHacker base constructor (C2), which the
// original init() calls for the 0x54/0x60-byte delay/mail hackers.

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <new>
#include <vector>
#include <algorithm>

// ---------------------------------------------------------------------------
// ZEN trace log boundary: the single shared declaration
// (src/commlib/zenlib/zen_trace_log_msg.h).  The local enum/struct stubs this
// TU used to carry are gone; every call site is unchanged.
// ---------------------------------------------------------------------------
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// ---------------------------------------------------------------------------
// tss_sdk_impl::UserInfo (400 bytes): canonical layout header.  hack() only
// reads base_info_.uin_ (offset 0x00) and base_info_.flags_ (offset 0x20).
// ---------------------------------------------------------------------------
#include "src/formmog/tss_sdk/layout/user_info.h"

// Canonical tag-394 hacker layer: IDPHacker / IDPHackerEventNotify /
// DPHackMgr (layout/dp_hack_mgr.h -> dp_hacker.h + dp_hacker_if.h).  The
// local class models this TU used to carry are gone.
#include "src/formmog/tss_sdk/layout/dp_hack_mgr.h"

namespace sec_proto {
struct DpBanModeReq;
struct DpDropCmdlistReq;
struct DPCaptchaResultRsp;
}  // namespace sec_proto

// ---------------------------------------------------------------------------
// tss_dp_comm config mapping (dp_config.h / formmog_conf_tsssdk_dp.h).
//
// DWARF layout of DPConfig (size 0x593a):
//   +0x3d7e DPCmdHack cmd_hack (size 0x1bbc)
//     +0x0000 DPDelayCmd delay_cmd                  35 bytes
//     +0x0023 safemode_to_client
//     +0x0024 safemode_hint_client
//     +0x0025 cf_room_qtalk_enable
//     +0x0026 DPHackFilter up_cmd_hack              (0x802)
//     +0x0828 DPHackFilter down_cmd_hack            (0x802)
//     +0x102a DPHackFilter raw_cmd_hack             (0x802)
// so the three filters are at absolute DPConfig offsets 0x3da4 / 0x45a6 /
// 0x4da8.  Those are the constants the ORIG hack() adds:
//   type == 1 -> 0x3da4 (up), type == 2 -> 0x45a6 (down), else 0x4da8 (raw).
//
// DPHackFilter embeds the fragment array at offset 2 (DWARF), so it is a
// packed aggregate: sizeof 2050, no 4-byte alignment padding before
// hack_fragments.  DPHackFragment is 16 bytes: cmd_start +0, cmd_end +4,
// hack_list[8] +8.
// ---------------------------------------------------------------------------
namespace tss_dp_comm {

struct DPHackFragment {
    uint32_t cmd_start;    // +0
    uint32_t cmd_end;      // +4
    uint8_t hack_list[8];  // +8
};

struct DPHackFilter {
    uint16_t fragment_count;             // +0
    DPHackFragment hack_fragments[128];  // +2
} __attribute__((packed));

struct DPCmdHack {
    uint8_t pad_to_filters_[0x26];
    DPHackFilter up_cmd_hack;    // +0x0026
    DPHackFilter down_cmd_hack;  // +0x0828
    DPHackFilter raw_cmd_hack;   // +0x102a
};

struct DPConfig {
    uint8_t pad_to_cmd_hack_[0x3d7e];
    DPCmdHack cmd_hack;  // +0x3d7e
};

struct DP_CONFIG {
    static DPConfig *glb_dp_config_;  // GOT 0x2943ac
};

}  // namespace tss_dp_comm

// ---------------------------------------------------------------------------
// tss_dpsdk hacker interfaces (canonical layout headers).
//
// IDPHacker: 4 bytes (vptr only), vtable _ZTVN9tss_dpsdk9IDPHackerE
//   +0x08 init(IDPHackerEventNotify*)  +0x0c hack(...) [pure]
//   +0x10 on_login  +0x14 on_logout  +0x18 proc  +0x1c on_ban_mode
//   +0x20 on_drop_cmd  +0x24 on_verify_response  +0x28 on_verify_msg
//   +0x2c on_safe_mode  +0x30 on_special_control  +0x34/~D1 +0x38/~D0
// IDPHackerEventNotify: 112 bytes (vptr +0, nine std::vector<IDPHacker*>
//   at +0x04); register_event is header-inline (de-duplicating push_back).
// DPHackMgr: 124 bytes (hackers_ +0x00, event_notify_ +0x0c); its nine event
//   dispatchers are header-inline with the ORIG body, and this TU defines the
//   out-of-line ctor/dtor/init/hack/get_filter symbols.
// ---------------------------------------------------------------------------
namespace tss_dpsdk {

// ---------------------------------------------------------------------------
// Partial plugin classes.  Sizes are exact (DWARF); the constructors are
// inline as in ORIG except DPPkgDelayHacker's, which ORIG calls out-of-line
// (C2 at 0x000b5da0, initialized members not reconstructed here).  Declaring
// an undefined hack override makes each vtable an external reference to the
// target's _ZTV symbol.
//
// The init() push order and GOT vtable entries (ebx = .got.plt 0x294534):
//   0x294294 DPPkgForwardLaHacker       (4)
//   0x294500 DPFilterForwardLaHacker    (4)
//   0x294354 DPUicTransHacker           (4)
//   0x29438c DPPropertyTransHacker      (4)
//   0x2943e8 -> DPPropertyDelayHacker   (0x54, also sets 0x294360 DPDelayHacker
//                                            vptr through the inline base ctor)
//   0x2944b4 DPMailVerifyHacker         (0x60)
//   0x294384 DPForbidModeHacker         (4)
//   0x2943a0 DPSafeModeHacker           (4)
//   0x2941b8 DPPkgDropHacker            (4)
//   0x294360 DPDelayHacker              (0x54)
//   0x294218 DPPkgDumpHacker            (4)
//   0x294144 DPCfRoomQtalkHacker        (4)
//   0x2944d4 DPCfRecordUserInfo         (4)
//   0x2943b4 DPCfExtractUserInfo        (4)
//   0x2941b4 DPCfForwardRoomMasterMsg   (4)
//   then 8 x DPCmdDropHacker(int)       (8 each, cur_hacker_pos_ = 0..7)
// ---------------------------------------------------------------------------

#define TSS_DP_HACKER_PARTIAL(cls, base)                                 \
    class cls : public base {                                            \
    public:                                                              \
        cls() {}                                                         \
        virtual int hack(tss_sdk_impl::UserInfo *user, int type,         \
                         unsigned int cmd_id, unsigned char *&buf,       \
                         unsigned int &len);                             \
    }

TSS_DP_HACKER_PARTIAL(DPPkgForwardLaHacker, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPFilterForwardLaHacker, DPPkgForwardLaHacker);
TSS_DP_HACKER_PARTIAL(DPUicTransHacker, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPPropertyTransHacker, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPForbidModeHacker, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPSafeModeHacker, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPPkgDropHacker, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPPkgDumpHacker, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPCfRoomQtalkHacker, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPCfRecordUserInfo, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPCfExtractUserInfo, IDPHacker);
TSS_DP_HACKER_PARTIAL(DPCfForwardRoomMasterMsg, DPPkgForwardLaHacker);

#undef TSS_DP_HACKER_PARTIAL

// 0x54 bytes (DWARF); the real class holds map/list/timeval/int members that
// are initialized by the out-of-line C2 constructor at 0x000b5da0.
class DPPkgDelayHacker : public IDPHacker {
public:
    DPPkgDelayHacker();
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);

private:
    uint8_t opaque_[0x50];
};

// 0x54 bytes, no additional data members.
class DPDelayHacker : public DPPkgDelayHacker {
public:
    DPDelayHacker() {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// 0x54 bytes, no additional data members.
class DPPropertyDelayHacker : public DPDelayHacker {
public:
    DPPropertyDelayHacker() {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// 0x60 bytes: DPPkgDelayHacker (+0x54) followed by captcha_no_response_
// (+0x54), captcha_counts_ (+0x58), last_captcha_retry_ (+0x5c), all zeroed
// by the inline constructor.
class DPMailVerifyHacker : public DPPkgDelayHacker {
public:
    DPMailVerifyHacker()
        : captcha_no_response_(0), captcha_counts_(0),
          last_captcha_retry_(0) {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);

private:
    unsigned int captcha_no_response_;
    unsigned int captcha_counts_;
    time_t last_captcha_retry_;
};

// 8 bytes: vptr + cur_hacker_pos_ (DWARF).  Inline constructor, ORIG
// 0x000ab776..0x000ab7ac writes only the vptr and the position.
class DPCmdDropHacker : public IDPHacker {
public:
    explicit DPCmdDropHacker(int pos) : cur_hacker_pos_(pos) {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);

private:
    int cur_hacker_pos_;
};

// ---------------------------------------------------------------------------
// DPHackMgr's flat hacker table (class from layout/dp_hack_mgr.h):
//   +0x00 DPHACKERS hackers_                 (std::vector<IDPHacker*>)
//   +0x0c IDPHackerEventNotify event_notify_ (vptr + 9 vectors, 112 bytes)
//
// hackers_ is the flat, index-addressable table used by hack():
//   0       NULL sentinel (config hack_list values are 1-based)
//   1..15   the plugin hackers created by init() (order below)
//   16..199 NULL slots (resize)
//   200..207 the eight DPCmdDropHacker instances
//   208..255 NULL slots (resize)
// hack_list[8] bytes in the config are indexes into this 256-slot table.
// event_notify_.hackers_[E_*] are the per-event dispatch lists populated by
// each plugin's init() through IDPHackerEventNotify::register_event.
//
// The nine event dispatchers are header-inline in ORIG (no out-of-line
// symbol) and now live in layout/dp_hack_mgr.h; the previous local inline
// copies and their address-taking emission arrays are gone.
// ---------------------------------------------------------------------------

// tagDPHackFilter &DPHackMgr::get_filter(int) -- out-of-line in ORIG
// (_ZN9tss_dpsdk9DPHackMgr10get_filterEi).  Selects the config filter by
// hack type: 1 = up_cmd_hack, 2 = down_cmd_hack, everything else =
// raw_cmd_hack (DPConfig +0x3da4 / +0x45a6 / +0x4da8).  No NULL check on
// glb_dp_config_ in ORIG.
tagDPHackFilter &DPHackMgr::get_filter(int type) {
    if (type == 1)
        return tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.up_cmd_hack;
    if (type == 2)
        return tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.down_cmd_hack;
    return tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.raw_cmd_hack;
}

// ---------------------------------------------------------------------------
// DPHackMgr::DPHackMgr / ~DPHackMgr (C1/C2 and D2/D1).
// Empty bodies: member construction/destruction is exactly the ORIG body.
// ---------------------------------------------------------------------------
DPHackMgr::DPHackMgr() {}

DPHackMgr::~DPHackMgr() {}

// ---------------------------------------------------------------------------
// int DPHackMgr::init()  @ 0x000ab370
//
// 0xab387  hackers_.clear()
// 0xab38c  hackers_.push_back(NULL)     -- slot 0 is a sentinel; the
//           dp_config.xml hack_list values are 1-based (defaults use 1 and 2,
//           which must map to DPPkgForwardLaHacker / DPFilterForwardLaHacker)
// 0xab3ad..0xab727  push 15 default hacker plugins (order below)
// 0xab727  hackers_.resize(200, NULL)
// 0xab76b..0xab7ac  push 8 x new DPCmdDropHacker(i), i = 0..7
// 0xab7ae  hackers_.resize(256, NULL)
// 0xab7f2..0xab82c  for each non-NULL hackers_[i]:
//                     hackers_[i]->init(&event_notify_)  [vptr+0x00]
// 0xab831  return 0
// ---------------------------------------------------------------------------
int DPHackMgr::init() {
    hackers_.clear();

    // Slot 0 is deliberately NULL: config hack_list indexes are 1-based.
    hackers_.push_back((IDPHacker *)0);

    hackers_.push_back(new DPPkgForwardLaHacker());
    hackers_.push_back(new DPFilterForwardLaHacker());
    hackers_.push_back(new DPUicTransHacker());
    hackers_.push_back(new DPPropertyTransHacker());
    hackers_.push_back(new DPPropertyDelayHacker());
    hackers_.push_back(new DPMailVerifyHacker());
    hackers_.push_back(new DPForbidModeHacker());
    hackers_.push_back(new DPSafeModeHacker());
    hackers_.push_back(new DPPkgDropHacker());
    hackers_.push_back(new DPDelayHacker());
    hackers_.push_back(new DPPkgDumpHacker());
    hackers_.push_back(new DPCfRoomQtalkHacker());
    hackers_.push_back(new DPCfRecordUserInfo());
    hackers_.push_back(new DPCfExtractUserInfo());
    hackers_.push_back(new DPCfForwardRoomMasterMsg());

    hackers_.resize(200, (IDPHacker *)0);

    for (int i = 0; i < 8; ++i)
        hackers_.push_back(new DPCmdDropHacker(i));

    hackers_.resize(256, (IDPHacker *)0);

    for (size_t i = 0; i < hackers_.size(); ++i) {
        if (hackers_[i] != 0)
            hackers_[i]->init(&event_notify_);
    }

    return 0;
}

// ---------------------------------------------------------------------------
// int DPHackMgr::hack(UserInfo *user, int type, unsigned int cmd_id,
//                     unsigned char *&buf, unsigned int &len)
//                                                          @ 0x000aac90
//
// 1. filter = get_filter(type);  fragment_count == 0 -> return 0.
// 2. Trace "[%u][%s], type=%d, cmd_id=%d, len=%u, fragment_count=%d".
//    Priority RS_INFO(3) iff user && (user->base_info_.flags_ & 1), else
//    RS_DEBUG(2); uin is user->base_info_.uin_ or 0.
// 3. int res = 0; walk the fragments in order.
//    - cmd_id <  fragment.cmd_start -> stop (return res).
//    - cmd_id >  fragment.cmd_end   -> next fragment.
//    - otherwise walk all 8 hack_list bytes:
//        hackers_[hack_list[j]] == NULL -> next fragment (whole fragment
//                                         skipped, remaining bytes ignored)
//        else trace begin, call
//             IDPHacker::hack(user, type, cmd_id, buf, len)   [vptr+0x04]
//        trace end; non-zero result is returned immediately.
// 4. Return res (0 when no hacker intercepted the command).
//
// The buf / len references are forwarded unchanged.  A hacker may replace
// the caller's pointer and length in place; DPHackMgr performs no copy,
// allocation or length fixup itself.  The return value is the raw hacker
// return code (DPInterface treats 1 as "packet intercepted").
// ---------------------------------------------------------------------------
int DPHackMgr::hack(tss_sdk_impl::UserInfo *user, int type,
                    unsigned int cmd_id, unsigned char *&buf,
                    unsigned int &len) {
    const tagDPHackFilter &filter = get_filter(type);

    if (filter.fragment_count == 0)
        return 0;

    const ZEN_LOG_PRIORITY log_level =
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO
                                                          : RS_DEBUG;

    ZEN_Trace_LogMsg::debug_output(
        log_level,
        "[%u][%s], type=%d, cmd_id=%d, len=%u, fragment_count=%d",
        user != 0 ? user->base_info_.uin_ : 0U, __func__, type, cmd_id, len,
        filter.fragment_count);

    int res = 0;
    for (int i = 0; i < (int)filter.fragment_count; ++i) {
        const tss_dp_comm::DPHackFragment &fragment = filter.hack_fragments[i];

        if (cmd_id < fragment.cmd_start)
            break;
        if (cmd_id > fragment.cmd_end)
            continue;

        for (unsigned int j = 0; j < 8; ++j) {
            IDPHacker *hacker = hackers_[fragment.hack_list[j]];
            if (hacker == 0)
                break;

            ZEN_Trace_LogMsg::debug_output(
                log_level,
                "[%u][%s], begin hack, i=%u,j=%u, hacker=%u",
                user != 0 ? user->base_info_.uin_ : 0U, __func__,
                (unsigned int)i, j, fragment.hack_list[j]);

            res = hacker->hack(user, type, cmd_id, buf, len);

            ZEN_Trace_LogMsg::debug_output(
                log_level, "[%u][%s], end hack, res=%u",
                user != 0 ? user->base_info_.uin_ : 0U, __func__, res);

            if (res != 0)
                return res;
        }
    }

    return res;
}

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// Layout assertions (DWARF sizes/offsets).
// ---------------------------------------------------------------------------
typedef char check_dp_hack_fragment[
    sizeof(tss_dp_comm::DPHackFragment) == 16 ? 1 : -1];
typedef char check_dp_hack_filter[
    sizeof(tss_dp_comm::DPHackFilter) == 2050 ? 1 : -1];
typedef char check_dp_hack_filter_offsets[
    (__builtin_offsetof(tss_dp_comm::DPHackFilter, hack_fragments) == 2)
        ? 1 : -1];
typedef char check_dp_cmd_hack_offsets[
    (__builtin_offsetof(tss_dp_comm::DPCmdHack, up_cmd_hack) == 0x26 &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, down_cmd_hack) == 0x828 &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, raw_cmd_hack) == 0x102a)
        ? 1 : -1];
typedef char check_dp_config_offsets[
    (__builtin_offsetof(tss_dp_comm::DPConfig, cmd_hack) == 0x3d7e &&
     __builtin_offsetof(tss_dp_comm::DPConfig, cmd_hack.up_cmd_hack) ==
         0x3da4 &&
     __builtin_offsetof(tss_dp_comm::DPConfig, cmd_hack.down_cmd_hack) ==
         0x45a6 &&
     __builtin_offsetof(tss_dp_comm::DPConfig, cmd_hack.raw_cmd_hack) ==
         0x4da8)
        ? 1 : -1];
typedef char check_user_info_offsets[
    (__builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.uin_) == 0x00 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.flags_) == 0x20)
        ? 1 : -1];
typedef char check_event_notify_size[
    sizeof(tss_dpsdk::IDPHackerEventNotify) == 112 ? 1 : -1];
typedef char check_dp_hack_mgr_size[
    sizeof(tss_dpsdk::DPHackMgr) == 124 ? 1 : -1];
// hackers_ is the first member and is a 12-byte vector; event_notify_ (align
// 4) therefore starts at 0x0c, matching the ORIG [this+0x0c] argument passed
// by init() to IDPHacker::init().
typedef char check_dp_hack_mgr_prefix[
    (sizeof(tss_dpsdk::DPHACKERS) == 12 &&
     __alignof__(tss_dpsdk::DPHackMgr) == 4 &&
     __alignof__(tss_dpsdk::IDPHackerEventNotify) == 4)
        ? 1 : -1];
typedef char check_hacker_sizes[
    (sizeof(tss_dpsdk::IDPHacker) == 4 &&
     sizeof(tss_dpsdk::DPCmdDropHacker) == 8 &&
     sizeof(tss_dpsdk::DPPkgDelayHacker) == 0x54 &&
     sizeof(tss_dpsdk::DPDelayHacker) == 0x54 &&
     sizeof(tss_dpsdk::DPPropertyDelayHacker) == 0x54 &&
     sizeof(tss_dpsdk::DPMailVerifyHacker) == 0x60)
        ? 1 : -1];

// ORIG 0x000abc30 (CU dp_hack_mgr.cpp, stl_algobase.h:636) ships the libstdc++
// 4.1 pointer-form fill_n used by vector<IDPHacker*>::assign.  GCC 13's
// <bits/stl_algobase.h> no longer instantiates that 4.1 signature from the
// same call site, so the target's weak symbol is reproduced explicitly.
typedef tss_dpsdk::IDPHacker *IDPHackerPtr;
template IDPHackerPtr *
std::fill_n<IDPHackerPtr *, unsigned int, IDPHackerPtr>(
    IDPHackerPtr *first, unsigned int n, IDPHackerPtr const &value);

// ---------------------------------------------------------------------------
// ORIG CU dp_hack_mgr.cpp static-initialization block.
//
// Decoded ORIG order (audit/report/init_brief.md, 26 calls, target size 996):
//   ios_base::Init::Init(__ioinit) + atexit
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (0x238d..0x239c) + atexit
//   __cxa_guard_acquire / TssSdkCommSvc::TssSdkCommSvc() /
//     __cxa_guard_release + atexit; guard_abort + _Unwind_Resume pad
// The CU has no other module-specific object (DPHackMgr itself is heap
// allocated by DPInterface's constructor, not a static).
// ---------------------------------------------------------------------------
#include "src/static_init_block.h"

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ORIG .bss 0x0029d800 (weak): Singleton<TssSdkCommSvc>::object_creator.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
