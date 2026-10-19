// Reconstructed from libtss_sdk.so.1.0.394 (SHA-256
// 8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7).
//
//   DPInterface::add_user(UserInfo*)   0x000bfa80 .. 0x000bfc54
//   DPInterface::del_user(UserInfo*)   0x000bfc60 .. 0x000bfdfe
//   DPInterface::check_timer(time_t)   0x000c0f00 .. 0x000c0f82
//
// The bodies below follow the emitted control flow.  Helper classes that are
// only forward declared in the available headers are modelled locally with
// the exact DWARF offsets/vtable order; every such model is marked
// "INTEGRATION BOUNDARY" and must be replaced by the real SDK header when the
// reconstruction is wired into the full build.
//
// The three DPInterface methods themselves are real definitions and emit
//   _ZN11DPInterface8add_userEPN12tss_sdk_impl8UserInfoE
//   _ZN11DPInterface8del_userEPN12tss_sdk_impl8UserInfoE
//   _ZN11DPInterface11check_timerEl
#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/formmog/tss_sdk/layout/dp_hack_mgr.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

namespace tss_dp_comm {

// Inline helper from the SDK config header (DWARF file/line 449:32, inlined
// into add_user at 0x000bfae0, call line 110).
template <typename T>
bool is_value_exist(const T &values, unsigned int value) {
    for (unsigned short i = 0; i < values.length; ++i) {
        if (value < values.values[i])
            break;
        if (value == values.values[i])
            return true;
    }
    return false;
}

}  // namespace tss_dp_comm


// ---------------------------------------------------------------------------
// DPInterface::add_user(tss_sdk_impl::UserInfo*)  @ 0x000bfa80
//
// 0xbfaa0  user == 0                         -> return -1
// 0xbfab6  is_inited_ == 0                   -> return -1
// 0xbfabc  !glb_dp_config_->dp_base_conf.is_use_dp -> return -1
// 0xbfac9  user->user_code_info_.client_state_ = ECS_NOT_INIT (1)
// 0xbfae0  inlined is_value_exist(uin_monitor_list, user->base_info_.uin_)
// 0xbfc36    true -> user->set_trace_flag() (inlined: flags_ |= 1)
// 0xbfb2d  inlined DPHackMgr::on_login(user): iterate hackers_[E_LOGIN],
//          call IDPHacker::on_login, break on non-zero
// 0xbfb61  tss_dpsdk::DPBillAdapter bill("add user", 3, user)
// 0xbfb85..0xbfc24  bill chain (call line 124)
// 0xbfc2a  ~DPBillAdapter, return 0
// ---------------------------------------------------------------------------
int DPInterface::add_user(tss_sdk_impl::UserInfo *user) {
    if (user == 0)
        return -1;
    if (is_inited_ == 0)
        return -1;
    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return -1;

    user->user_code_info_.client_state_ = tss_sdk_impl::ECS_NOT_INIT;

    if (tss_dp_comm::is_value_exist(
            tss_dp_comm::DP_CONFIG::glb_dp_config_->uin_monitor_list,
            user->base_info_.uin_)) {
        // UserInfo::set_trace_flag() is an inline method in user_info.h; its
        // observed expansion at 0x000bfc36 is flags_ |= 1 (call line 112).
        user->base_info_.flags_ |= 1;
    }

    // DPHackMgr::on_login(user), call line 115 (bodies are out of line in
    // dp_hack_mgr_agent.cpp until the canonical header inlines them).
    hack_manager_->on_login(user);

    // user_info_recon.h types role_name_ as unsigned char[64], while DWARF
    // (type id 0xe6499) and the bill call site say char[64]; bridge the two
    // representations without changing the bytes passed to the bill.
    const char (&role_name)[64] =
        reinterpret_cast<const char (&)[64]>(user->base_info_.role_name_);

    tss_dpsdk::DPBillAdapter bill("add user", 3, user);
    bill << user->base_info_.game_usr_id_
         << user->base_info_.client_version_
         << user->base_info_.game_id_
         << user->base_info_.room_id_
         << user->base_info_.client_ip_
         << user->base_info_.server_ip_
         << role_name;
    return 0;
}

// ---------------------------------------------------------------------------
// DPInterface::del_user(tss_sdk_impl::UserInfo*)  @ 0x000bfc60
//
// 0xbfc80  user == 0                         -> return -1
// 0xbfc96  is_inited_ == 0                   -> return -1
// 0xbfc9c  !glb_dp_config_->dp_base_conf.is_use_dp -> return -1
// 0xbfca9  inlined DPHackMgr::on_logout(user): iterate hackers_[E_LOGOUT],
//          call IDPHacker::on_logout, break on non-zero (call line 135)
// 0xbfcdd  tss_dpsdk::DPBillAdapter bill("delete user", 5, user)
// 0xbfd01..0xbfdd7  bill chain (call line 145)
// 0xbfddd  ~DPBillAdapter, return 0
// ---------------------------------------------------------------------------
int DPInterface::del_user(tss_sdk_impl::UserInfo *user) {
    if (user == 0)
        return -1;
    if (is_inited_ == 0)
        return -1;
    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return -1;

    // DPHackMgr::on_logout(user), call line 135.
    hack_manager_->on_logout(user);

    tss_dpsdk::DPBillAdapter bill("delete user", 5, user);
    bill << user->user_stat_.internal_msg_send_count_
         << user->user_stat_.internal_fix_encrypt_count_
         << user->user_stat_.internal_fix_decrypt_count_
         << user->user_stat_.internal_dynamic_decrypt_count_
         << user->user_stat_.fix_encrypt_count_
         << user->user_stat_.dynamic_encrypt_count_
         << user->user_stat_.game_fix_decrypt_count_
         << user->user_stat_.game_dynamic_decrypt_count_;
    return 0;
}

// ---------------------------------------------------------------------------
// DPInterface::check_timer(time_t)  @ 0x000c0f00
//
// 0xc0f17  is_inited_ == 0                   -> return 0
// 0xc0f2a  check_and_reload_config()
// 0xc0f2f  !glb_dp_config_->dp_base_conf.is_use_dp -> return 0
// 0xc0f3c  inlined DPHackMgr::proc(now): iterate hackers_[E_PROC],
//          call IDPHacker::proc, break on non-zero (call line 164)
// 0xc0f6d  tss_dpsdk::DPMonitorReporter::check_timer(now)
// 0xc0f78  return 0 (the reporter result is discarded)
// ---------------------------------------------------------------------------
int DPInterface::check_timer(time_t now) {
    if (is_inited_ == 0)
        return 0;

    check_and_reload_config();

    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return 0;

    // DPHackMgr::proc(now), call line 164.
    hack_manager_->proc(now);

    tss_dpsdk::DPMonitorReporter::check_timer(now);
    return 0;
}
