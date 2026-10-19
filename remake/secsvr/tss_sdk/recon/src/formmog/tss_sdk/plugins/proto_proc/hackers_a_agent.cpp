// Semantic reconstruction from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x,
// -O2 -fPIC -fno-implement-inlines).  Not a byte-identical source drop.
//
// Original translation units / ORIG virtual-address ranges (nm -S):
//
//   dp_cmd_drop_hacker.cpp
//     DPCmdDropHacker::hack(UserInfo*, int, unsigned, unsigned char*&,
//                           unsigned int&)        0x000a66a0..0x000a6878 (0x1d8)
//     DPCmdDropHacker::on_drop_cmd(UserInfo*,
//                           const sec_proto::DpDropCmdlistReq&)
//                                                  0x000a6880..0x000a69f5 (0x175)
//     DPCmdDropHacker::init(IDPHackerEventNotify*)
//                                                  0x000a6e40..0x000a6ec1 (0x81, W)
//     DPCmdDropHacker::init(...)::inited           0x002baea0 (4, V)
//
//   dp_pkg_drop_hacker.cpp
//     DPPkgDropHacker::hack(...)                   0x000b6680..0x000b684d (0x1cd)
//
//   dp_pkg_dump_hacker.cpp
//     DPPkgDumpHacker::hack(...)                   0x000b6d70..0x000b6de3 (0x73)
//
//   dp_filter_foward_hacker.cpp
//     DPFilterForwardLaHacker::hack(...)           0x000a9d30..0x000a9e63 (0x133)
//
//   dp_pkg_foward_hacker.cpp
//     DPPkgForwardLaHacker::hack(...)              0x000b7720..0x000b7e06 (0x6e6)
//
//   dp_delay_hacker.cpp
//     DPDelayHacker::hack(...)                     0x000a9310..0x000a9488 (0x178)
//
//   dp_property_trans_hacker.cpp
//     DPPropertyDelayHacker::hack(...)             0x000b9e90..0x000b9f1a (0x8a)
//     DPPropertyDelayHacker::init(...)             0x000b88b0..0x000b8a2a (0x17a)
//
// The seven classes are thin IDPHacker plugins.  Everything they call that is
// owned by another translation unit (DPBillAdapter, CommBillRecord,
// DPMonitorReporter, DPMsgPacker, DPPkgDelayHacker::insert, get_cmd_id,
// DPPropertyTransImpl, ZEN trace, IDPHacker default slots) is declared here at
// its exact ABI and left undefined: those are integration boundaries, not
// reimplemented or stubbed.

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"
#include "src/formmog/tss_sdk/layout/dp_bill_recorder.h"
#include "src/formmog/tss_sdk/layout/dp_hacker.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_if.h"
#include "src/formmog/tss_sdk/layout/dp_hacker_safe.h"
#include "src/formmog/tss_sdk/layout/dp_property_trans.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"
#include "src/closing/dp_config.h"

// ---------------------------------------------------------------------------
// ORIG static-initialization blocks.  This slice is the primary reconstruction
// for six ORIG CUs:
//   dp_cmd_drop_hacker.cpp, dp_delay_hacker.cpp,
//   dp_filter_foward_hacker.cpp, dp_pkg_drop_hacker.cpp,
//   dp_pkg_dump_hacker.cpp, dp_pkg_foward_hacker.cpp
// (dp_property_trans_hacker.cpp carries the same block but its primary
// reconstruction is property_trans_impl_agent.cpp.)
//
// audit/report/init_brief.md: each init target is 996 bytes; calls =
// ios_base::Init 1, __cxa_atexit 3, ZEN_STATUS_ITEM_WITHNAME ctor 16,
// __cxa_guard_acquire/release 1 each, TssSdkCommSvc::TssSdkCommSvc 1,
// ~ZEN_STATUS_ITEM_WITHNAME 1, __cxa_guard_abort 1, _Unwind_Resume 1.
// The 16-entry status array and __ioinit are TU-local objects: emit one real
// block per ORIG CU.  The weak
// Singleton<TssSdkCommSvc>::ObjectCreator pre-create below is one shared
// object in the shipped DSO.
// ---------------------------------------------------------------------------
#define TSS_SDK_STATIC_INIT_ID dp_cmd_drop_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_delay_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_filter_foward_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_pkg_drop_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_pkg_dump_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID

#define TSS_SDK_STATIC_INIT_ID dp_pkg_foward_hacker
#include "src/static_init_block_impl.h"
#undef TSS_SDK_STATIC_INIT_ID


template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;

// ---------------------------------------------------------------------------
// sec_proto wire types touched by the assigned methods.
//
// DpDropCmdlistReq (DWARF size 808):
//   +0  uint16 channel_id_
//   +2  uint16 room_id_
//   +4  uint32 count_
//   +8  DpDropCmdlistInfo cmd_lists_[100]     (each 8 bytes)
//
// LATransAdaptDPCmdBufferMonitor (DWARF size 32786, packed):
//   +0  uint32 room_id_
//   +4  uint32 time_stamp_
//   +8  int32  monitor_cmd_id_
//   +12 uint8  is_need_encrypt_
//   +13 uint8  is_encrypted_
//   +14 uint32 data_len_        (unaligned)
//   +18 uint8  data_[32768]
// ---------------------------------------------------------------------------
namespace sec_proto {

struct DpBanModeReq;
struct DPCaptchaResultRsp;

struct DpDropCmdlistInfo {
    uint32_t cmdlist_id_;   // +0
    uint32_t keep_time_;    // +4
};

struct DpDropCmdlistReq {
    uint16_t channel_id_;               // +0
    uint16_t room_id_;                  // +2
    uint32_t count_;                    // +4
    DpDropCmdlistInfo cmd_lists_[100];  // +8
};

struct LATransAdaptDPCmdBufferMonitor {
    uint32_t room_id_;           // +0
    uint32_t time_stamp_;        // +4
    int32_t monitor_cmd_id_;     // +8
    uint8_t is_need_encrypt_;    // +12
    uint8_t is_encrypted_;       // +13
    uint32_t data_len_;          // +14 (unaligned)
    uint8_t data_[32768];        // +18
} __attribute__((packed));

}  // namespace sec_proto

// ---------------------------------------------------------------------------
// tss_sdk_impl::UserInfo comes from the canonical layout/user_info.h (pulled
// in by user_info_recon.h).  This slice reads:
//   base_info_.uin_/room_id_/flags_, and hack_data_.drop_cmd_ (time_t[8] at
//   UserInfo+0xe4, indexed by DPCmdDropHacker).
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// tss_dp_comm configuration comes from the shared ORIG-DWARF header
// (src/origsrc/formmog_conf_tsssdk_dp.h, included by src/closing/dp_config.h):
//   DPConfig 22842, cmd_hack at +0x3d7e; DPCmdHack 7100 with
//   delay_cmd +0x00, drop_subcmd_count +0x183c, drop_subcmd_list +0x183e,
//   forward_subcmd_count +0x198e, forward_subcmd_list +0x1990,
//   ban_mode +0x1ae0, drop_cmd +0x1b62 (actions +0x22), property_cmd +0x1b8c,
//   and DP_CONFIG::glb_dp_config_ at 0x002bb4c0 (owned by dp_config.cpp).
// ---------------------------------------------------------------------------
namespace tss_dp_comm {

// dp_config.h inline helpers, reproduced from their inlined bodies at
// 0x000a9d89 (is_match_filter) and 0x000a9dde (is_value_exist).
static inline bool is_match_filter(const DPSubCmdList &filter, int type,
                                   unsigned int cmd_id) {
    if (cmd_id != filter.cmd_id)
        return false;
    const unsigned char type_byte = (unsigned char)type;
    return (filter.cmd_id_type & type_byte) == type_byte;
}

static inline bool is_value_exist(const DPValueArray_8 &values,
                                  unsigned int value) {
    if (values.length == 0)
        return false;
    if (value < values.values[0])
        return false;
    if (value == values.values[0])
        return true;
    for (unsigned int i = 1; i < values.length; ++i) {
        if (value < values.values[i])
            return false;
        if (value == values.values[i])
            return true;
    }
    return false;
}

}  // namespace tss_dp_comm

// ---------------------------------------------------------------------------
// tss_dpsdk interfaces: IDPHacker, IDPHackerEventNotify and IDPHackerSafe are
// the canonical layout/dp_hacker*.h declarations (included above).  The seven
// reconstructed plugin classes below derive from them.
// ---------------------------------------------------------------------------

// ORIG comm_bill_record.h: CommBillRecord is a global struct; all ORIG call
// sites observe _ZN14CommBillRecordlsEx.  The earlier in-namespace
// declaration produced tss_dpsdk::CommBillRecord, which ORIG does not define.

namespace tss_dpsdk {

// ---------------------------------------------------------------------------
// DPBillAdapter boundary (DWARF size 4: user_ at +0).  The header's streaming
// helper (dp_bill_adapter.h around line 108) forwards every value to the
// process-wide CommBillRecord as a long long; bill_ is DPBillRecorder in ORIG
// (same data symbol, resolution is by name) and is defined by the bill TU.
// ---------------------------------------------------------------------------

class DPBillAdapter {
public:
    DPBillAdapter(const char *msg, unsigned int log_type,
                  tss_sdk_impl::UserInfo *user);
    ~DPBillAdapter();

    static int dump(const char *msg, unsigned int uin,
                    const unsigned char *data, unsigned int len);

    template <class T>
    DPBillAdapter &operator<<(T value) {
        bill_ << (long long)value;
        return *this;
    }

private:
    tss_sdk_impl::UserInfo *user_;   // +0
    static CommBillRecord bill_;
};

class DPMonitorReporter {
public:
    static int report_data(unsigned int id, unsigned int value);
};

// ---------------------------------------------------------------------------
// DPMsgPacker boundary.  DPPkgForwardLaHacker inlines
// send_datatrans_pkg<LATransAdaptDPCmdBufferMonitor> in ORIG; the
// reconstruction calls the real weak instantiation at 0x000b80d0.
// ---------------------------------------------------------------------------
class DPMsgPacker {
public:
    enum E_TIPS_WINDOWS {
        E_TIPS_NONE = 0,
        E_TIPS_SAFEMODE = 1,
        E_TIPS_SEND_SUCCESS = 2
    };

    static int revive_client_mouse(tss_sdk_impl::UserInfo *user,
                                   unsigned int cmd_id,
                                   E_TIPS_WINDOWS tips);

    template <class PKG>
    static int send_datatrans_pkg(tss_sdk_impl::UserInfo *user, int entire,
                                  const PKG &forward_pkg, unsigned int cmd);
};

template <>
int DPMsgPacker::send_datatrans_pkg<
    sec_proto::LATransAdaptDPCmdBufferMonitor>(
    tss_sdk_impl::UserInfo *user, int entire,
    const sec_proto::LATransAdaptDPCmdBufferMonitor &forward_pkg,
    unsigned int cmd);

// ---------------------------------------------------------------------------
// DPPkgDelayHacker is the canonical layout/dp_hacker_safe.h class (84 bytes:
// IDPHackerSafe 32 + map +0x20 + list +0x38 + timeval +0x40 + flag +0x48 +
// node_wait_for_send_ +0x4c + node_in_delay_ +0x50).  DPDelayHacker::hack
// below reads the two node counters, which is why the canonical header keeps
// them protected.

// DWARF 84 bytes: no members beyond DPPkgDelayHacker.
class DPDelayHacker : public DPPkgDelayHacker {
public:
    // Implicit constructor, matching ORIG (no out-of-line ctor symbol).
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// DWARF 84 bytes: no members beyond DPDelayHacker.  Its hack() delegates to
// the TU-local DPPropertyTransImpl object first, then to DPDelayHacker::hack.
class DPPropertyDelayHacker : public DPDelayHacker {
public:
    // Implicit constructor, matching ORIG (no out-of-line ctor symbol).
    virtual int init(IDPHackerEventNotify *event_notify);
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// DPPropertyTransImpl is the canonical layout/dp_property_trans.h class
// (IDPHackerSafe 32 + std::map uid2usr_ +0x20 = 56 bytes).

// Single definition lives in property_trans_impl_agent.cpp (the ORIG
// dp_property_trans_hacker.cpp owner) with hidden visibility because the
// reconstruction split that CU; this declaration mirrors the real 0x38-byte
// layout (IDPHackerSafe base + map at +32) so the direct hack() call is
// ABI-safe.
__attribute__((visibility("hidden"))) extern DPPropertyTransImpl propety_impl;

// ---------------------------------------------------------------------------
// DPPkgForwardLaHacker (DWARF 4 bytes).
// ---------------------------------------------------------------------------
class DPPkgForwardLaHacker : public IDPHacker {
public:
    DPPkgForwardLaHacker() {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// DWARF 4 bytes; derives DPPkgForwardLaHacker (not directly IDPHacker).
class DPFilterForwardLaHacker : public DPPkgForwardLaHacker {
public:
    DPFilterForwardLaHacker() {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// DWARF 4 bytes.
class DPPkgDropHacker : public IDPHacker {
public:
    DPPkgDropHacker() {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// DWARF 4 bytes.
class DPPkgDumpHacker : public IDPHacker {
public:
    DPPkgDumpHacker() {}
    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);
};

// DWARF 8 bytes: vptr + int cur_hacker_pos_ at +4.  DPHackMgr::init creates
// eight of these with ids 0..7.  init() is inline in the original header and
// therefore weak (0x000a6e40); its function-local static guard is the weak
// symbol DPCmdDropHacker::init(...)::inited at 0x002baea0.
class DPCmdDropHacker : public IDPHacker {
public:
    explicit DPCmdDropHacker(int pos) : cur_hacker_pos_(pos) {}

    virtual int init(IDPHackerEventNotify *event_notify) {
        static int inited = 0;
        if (!inited) {
            event_notify->register_event(E_DROP_CMD, this);
            inited = 1;
        }
        return 0;
    }

    virtual int on_drop_cmd(
        tss_sdk_impl::UserInfo *user,
        const sec_proto::DpDropCmdlistReq &pkg);

    virtual int hack(tss_sdk_impl::UserInfo *user, int type,
                     unsigned int cmd_id, unsigned char *&buf,
                     unsigned int &len);

private:
    int cur_hacker_pos_;  // +4
};

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// Layout guards (all values from DWARF / disassembly).
// ---------------------------------------------------------------------------
typedef char check_dp_sub_cmd_list[
    sizeof(tss_dp_comm::DPSubCmdList) == 42 ? 1 : -1];
typedef char check_dp_value_array_8[
    sizeof(tss_dp_comm::DPValueArray_8) == 34 ? 1 : -1];
typedef char check_dp_drop_cmd[
    sizeof(tss_dp_comm::DPDropCmd) == 42 ? 1 : -1];
typedef char check_dp_cmd_hack_offsets[
    (__builtin_offsetof(tss_dp_comm::DPCmdHack, delay_cmd) == 0 &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, delay_cmd.delay_cmd_time) ==
         0 &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, drop_subcmd_count) ==
         0x183c &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, drop_subcmd_list) ==
         0x183e &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, forward_subcmd_count) ==
         0x198e &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, forward_subcmd_list) ==
         0x1990 &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, drop_cmd) == 0x1b62 &&
     __builtin_offsetof(tss_dp_comm::DPCmdHack, drop_cmd.actions) == 0x1b84)
        ? 1 : -1];
typedef char check_dp_config_offsets[
    (__builtin_offsetof(tss_dp_comm::DPConfig, cmd_hack) == 0x3d7e &&
     __builtin_offsetof(tss_dp_comm::DPConfig,
                        cmd_hack.delay_cmd.delay_cmd_time) == 0x3d7e &&
     __builtin_offsetof(tss_dp_comm::DPConfig,
                        cmd_hack.drop_subcmd_count) == 0x55ba &&
     __builtin_offsetof(tss_dp_comm::DPConfig,
                        cmd_hack.drop_subcmd_list) == 0x55bc &&
     __builtin_offsetof(tss_dp_comm::DPConfig,
                        cmd_hack.forward_subcmd_count) == 0x570c &&
     __builtin_offsetof(tss_dp_comm::DPConfig,
                        cmd_hack.forward_subcmd_list) == 0x570e &&
     __builtin_offsetof(tss_dp_comm::DPConfig,
                        cmd_hack.drop_cmd) == 0x58e0 &&
     __builtin_offsetof(tss_dp_comm::DPConfig,
                        cmd_hack.drop_cmd.actions) == 0x5902)
        ? 1 : -1];
typedef char check_dp_cmd_hack_size[
    sizeof(tss_dp_comm::DPCmdHack) == 7100 ? 1 : -1];
typedef char check_dp_config_size[
    sizeof(tss_dp_comm::DPConfig) == 22842 ? 1 : -1];
typedef char check_user_info_offsets[
    (__builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.uin_) == 0 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.room_id_) == 0x08 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.flags_) == 0x20 &&
     __builtin_offsetof(tss_sdk_impl::UserInfo,
                        hack_data_.drop_cmd_) == 0xe4)
        ? 1 : -1];
typedef char check_user_info_size[
    sizeof(tss_sdk_impl::UserInfo) == 400 ? 1 : -1];
typedef char check_drop_cmdlist_req[
    (sizeof(sec_proto::DpDropCmdlistInfo) == 8 &&
     sizeof(sec_proto::DpDropCmdlistReq) == 808 &&
     __builtin_offsetof(sec_proto::DpDropCmdlistReq, count_) == 4 &&
     __builtin_offsetof(sec_proto::DpDropCmdlistReq, cmd_lists_) == 8)
        ? 1 : -1];
typedef char check_buffer_monitor[
    (sizeof(sec_proto::LATransAdaptDPCmdBufferMonitor) == 32786 &&
     __builtin_offsetof(sec_proto::LATransAdaptDPCmdBufferMonitor, room_id_) ==
         0 &&
     __builtin_offsetof(sec_proto::LATransAdaptDPCmdBufferMonitor,
                        is_need_encrypt_) == 12 &&
     __builtin_offsetof(sec_proto::LATransAdaptDPCmdBufferMonitor,
                        is_encrypted_) == 13 &&
     __builtin_offsetof(sec_proto::LATransAdaptDPCmdBufferMonitor,
                        data_len_) == 14 &&
     __builtin_offsetof(sec_proto::LATransAdaptDPCmdBufferMonitor, data_) ==
         18)
        ? 1 : -1];
typedef char check_hacker_sizes[
    (sizeof(tss_dpsdk::DPPkgForwardLaHacker) == 4 &&
     sizeof(tss_dpsdk::DPFilterForwardLaHacker) == 4 &&
     sizeof(tss_dpsdk::DPPkgDropHacker) == 4 &&
     sizeof(tss_dpsdk::DPPkgDumpHacker) == 4 &&
     sizeof(tss_dpsdk::DPCmdDropHacker) == 8 &&
     sizeof(tss_dpsdk::IDPHackerSafe) == 32 &&
     sizeof(tss_dpsdk::DPPkgDelayHacker) == 0x54 &&
     sizeof(tss_dpsdk::DPDelayHacker) == 0x54 &&
     sizeof(tss_dpsdk::DPPropertyDelayHacker) == 0x54 &&
     sizeof(tss_dpsdk::DPPropertyTransImpl) == 56)
        ? 1 : -1];
// ===========================================================================
// DPCmdDropHacker
// ===========================================================================
namespace tss_dpsdk {

// ORIG 0x000a66a0..0x000a6878.
//
// Decoded behaviour:
//   keep_time = user->hack_data_.drop_cmd_[cur_hacker_pos_]
//   if (keep_time == 0) return 0;
//   trace "[%u][%s], cur_hacker_pos_=%u, keep_time=%lu"
//   if (time(0) >= keep_time) { keep_time = 0; return 0; }
//   bill "drop cmd" with log_type = uin + 1000, then stream
//        type, cmd_id, *len, keep_time, uin
//   report_data(0x409 + cur_hacker_pos_, 1)
//   action = config->cmd_hack.drop_cmd.actions[cur_hacker_pos_]
//   if (action != 0) revive_client_mouse(user, cmd_id, action - 1)
//   return 1
//
// The buffer pointer/length are not modified.  There is no user-null guard
// (the ORIG dereferences user before the trace's null-aware priority test).
int DPCmdDropHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                          unsigned int cmd_id, unsigned char *&buf,
                          unsigned int &len) {
    (void)buf;
    const int pos = cur_hacker_pos_;
    time_t &keep_time =
        user->hack_data_.drop_cmd_[pos];  // user + 0xe4 + pos*4
    if (keep_time == 0)
        return 0;

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1u) != 0) ? RS_INFO
                                                          : RS_DEBUG,
        "[%u][%s], cur_hacker_pos_=%u, keep_time=%lu",
        user != 0 ? user->base_info_.uin_ : 0u, __func__,
        (unsigned int)pos, (unsigned long)keep_time);

    if (::time(0) >= keep_time) {
        keep_time = 0;
        return 0;
    }

    {
        DPBillAdapter bill("drop cmd",
                           user->base_info_.uin_ + 1000u, user);
        bill << type;
        bill << cmd_id;
        bill << len;
        bill << keep_time;
        bill << user->base_info_.uin_;

        DPMonitorReporter::report_data(0x409u + (unsigned int)pos, 1);

        const unsigned char action =
            tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.drop_cmd
                .actions[pos];
        if (action > 0) {
            DPMsgPacker::revive_client_mouse(
                user, cmd_id, (DPMsgPacker::E_TIPS_WINDOWS)(action - 1));
        }
    }
    return 1;
}

// ORIG 0x000a6880..0x000a69f5.
//
// Decoded behaviour:
//   if (pkg.count_ == 0) return 0;
//   now = time(0); bill "recv drop cmd", log_type 45, user
//   for (i = 0; i < pkg.count_; ++i) {
//     cmd  = pkg.cmd_lists_[i].cmdlist_id_
//     keep = pkg.cmd_lists_[i].keep_time_
//     index = cmd & 7;
//     cfg = config->cmd_hack.drop_cmd
//     if (cfg.length != 0) {
//        scan cfg.values[0..length-1] for cmd; on hit index = position;
//        if no hit index = cmd & 7;
//     }
//     user->hack_data_.drop_cmd_[index] = keep ? now + keep : 0;
//     bill << cmd << keep;
//   }
//   report_data(0x413, pkg.count_);
//   return 0
//
// The configured positions are indices into the same 8-slot drop_cmd_ array
// that the eight DPCmdDropHacker instances read; the instance id itself is
// not involved here.
int DPCmdDropHacker::on_drop_cmd(
    tss_sdk_impl::UserInfo *user,
    const sec_proto::DpDropCmdlistReq &pkg) {
    if (pkg.count_ == 0)
        return 0;

    const time_t now = ::time(0);
    {
        DPBillAdapter bill("recv drop cmd", 0x2d, user);

        for (uint32_t i = 0; i < pkg.count_; ++i) {
            const sec_proto::DpDropCmdlistInfo &info = pkg.cmd_lists_[i];
            const unsigned int cmd = info.cmdlist_id_;
            const unsigned int keep = info.keep_time_;

            unsigned int index = cmd & 7u;
            const tss_dp_comm::DPDropCmd &cfg =
                tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.drop_cmd;
            const uint16_t length = cfg.length;
            if (length != 0) {
                bool found = false;
                for (unsigned int j = 0; j < length; ++j) {
                    if (cfg.values[j] == cmd) {
                        index = j;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    index = cmd & 7u;
            }

            user->hack_data_.drop_cmd_[index] =
                keep != 0 ? now + (time_t)keep : (time_t)0;

            bill << cmd;
            bill << keep;
        }

        DPMonitorReporter::report_data(0x413, pkg.count_);
    }
    return 0;
}

// ===========================================================================
// DPPkgDropHacker
// ===========================================================================
// ORIG 0x000b6680..0x000b684d.
//
// Decoded behaviour:
//   sub_cmd_id = 0
//   count = config->cmd_hack.drop_subcmd_count_
//   if (count > 0) {
//     for each drop_subcmd_list_[i]:
//        if (!is_match_filter(entry, type, cmd_id)) continue;
//        exists = is_subcmd_exist(entry, buf, (uint16)len, sub_cmd_id)
//        if (exists == (entry.is_accept_ != 0)) goto drop;
//        return 0;                       // explicitly not selected
//   }
// drop:
//   bill "drop package", log_type 49:
//        type, cmd_id, sub_cmd_id, *len
//   report_data(0x402, 1)
//   return 1
//
// Thus an entry with is_accept_ selects its listed sub-commands and a zero
// is_accept_ entry selects the complement.  No matching cmd entry (or an
// empty count) selects the packet: the raw ORIG falls through to the drop
// path.  The buffer is never modified.
int DPPkgDropHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                          unsigned int cmd_id, unsigned char *&buf,
                          unsigned int &len) {
    unsigned int sub_cmd_id = 0;
    const tss_dp_comm::DPCmdHack &cfg =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack;
    const int count = (int)cfg.drop_subcmd_count;

    if (count > 0) {
        for (int i = 0; i < count; ++i) {
            const tss_dp_comm::DPSubCmdList &entry =
                cfg.drop_subcmd_list[i];
            if (!tss_dp_comm::is_match_filter(entry, type, cmd_id))
                continue;

            bool exists = false;
            if (tss_dp_comm::get_cmd_id(
                    entry.dp_subcmd_hack, buf,
                    (unsigned short)len, sub_cmd_id) == 0) {
                exists = tss_dp_comm::is_value_exist(entry.subcmd_list,
                                                     sub_cmd_id);
            }

            if (exists == (entry.is_accept != 0))
                goto drop;
            return 0;
        }
    }

drop:
    {
        DPBillAdapter bill("drop package", 0x31, user);
        bill << type;
        bill << cmd_id;
        bill << sub_cmd_id;
        bill << len;

        DPMonitorReporter::report_data(0x402, 1);
    }
    return 1;
}

// ===========================================================================
// DPPkgDumpHacker
// ===========================================================================
// ORIG 0x000b6d70..0x000b6de3.
//
// Decoded behaviour:
//   snprintf(name, 256, "DPPkgDumpHacker, type=%d,cmd_id=0X%X", type, cmd_id)
//   DPBillAdapter::dump(name, user->uin, buf, len)
//   return 0
// No config lookup, no bill adapter, no monitor report, no buffer change.
int DPPkgDumpHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                          unsigned int cmd_id, unsigned char *&buf,
                          unsigned int &len) {
    char name[0x100];
    ::snprintf(name, sizeof(name),
               "DPPkgDumpHacker, type=%d,cmd_id=0X%X", type, cmd_id);
    DPBillAdapter::dump(name, user->base_info_.uin_, buf, len);
    return 0;
}

// ===========================================================================
// DPFilterForwardLaHacker
// ===========================================================================
// ORIG 0x000a9d30..0x000a9e63.
//
// Decoded behaviour:
//   if (config->cmd_hack.forward_subcmd_count_ == 0) goto forward;
//   for each forward_subcmd_list_[i]:
//      if (!is_match_filter(entry, type, cmd_id)) continue;
//      exists = is_subcmd_exist(entry, buf, (uint16)len, sub_cmd_id)
//      if (exists == (entry.is_accept_ != 0)) goto forward;
//      return 0;                        // filtered out
// forward:
//   return DPPkgForwardLaHacker::hack(user, type, cmd_id, buf, len);
//
// When no cmd entry matches, the loop falls through to forward (the base
// hacker's own command/trace logic still runs).
int DPFilterForwardLaHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                                  unsigned int cmd_id, unsigned char *&buf,
                                  unsigned int &len) {
    const tss_dp_comm::DPCmdHack &cfg =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack;
    const int count = (int)cfg.forward_subcmd_count;

    if (count > 0) {
        for (int i = 0; i < count; ++i) {
            const tss_dp_comm::DPSubCmdList &entry =
                cfg.forward_subcmd_list[i];
            if (!tss_dp_comm::is_match_filter(entry, type, cmd_id))
                continue;

            unsigned int sub_cmd_id = 0;
            bool exists = false;
            if (tss_dp_comm::get_cmd_id(
                    entry.dp_subcmd_hack, buf,
                    (unsigned short)len, sub_cmd_id) == 0) {
                exists = tss_dp_comm::is_value_exist(entry.subcmd_list,
                                                     sub_cmd_id);
            }

            if (exists == (entry.is_accept != 0))
                goto forward;
            return 0;
        }
    }

forward:
    return DPPkgForwardLaHacker::hack(user, type, cmd_id, buf, len);
}

// ===========================================================================
// DPPkgForwardLaHacker
// ===========================================================================
// ORIG 0x000b7720..0x000b7e06.
//
// Decoded behaviour:
//   LATransAdaptDPCmdBufferMonitor pkg;
//   pkg.room_id_          = user->base_info_.room_id_;   // user+0x08
//   pkg.time_stamp_       = (uint32)time(0);
//   pkg.monitor_cmd_id_   = (int32)cmd_id;
//   pkg.is_need_encrypt_  = 1;
//   pkg.is_encrypted_     = (type != 4);
//   pkg.data_len_         = len;                         // original length
//   if (len != 0) {
//       unsigned int copy_len = len;
//       if (copy_len > 0x8000) copy_len = 0x8000;        // local only
//       memcpy(pkg.data_, buf, copy_len);
//   }
//   trace "[%u][%s], forward pkg to la"
//   DPMsgPacker::send_datatrans_pkg(user, 0x23, pkg, 0x5573f);
//   DPMonitorReporter::report_data(0x403, 1);
//   return 0;
//
// The send result only feeds that template's own error logging; the hacker
// always reports and returns 0, i.e. the original packet is not intercepted.
// Neither the buffer contents nor len is modified.
int DPPkgForwardLaHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                               unsigned int cmd_id, unsigned char *&buf,
                               unsigned int &len) {
    sec_proto::LATransAdaptDPCmdBufferMonitor pkg;
    pkg.room_id_ = user->base_info_.room_id_;
    pkg.time_stamp_ = (uint32_t)::time(0);
    pkg.monitor_cmd_id_ = (int32_t)cmd_id;
    pkg.is_need_encrypt_ = 1;
    pkg.is_encrypted_ = (type != 4) ? 1 : 0;
    pkg.data_len_ = len;

    if (len != 0) {
        unsigned int copy_len = len;
        if (copy_len > 0x8000u)
            copy_len = 0x8000u;
        ::memcpy(pkg.data_, buf, copy_len);
    }

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1u) != 0) ? RS_INFO
                                                          : RS_DEBUG,
        "[%u][%s], forward pkg to la",
        user != 0 ? user->base_info_.uin_ : 0u, __func__);

    DPMsgPacker::send_datatrans_pkg(user, 0x23, pkg, 0x5573fu);
    DPMonitorReporter::report_data(0x403, 1);
    return 0;
}

// ===========================================================================
// DPDelayHacker
// ===========================================================================
// ORIG 0x000a9310..0x000a9488.
//
// Decoded behaviour:
//   if (type != 1) return 0;
//   res = DPPkgDelayHacker::insert(
//             user, config->cmd_hack.delay_cmd_.delay_cmd_time,
//             cmd_id, buf, len);
//   bill ((res == 0) ? "delay cmd" : "delay cmd fail"), log_type 55, user
//        cmd_id, *len, node_wait_for_send_, node_in_delay_,
//        config->cmd_hack.delay_cmd_.delay_cmd_time
//   return (res == 0) ? 1 : 0
//
// A successful insert means the packet has been copied into the delay queue,
// so the caller must drop the original (1).  Insert failure lets it through.
int DPDelayHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                        unsigned int cmd_id, unsigned char *&buf,
                        unsigned int &len) {
    if (type != 1)
        return 0;
    const unsigned char delay_time =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->cmd_hack.delay_cmd
            .delay_cmd_time;
    const int res =
        insert(user, (int)delay_time, cmd_id, buf, len);

    {
        DPBillAdapter bill(res == 0 ? "delay cmd" : "delay cmd fail", 0x37,
                           user);
        bill << cmd_id;
        bill << len;
        bill << node_wait_for_send_;
        bill << node_in_delay_;
        bill << delay_time;
    }

    return res == 0 ? 1 : 0;
}

// ===========================================================================
// DPPropertyDelayHacker
// ===========================================================================
// ORIG 0x000b9e90..0x000b9f1a.
//
// Decoded behaviour:
//   res = propety_impl.hack(user, type, cmd_id, buf, len);
//   if (res != 0)
//       res = DPDelayHacker::hack(user, type, cmd_id, buf, len);
//   return res;
//
// The TU-local DPPropertyTransImpl handles property-command forwarding first;
// when it does not intercept, the inherited delay behaviour runs.  The direct
// call target is the out-of-line DPPropertyTransImpl::hack symbol.
int DPPropertyDelayHacker::hack(tss_sdk_impl::UserInfo *user, int type,
                                unsigned int cmd_id, unsigned char *&buf,
                                unsigned int &len) {
    int res = propety_impl.DPPropertyTransImpl::hack(
        user, type, cmd_id, buf, len);
    if (res != 0)
        res = DPDelayHacker::hack(user, type, cmd_id, buf, len);
    return res;
}

// ORIG 0x000b88b0..0x000b8a2a.
//
// Decoded behaviour:
//   event_notify->register_event(E_LOGOUT, &propety_impl);
//   event_notify->register_event(E_PROC, this);
//   event_notify->register_event(E_SPECIAL_CONTROL, this);
//   return 0;
//
// This override does not call DPPkgDelayHacker::init: logout is delegated to
// the static property implementation, while proc/special-control stay on the
// delay hacker.  The register_event body is the inline de-duplicating append
// (vectors at event_notify+0x10, +0x1c and +0x64).
int DPPropertyDelayHacker::init(IDPHackerEventNotify *event_notify) {
    event_notify->register_event(E_LOGOUT, &propety_impl);
    event_notify->register_event(E_PROC, this);
    event_notify->register_event(E_SPECIAL_CONTROL, this);
    return 0;
}

}  // namespace tss_dpsdk
