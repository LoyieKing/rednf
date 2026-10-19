// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.0, -O2 -fPIC).
//
// Recovered symbols and ORIG extents (nm -S --defined-only):
//   _ZN9tss_dpsdk12DPMsgHandlerC1Ev   0x000ae2f0 + 0x54
//   _ZN9tss_dpsdk12DPMsgHandlerC2Ev   0x000ae350 + 0x54
//   _ZN9tss_dpsdk12DPMsgHandlerD1Ev   0x000ae070 + 0x05
//   _ZN9tss_dpsdk12DPMsgHandlerD2Ev   0x000ae060 + 0x05
//   _ZN9tss_dpsdk12DPMsgHandler4initEv        0x000ae2c0 + 0x25
//   _ZN9tss_dpsdk12DPMsgHandler6reloadEv      0x000ae290 + 0x25
//   _ZN9tss_dpsdk12DPMsgHandler15is_need_decryptEPN12tss_sdk_impl8UserInfoEi
//                                             0x000ae080 + 0x8d
//   _ZN9tss_dpsdk12DPMsgHandler15is_need_encryptEPN12tss_sdk_impl8UserInfoEi
//                                             0x000ae730 + 0x8d
//   _ZN9tss_dpsdk12DPMsgHandler13anticheck_pkgEPN12tss_sdk_impl8UserInfoEi
//                                             0x000ae7c0 + 0x13d
//
// DWARF (dp_msg_handler.h:27/29..36) fixes the class as 4 bytes with the single
// private member vm_engine_ at +0x00.  DWARF returns are:
//   init/reload/anticheck_pkg -> int, is_need_decrypt/is_need_encrypt -> bool.
//
// The helper types below are local declaration/layout models only.  They are
// used to emit the exact original mangled references and byte offsets; the
// real classes and objects are linked from their own TSS SDK translation
// units.  This file intentionally does not modify or include the shared
// dp_msg_handler/dp_config/user_info headers.
//
// INTEGRATION BOUNDARY: every model class is a partial view, not a claim that
// these are the complete original declarations.

// ---------------------------------------------------------------------------
// tss_sdk_impl::UserInfo (DWARF user_info.h: UserInfo is 0x190 bytes;
// user_code_info_ +0x64 (0x30), user_timer_ +0x94 (0x14), user_stat_ +0xa8
// (0x34), hack_data_ +0xdc).
// ---------------------------------------------------------------------------
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/layout/dp_vmbase_engine.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

namespace tss_dp_comm {

// dp_config.h header-inline membership test, inlined at 0xae0cd / 0xae77d
// (loop body dp_config.h:36/40).  It has no out-of-line symbol in ORIG.
template <typename T>
inline bool is_value_exist(const T &values, unsigned int value)
{
    for (unsigned short i = 0; i < values.length; ++i) {
        if (values.values[i] > value)
            return false;
        if (values.values[i] == value)
            return true;
    }
    return false;
}

}  // namespace tss_dp_comm

namespace tss_dpsdk {

class DPMsgPacker {
public:
    static int send_error_report_msg(tss_sdk_impl::UserInfo *user,
                                     int msg_type, int flag);
    static int send_cmd_monitor(tss_sdk_impl::UserInfo *user,
                                unsigned int cmd_id, bool monitor,
                                bool report);
};

}  // namespace tss_dpsdk


// ---------------------------------------------------------------------------
// DPMsgHandler::DPMsgHandler() @ 0x000ae2f0 (C1) / 0x000ae350 (C2)
//   line 15: entry
//   line 17: operator new(0x44), DPVMBaseEngine::C1(ptr), store into this->[0]
//   line 18: return (cold landing pad: operator delete(ptr); _Unwind_Resume)
// ---------------------------------------------------------------------------
tss_dpsdk::DPMsgHandler::DPMsgHandler()
{
    vm_engine_ = new DPVMBaseEngine();
}

// ---------------------------------------------------------------------------
// DPMsgHandler::~DPMsgHandler() @ 0x000ae060 (D2) / 0x000ae070 (D1)
//   line 20: entry, line 23: epilogue.  Empty body; vm_engine_ is not deleted.
// ---------------------------------------------------------------------------
tss_dpsdk::DPMsgHandler::~DPMsgHandler()
{
}

// ---------------------------------------------------------------------------
// DPMsgHandler::init() @ 0x000ae2c0
//   line 25: entry; line 28: return vm_engine_->init(); line 29: epilogue.
//   Emitted as: this->[0] -> DPVMBaseEngine::init(), result returned unchanged.
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::init()
{
    return vm_engine_->init();
}

// ---------------------------------------------------------------------------
// DPMsgHandler::reload() @ 0x000ae290
//   line 31: entry; line 34: return vm_engine_->reload(); line 35: epilogue.
//   Emitted as: this->[0] -> DPVMBaseEngine::reload(), result returned.
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::reload()
{
    return vm_engine_->reload();
}

// ---------------------------------------------------------------------------
// DPMsgHandler::is_need_decrypt(UserInfo*, int) @ 0x000ae080
//   line 63: entry
//   line 65: pos = glb_dp_config_->dp_code_info.up_cmd_list.cur_posi (+0x2956)
//   line 66: if (user != 0 && user->user_code_info_.up_cmd_list_pos_ (+0x90)
//                 >= 0)
//   line 68:     pos = that signed short
//   line 70: return is_value_exist(up_cmd_list.cmd_lists[pos], cmd_id)
//   line 71: return false
// The unsigned short position selects a packed 0x202-byte DPValueArray_128;
// the emitted address is config + 0x2950 + pos*0x202 + 0x0e for length and
// +0x10 for values[0].
// ---------------------------------------------------------------------------
bool tss_dpsdk::DPMsgHandler::is_need_decrypt(
    tss_sdk_impl::UserInfo *user, int cmd_id)
{
    unsigned short pos =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.
            up_cmd_list.cur_posi;

    if (user != 0 && user->user_code_info_.up_cmd_list_pos_ >= 0)
        pos = (unsigned short)user->user_code_info_.up_cmd_list_pos_;

    return tss_dp_comm::is_value_exist(
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.
            up_cmd_list.cmd_lists[pos],
        (unsigned int)cmd_id);
}

// ---------------------------------------------------------------------------
// DPMsgHandler::is_need_encrypt(UserInfo*, int) @ 0x000ae730
//   line 73: entry
//   line 75: pos = down_cmd_list.cur_posi (+0x3166)
//   line 76: if (user != 0 && user->user_code_info_.down_cmd_list_pos_ (+0x92)
//                 >= 0)
//   line 78:     pos = that signed short
//   line 80: return is_value_exist(down_cmd_list.cmd_lists[pos], cmd_id)
//   line 81: return false
// The emitted address is config + 0x3160 + pos*0x202 + 0x0e for length and
// +0x10 for values[0].
// ---------------------------------------------------------------------------
bool tss_dpsdk::DPMsgHandler::is_need_encrypt(
    tss_sdk_impl::UserInfo *user, int cmd_id)
{
    unsigned short pos =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.
            down_cmd_list.cur_posi;

    if (user != 0 && user->user_code_info_.down_cmd_list_pos_ >= 0)
        pos = (unsigned short)user->user_code_info_.down_cmd_list_pos_;

    return tss_dp_comm::is_value_exist(
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.
            down_cmd_list.cmd_lists[pos],
        (unsigned int)cmd_id);
}

// ---------------------------------------------------------------------------
// DPMsgHandler::anticheck_pkg(UserInfo*, int) @ 0x000ae7c0
//   line 83: entry
//   line 86: if (user == 0 || !user->is_client_inited()) return 0
//   line 91: bool res = false
//   line 92: res = is_value_exist(up_cmd_list.cmd_lists[
//                                    user->up_cmd_list_pos_], cmd_id)
//            (signed short index; the original emits movsx, i.e. no clamp)
//   line 95: if (res) { DPBillAdapter bill("recv cheat pkg", 0x1c, user);
//   line 96:            bill << cmd_id;
//   line 98:            DPMsgPacker::send_error_report_msg(user, 0x191, 1);
//   line 100:           DPMsgPacker::send_cmd_monitor(user, cmd_id, 1, 0); }
//   line 103: return res;
// The bill string at 0x21bd49 is "recv cheat pkg" (NUL-terminated); 0x1c is
// the DPBillAdapter constructor's unsigned-int argument.
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::anticheck_pkg(
    tss_sdk_impl::UserInfo *user, int cmd_id)
{
    if (user == 0 || !user->is_client_inited())
        return 0;

    bool res = false;
    res = tss_dp_comm::is_value_exist(
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.
            up_cmd_list.cmd_lists[user->user_code_info_.up_cmd_list_pos_],
        (unsigned int)cmd_id);

    if (res) {
        tss_dpsdk::DPBillAdapter bill("recv cheat pkg", 0x1c, user);
        bill << (long long)cmd_id;
        tss_dpsdk::DPMsgPacker::send_error_report_msg(user, 0x191, 1);
        tss_dpsdk::DPMsgPacker::send_cmd_monitor(user, cmd_id, true, false);
    }

    return res;
}

// ---------------------------------------------------------------------------
// ORIG CU dp_msg_handler.cpp static-initialization block.
//
// This slice is the single owner of the block for the ORIG CU
// dp_msg_handler.cpp (dp_msg_handler_core/buffer/send/recv_agent.cpp all
// cover parts of it; a second block would duplicate the objects).
//
// Decoded ORIG order (audit/report/init_brief.md, 26 calls, target size 996):
//   ios_base::Init::Init(__ioinit) + atexit
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (0x238d..0x239c) + atexit
//   __cxa_guard_acquire / TssSdkCommSvc::TssSdkCommSvc() /
//     __cxa_guard_release + atexit; guard_abort + _Unwind_Resume pad
// TEST_DATA_BUF (ORIG .rodata 0x0021c3da, const char[10] = "0123456789") is
// the only module-specific object of this CU.
// ---------------------------------------------------------------------------
#include "src/static_init_block.h"

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ORIG .rodata 0x0021c3da, 10 bytes: "0123456789" (no terminator).
static const char TEST_DATA_BUF[10] __attribute__((used)) = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

// ORIG .bss 0x0029d800 (weak): Singleton<TssSdkCommSvc>::object_creator.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
