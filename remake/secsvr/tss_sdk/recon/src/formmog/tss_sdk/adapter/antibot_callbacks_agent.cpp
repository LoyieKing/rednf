// Reconstructed from libtss_sdk.so.1.0.394
//   (SHA-256 8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7,
//    ELF32 i386, GCC 4.1.0 SUSE, DWARF present).
//
// Partial translation unit for
//   src/formmog/tss_sdk/adapter/antibotinterface.cpp
// holding the eight plugin callbacks that AntibotInterface::Initialize()
// registers in plugin_array (source lines 94-281 of the CU):
//
//   0x00088770  process_safemode_mba_set_verify_status_cb_    0x40  lines 219-237
//   0x000887b0  process_safemode_mba_set_verify_options_cb_   0x48  lines 197-216
//   0x00088800  process_safemode_mba_set_safe_status_cb_      0x39  lines 182-194
//   0x00088840  process_safemode_set_limit_status_cb_         0x4e  lines 156-179
//   0x00088890  process_safemode_msg_cb_                      0x55  lines 143-152
//   0x000888f0  process_safemode_cb_                          0x82  lines 241-281
//   0x00089120  send_buffer_to_client_cb_                     0x140 lines 94-102
//   0x00089260  punish_user_cb_                               0x1eb lines 106-139
//
// Code sizes are the `nm -S` sizes of the original symbols; the line numbers
// are the CU's DWARF decl/statement lines (objdump -dl).
//
// The companion adapter files already reconstructed by other agents cover the
// rest of the same original TU:
//   antibot_client_agent.cpp   Initialize / AddClient / DeleteClient
//                              (defines g_tss_sdk, declares g_business and
//                               g_channel_id extern, registers these callbacks)
//   antibot_packets_agent.cpp  OnRecvBufferFromClient / RecvBufferForEncrypt /
//                              RecvBufferForDecrypt / OnNotifyServer
//   antibot_policy_agent.cpp   IsCmdInUpList / IsCmdInDownList /
//                              QueryProtectPolicy / ProcessAntiBotInput /
//                              AntiCheatPacket
//   antibotinterface_ctor_agent.cpp, antibotinterface_trivial.cpp
//
// Nothing here is byte-identical to the shipped object; it is a semantic/ABI
// reconstruction.  Deliberate, evidence-backed choices are marked [EVIDENCE]
// in the comments below.

#include <stddef.h>

// ---------------------------------------------------------------------------
// Trace helpers (real symbols in the target library, variadic).
// ---------------------------------------------------------------------------
struct ZEN_Trace_LogMsg {
    static void debug_infoex(const char *str_format, ...);
    static void debug_debugEx(const char *str_format, ...);
    static void debug_errorex(const char *str_format, ...);
};

// ---------------------------------------------------------------------------
// IBusiness re-entry.
//
// `g_business` (0x0029d1a0) is an `IBusiness *`; it is set by
// AntibotInterface::Initialize() and read by these callbacks.  In this CU's
// DWARF `IBusiness` is a member-less declaration (the interface itself lives on
// the game-server side, ibusiness.h), so only the one thing the library does
// with it is modelled: the three call sites load the vptr from the object
// (0x8918c, 0x8937b, 0x888cd) and call the slots at +0x00, +0x04 and +0x08.
// The slot names below are positional (they encode the recovered byte offset);
// the argument lists are read off those call sites, they are not original
// declarations.
// ---------------------------------------------------------------------------
struct IBusiness;
struct tagAntibotPunishInfo;   // defined below; declared in ibusiness.h

struct IBusinessVtable {
    // 0x891bb: slot +0x00.  args: pkg->busi_type_, g_channel_id, 0,
    //          pkg->uin_, pkg->buf_, (int)pkg->buf_size_ -> int
    int (*slot_0x00)(IBusiness *self, int busi_type, unsigned int channel_id,
                     int reserved, unsigned int uin, unsigned char *buf,
                     int buf_size);

    // 0x893aa: slot +0x04.  args: g_channel_id, 0, pkg->uin_,
    //          pkg->punish_mode_, &punish_info, punish_reason -> int
    int (*slot_0x04)(IBusiness *self, unsigned int channel_id, int reserved,
                     unsigned int uin, int punish_mode,
                     const tagAntibotPunishInfo *punish_info,
                     const unsigned char *punish_reason);

    // 0x888d6: slot +0x08.  arg: the callback's own sec_data pointer.
    int (*slot_0x08)(IBusiness *self, void *sec_data);
};

struct IBusiness {
    IBusinessVtable *vtable_;   // [EVIDENCE] first dword of the object
};

// Original file-scope globals of the TU (local 'b' symbols in ORIG).  The
// single definitions live in antibot_client_agent.cpp with hidden visibility
// because the reconstruction split the original antibotinterface.cpp.
__attribute__((visibility("hidden"))) extern IBusiness *g_business;  // 0x0029d1a0
__attribute__((visibility("hidden"))) extern unsigned int g_channel_id;  // 0x0029d1a4

// ---------------------------------------------------------------------------
// Function-local core-dump traces used by send_buffer_to_client_cb_ and
// punish_user_cb_ (antibotinterface.cpp lines 51-85 of the same CU's DWARF:
// ctor line 53/55, enter line 61/62, leave line 66, adapter ctor line 71,
// adapter dtor line 75, adapter member line 81, core_pkg_ member line 85).
//
// The template itself is NOT packed (only the packet types are):
//   CTssCoreDumpPkgTrace<TssSdkPluginChannelGameChannelPkg> : size 15,
//       is_in_func_ @0, core_pkg_ @1   (T has alignment 1)
//   CTssCoreDumpPkgTrace<tagAntibotPunishInfo> : size 32,
//       is_in_func_ @0, core_pkg_ @4   (T has alignment 4 - see below)
// ---------------------------------------------------------------------------
template <typename T>
class CTssCoreDumpPkgTrace {
public:
    CTssCoreDumpPkgTrace(const char *func, const char *type) {
        is_in_func_ = 0;
        ZEN_Trace_LogMsg::debug_infoex(
            "CTssCoreDumpPkgTrace, func=%s, type=%s, this=0x%p, core_pkg_=0x%p",
            func, type, (const void *)this, (const void *)&core_pkg_);
    }

    void enter(const T &pkg) {
        is_in_func_ = 1;
        core_pkg_ = pkg;
    }

    void leave() {
        is_in_func_ = 0;
    }

private:
    char is_in_func_;
    T core_pkg_;
};

template <typename T>
class CTssCoreDumpPkgTraceAdapter {
public:
    CTssCoreDumpPkgTraceAdapter(CTssCoreDumpPkgTrace<T> &dump_info,
                                const T &pkg)
        : dump_info_(dump_info) {
        dump_info_.enter(pkg);
    }

    ~CTssCoreDumpPkgTraceAdapter() {
        dump_info_.leave();
    }

private:
    // DWARF prints this member as `const CTssCoreDumpPkgTrace<T> &`; the
    // inlined ctor/enter/leave mutate the trace, so the reconstructed adapter
    // keeps a non-const reference (no symbol is emitted either way).
    CTssCoreDumpPkgTrace<T> &dump_info_;
};

// ---------------------------------------------------------------------------
// Packet layouts.  All offsets/sizes are from the DWARF of this CU and are
// cross-checked against the load/store offsets in the disassembly.
//
//   ._495 = TssSdkPluginChannelGameChannelPkg   (size 14, alignment 1)
//           named by the trace type string at 0x218644
//   ._513 = TssSdkPluginUserControlPunishUser   (size 32) - the name is the one
//           used by the reconstructed user-control plugin CU (identical member
//           names/offsets there), this CU only has the anonymous type.
//   ._532 = the safemode plugin package (size 90, alignment 1).  Again
//           anonymous here; the member names are the ones the reconstructed
//           safemode plugin CU uses (same member names/offsets).
// ---------------------------------------------------------------------------
#pragma pack(push, 1)

struct TssSdkPluginChannelGameChannelPkg {      // sizeof == 14, packed
    int busi_type_;                 // +0
    unsigned int uin_;              // +4
    unsigned char *buf_;            // +8
    short buf_size_;                // +12
};

struct TssSdkPluginUserControlPunishUser {      // sizeof == 32, packed
    unsigned int uin_;              // +0
    unsigned short game_id_;        // +4
    unsigned short room_id_;        // +6
    int punish_mode_;               // +8
    int punish_time_;               // +12
    unsigned int punish_info_len_;  // +16
    const unsigned char *punish_info_;    // +20
    unsigned int punish_reason_len_;      // +24
    const unsigned char *punish_reason_;  // +28
};

struct SafeModeHead {                           // sizeof == 6
    unsigned short msg_type_;       // +0
    unsigned int uin_;              // +2
};

struct SafeModeUpdate {                         // sizeof == 38
    unsigned short ipc_days_;       // +0
    signed char safemode_level_;    // +2   (read with movsbl at 0x888aa)
    signed char has_verify_mb_;     // +3
    unsigned short event_type_;     // +4
    signed char reserve_[32];       // +6
};

struct SafeModeLimitStatus {                    // sizeof == 20
    int ret_;                       // +0
    unsigned int uin_;              // +4
    int limit_status_;              // +8
    unsigned int unlock_url_len_;   // +12
    char *unlock_url_;              // +16
};

struct SafeModeMbaSafeStatus {                  // sizeof == 8
    unsigned int uin_;              // +0
    unsigned int status_;           // +4
};

struct SafeModeMbaVerifyInfo {                  // sizeof == 14
    unsigned int uin_;              // +0
    unsigned int seq_;              // +4
    short verify_url_len_;          // +8   (read with movswl at 0x887cc)
    char *verify_url_;              // +10
};

struct SafeModeMbaVerifyStatus {                // sizeof == 12
    unsigned int uin_;              // +0
    unsigned int seq_;              // +4
    unsigned int result_;           // +8
};

struct SafeModeParam {                          // sizeof == 8 (._529)
    char *param_key_;               // +0
    char *param_value_;             // +4
};

struct SafeModeAppendParam {                    // sizeof == 16 (._530)
    char *url_;                     // +0
    unsigned int url_buf_max_len_;  // +4
    unsigned int param_count_;      // +8
    SafeModeParam *param_;          // +12
};

struct SafeModeMbaLaunchVerify {                // sizeof == 84 (._526)
    unsigned int uin_;              // +0
    unsigned int seq_;              // +4
    unsigned int op_level_;         // +8
    unsigned int op_scene_len_;     // +12
    char op_scene_[64];             // +16
    unsigned int client_ip_;        // +80
};

union SafeModeBody {                            // sizeof == 84 (._531)
    SafeModeUpdate safemode_update_;
    SafeModeUpdate safemode_ack_;
    SafeModeLimitStatus limit_status_;
    SafeModeAppendParam append_param_;
    SafeModeMbaSafeStatus mba_safe_status_;
    SafeModeMbaVerifyInfo mba_verify_info_;
    SafeModeMbaLaunchVerify mba_launch_verify_;
    SafeModeMbaVerifyStatus mba_verify_status_;
};

struct SafeModePkg {                            // sizeof == 90
    SafeModeHead head_;             // +0
    SafeModeBody body_;             // +6
};

#pragma pack(pop)

// ubuntu/ibusiness.h: sizeof == 28, natural alignment (so the packed
// CTssCoreDumpPkgTrace above comes out 32 bytes with core_pkg_ at +4, exactly
// as DWARF reports for CTssCoreDumpPkgTrace<tagAntibotPunishInfo>).
struct tagAntibotPunishInfo {
    int iGameID;                    // +0
    int iBanTimeSecond;             // +4
    int iVerifyTimeout;             // +8
    int iVerifyMaxErrorCount;       // +12
    int iVerifyLevel;               // +16
    unsigned int punish_info_len;   // +20
    const unsigned char *punish_info;     // +24
};
typedef tagAntibotPunishInfo AntibotPunishInfo;

// Compile-time layout checks against the DWARF byte sizes.
typedef char chk_game_channel_pkg[
    sizeof(TssSdkPluginChannelGameChannelPkg) == 14 ? 1 : -1];
typedef char chk_punish_user_pkg[
    sizeof(TssSdkPluginUserControlPunishUser) == 32 ? 1 : -1];
typedef char chk_safemode_pkg[sizeof(SafeModePkg) == 90 ? 1 : -1];
typedef char chk_safemode_body[sizeof(SafeModeBody) == 84 ? 1 : -1];
typedef char chk_punish_info[sizeof(AntibotPunishInfo) == 28 ? 1 : -1];
typedef char chk_trace_channel[
    sizeof(CTssCoreDumpPkgTrace<TssSdkPluginChannelGameChannelPkg>) == 15 ?
    1 : -1];
typedef char chk_trace_punish[
    sizeof(CTssCoreDumpPkgTrace<tagAntibotPunishInfo>) == 32 ? 1 : -1];

// ===========================================================================
// process_safemode_mba_set_verify_status_cb_   0x88770..0x887af  lines 219-237
//
// Logs body_.mba_verify_status_ (uin/seq/result, three dwords at pkg+6/+0xa/
// +0xe) and returns 0.  exec: 0x887a3 debug_debugEx, 0x887ab xor eax,eax.
// ===========================================================================
int process_safemode_mba_set_verify_status_cb_(void *sec_data) {
    SafeModePkg *pkg = (SafeModePkg *)sec_data;

    ZEN_Trace_LogMsg::debug_debugEx(
        "in process_safemode_mba_set_verify_status_cb_. "
        "uin=%u, seq=%u, result=%u",
        pkg->body_.mba_verify_status_.uin_,
        pkg->body_.mba_verify_status_.seq_,
        pkg->body_.mba_verify_status_.result_);
    return 0;
}

// ===========================================================================
// process_safemode_mba_set_verify_options_cb_  0x887b0..0x887f7  lines 197-216
//
// Logs body_.mba_verify_info_.  The format string still says
// "process_safemode_mba_set_verify_info_cb_" (0x218330) - that mismatch with
// the function name is in the shipped object and is preserved verbatim.
// verify_url_len_ is a short read with movswl, verify_url_ is the pointer at
// pkg+0x10.
// ===========================================================================
int process_safemode_mba_set_verify_options_cb_(void *sec_data) {
    SafeModePkg *pkg = (SafeModePkg *)sec_data;

    ZEN_Trace_LogMsg::debug_debugEx(
        "in process_safemode_mba_set_verify_info_cb_. "
        "uin=%u, seq=%u, verify_url_len=%d, verify_url=%s",
        pkg->body_.mba_verify_info_.uin_,
        pkg->body_.mba_verify_info_.seq_,
        (int)pkg->body_.mba_verify_info_.verify_url_len_,
        pkg->body_.mba_verify_info_.verify_url_);
    return 0;
}

// ===========================================================================
// process_safemode_mba_set_safe_status_cb_     0x88800..0x88838  lines 182-194
// ===========================================================================
int process_safemode_mba_set_safe_status_cb_(void *sec_data) {
    SafeModePkg *pkg = (SafeModePkg *)sec_data;

    ZEN_Trace_LogMsg::debug_debugEx(
        "in process_safemode_mba_set_safe_status_cb_. uin=%u, status=%u",
        pkg->body_.mba_safe_status_.uin_,
        pkg->body_.mba_safe_status_.status_);
    return 0;
}

// ===========================================================================
// process_safemode_set_limit_status_cb_        0x88840..0x8888d  lines 156-179
//
// DWARF local `char *url_ptr` (line 168) is the body_.limit_status_.unlock_url_
// pointer; the shipped code loads it straight into the last vararg slot.
// ===========================================================================
int process_safemode_set_limit_status_cb_(void *sec_data) {
    SafeModePkg *pkg = (SafeModePkg *)sec_data;

    char *url_ptr = pkg->body_.limit_status_.unlock_url_;

    ZEN_Trace_LogMsg::debug_debugEx(
        "recv limit status safemode cb. ret=%d, uin=%u, limit_status=%d, "
        "unlock_url_len=%d, unlock_url=%s\n",
        pkg->body_.limit_status_.ret_,
        pkg->body_.limit_status_.uin_,
        pkg->body_.limit_status_.limit_status_,
        pkg->body_.limit_status_.unlock_url_len_,
        url_ptr);
    return 0;
}

// ===========================================================================
// process_safemode_msg_cb_                     0x88890..0x888e4  lines 143-152
//
// Logs head_.uin_ and body_.safemode_update_.safemode_level_ (signed char),
// then re-enters the business layer through IBusiness's vtable slot +0x08 with
// the *original* callback argument (0x888d6 passes sec_data, not the cast).
// The slot's return value is discarded, the callback returns 0.
// ===========================================================================
int process_safemode_msg_cb_(void *sec_data) {
    SafeModePkg *pkg = (SafeModePkg *)sec_data;

    ZEN_Trace_LogMsg::debug_debugEx(
        "recv update safemode cb. uin=%u level=%d",
        pkg->head_.uin_,
        (int)pkg->body_.safemode_update_.safemode_level_);

    g_business->vtable_->slot_0x08(g_business, sec_data);
    return 0;
}

// ===========================================================================
// process_safemode_cb_                         0x888f0..0x88971  lines 241-281
//
// Dispatch on the u16 head_.msg_type_.  The shipped code uses a relative jump
// table at 0x218d90 (base 0x294534, `mov -0x7b7a4(%ebx,%eax,4),%eax` /
// `add %ebx,%eax` at 0x88914); decoded entries:
//
//   18 (0x12) -> 0x88936  process_safemode_msg_cb_
//   19,20,22,24 -> 0x88920  default (debug_errorex "unknown msg_type")
//   21 (0x15) -> 0x88962  process_safemode_set_limit_status_cb_
//   23 (0x17) -> 0x88954  process_safemode_mba_set_safe_status_cb_
//   25 (0x19) -> 0x8894a  process_safemode_mba_set_verify_options_cb_
//   26 (0x1a) -> 0x88940  process_safemode_mba_set_verify_status_cb_
//
// (= msg_type - 18 in [0,8], `cmp $0x8,%ax; ja` at 0x8890b.)  Every path
// returns 0; the case handlers are called with the cast pkg pointer.
// ===========================================================================
int process_safemode_cb_(void *sec_data) {
    SafeModePkg *pkg = (SafeModePkg *)sec_data;

    switch (pkg->head_.msg_type_) {
    case 18:                                            // safemode update
        process_safemode_msg_cb_(pkg);
        return 0;

    case 21:                                            // limit status
        process_safemode_set_limit_status_cb_(pkg);
        return 0;

    case 23:                                            // MBA safe status
        process_safemode_mba_set_safe_status_cb_(pkg);
        return 0;

    case 25:                                            // MBA verify info
        process_safemode_mba_set_verify_options_cb_(pkg);
        return 0;

    case 26:                                            // MBA verify status
        process_safemode_mba_set_verify_status_cb_(pkg);
        return 0;

    default:
        ZEN_Trace_LogMsg::debug_errorex(
            "in process_safemode_cb_, unknown msg_type");
        return 0;
    }
}

// ===========================================================================
// send_buffer_to_client_cb_                    0x89120..0x8925f  lines 94-102
//
// pkg (TssSdkPluginChannelGameChannelPkg *) is copied into the function-local
// static trace, then the business layer is re-entered through slot +0x00:
//
//   ret = slot(pkg->busi_type_, g_channel_id, 0, pkg->uin_, pkg->buf_,
//              (int)pkg->buf_size_);
//
//   trace static : guard 0x0029d1e0 (8 B), object 0x0029d1e8 (15 B):
//                  is_in_func_ 0x0029d1e8, core_pkg_ 0x0029d1e9..0x0029d1f6
//   trace ctor   : __func__ "send_buffer_to_client_cb_" (0x218e62),
//                  type "TssSdkPluginChannelGameChannelPkg" (0x218644)
//   log          : "send buffer to client: uin=%u type=%d len=%d ret=%d"
//                  (0x218668), args uin_, busi_type_, (int)buf_size_, ret
// Returns 0 unconditionally (0x891ee).
// ===========================================================================
int send_buffer_to_client_cb_(void *sec_data) {
    TssSdkPluginChannelGameChannelPkg *pkg =
        (TssSdkPluginChannelGameChannelPkg *)sec_data;

    static CTssCoreDumpPkgTrace<TssSdkPluginChannelGameChannelPkg>
        tmp_core_dump_info(__func__, "TssSdkPluginChannelGameChannelPkg");
    CTssCoreDumpPkgTraceAdapter<TssSdkPluginChannelGameChannelPkg>
        temp_trace(tmp_core_dump_info, *pkg);

    int ret = g_business->vtable_->slot_0x00(
        g_business, pkg->busi_type_, g_channel_id, 0, pkg->uin_, pkg->buf_,
        (int)pkg->buf_size_);

    ZEN_Trace_LogMsg::debug_debugEx(
        "send buffer to client: uin=%u type=%d len=%d ret=%d",
        pkg->uin_, pkg->busi_type_, (int)pkg->buf_size_, ret);
    return 0;
}

// ===========================================================================
// punish_user_cb_                              0x89260..0x8944a  lines 106-139
//
// 1. punish_info (AntibotPunishInfo, 28 B at CFA-60 = ebp-0x34) is filled at
//    line 116 from g_channel_id and the packet:
//        iGameID             = g_channel_id
//        iBanTimeSecond      = pkg->punish_time_
//        iVerifyTimeout      = 0
//        iVerifyMaxErrorCount= 0
//        iVerifyLevel        = 0
//        punish_info_len     = pkg->punish_info_len_
//        punish_info         = pkg->punish_info_
//    (0x89277..0x892d5; the same line also zeroes a second, never-read 28-byte
//    stack temporary at ebp-0x58 - compiler temporary, no DWARF entry, so it is
//    not modelled.)
// 2. debug_debugEx of the packet fields (line 125, format 0x21869c, 8 varargs:
//    g_channel_id, uin_, punish_mode_, punish_time_, game_id_, room_id_,
//    punish_info_len_, punish_reason_len_ - game_id_/room_id_ are zero-extended
//    u16 loads).
// 3. trace static: guard 0x0029d1a8 (8 B), object 0x0029d1c0 (32 B):
//    is_in_func_ 0x0029d1c0, core_pkg_ 0x0029d1c4..0x0029d1df; ctor __func__
//    "punish_user_cb_" (0x218e52), type "AntibotPunishInfo" (0x219044).  The
//    adapter copies the *local* AntibotPunishInfo, not the packet.
// 4. punish_reason is 0 unless pkg->punish_reason_len_ is non-zero (lines
//    128-131; the load of pkg->punish_reason_ at 0x89372 is guarded).
// 5. rc = slot +0x04(g_channel_id, 0, pkg->uin_, pkg->punish_mode_,
//                   &punish_info, punish_reason)          (0x893aa)
// 6. debug_debugEx("In punish_user_cb_, rc=%d.", rc)       (0x219056)
// 7. return 0 unconditionally (0x893c9).
//
// ===========================================================================
int punish_user_cb_(void *sec_data) {
    TssSdkPluginUserControlPunishUser *pkg =
        (TssSdkPluginUserControlPunishUser *)sec_data;
    AntibotPunishInfo punish_info;

    punish_info.iGameID = (int)g_channel_id;
    punish_info.iBanTimeSecond = pkg->punish_time_;
    punish_info.iVerifyTimeout = 0;
    punish_info.iVerifyMaxErrorCount = 0;
    punish_info.iVerifyLevel = 0;
    punish_info.punish_info_len = pkg->punish_info_len_;
    punish_info.punish_info = pkg->punish_info_;

    ZEN_Trace_LogMsg::debug_debugEx(
        "In punish_user_cb_, channel_id=%u, uin=%u, punish_mode=%d, "
        "punish_duration=%u, game_id=%u, room_id=%u, punish_info_len=%u, "
        "punish_reason_len=%d",
        g_channel_id, pkg->uin_, pkg->punish_mode_,
        (unsigned int)pkg->punish_time_, (unsigned int)pkg->game_id_,
        (unsigned int)pkg->room_id_, pkg->punish_info_len_,
        (int)pkg->punish_reason_len_);

    static CTssCoreDumpPkgTrace<tagAntibotPunishInfo>
        tmp_core_dump_info(__func__, "AntibotPunishInfo");
    CTssCoreDumpPkgTraceAdapter<tagAntibotPunishInfo>
        temp_trace(tmp_core_dump_info, punish_info);

    const unsigned char *punish_reason = 0;
    if (pkg->punish_reason_len_ != 0) {
        punish_reason = pkg->punish_reason_;
    }

    int rc = g_business->vtable_->slot_0x04(
        g_business, g_channel_id, 0, pkg->uin_, pkg->punish_mode_,
        &punish_info, punish_reason);

    ZEN_Trace_LogMsg::debug_debugEx("In punish_user_cb_, rc=%d.", rc);
    return 0;
}
