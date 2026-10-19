// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
//   DPInterface::recv_buffer_for_encrypt(UserInfo*, unsigned char*,
//       unsigned int, unsigned char*, unsigned int&)
//       0x000bf520 - 0x000bf7ea  (next symbol 0x000bf7f0)
//   DPInterface::recv_buffer_for_encrypt(UserInfo*, unsigned int,
//       unsigned char*, unsigned int, unsigned char*, unsigned int&)
//       0x000bf7f0 - 0x000bfa7e  (next symbol 0x000bfa80)
//   DPInterface::recv_buffer_for_decrypt(UserInfo*, unsigned char*,
//       unsigned int, unsigned char*, unsigned int&)
//       0x000bfe00 - 0x000c04ea  (next symbol 0x000c04f0)
//
// The three DPInterface definitions are the only symbols this file is meant to
// provide.  All helper classes below are local, minimal declarations: the
// shared dpinterface.h only has placeholder declarations for DPMsgHandler and
// DPHackMgr, and dpinterface_deps.h does not declare the members used here.
//
// Recovered dependencies (nm -C of the .so; all out-of-line except where noted):
//   ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)
//   tss_dp_comm::get_cmd_id(DPGameCmdStruct const&, unsigned char*,
//                           unsigned short, unsigned int&)
//   tss_dpsdk::DPHackMgr::hack(UserInfo*, int, unsigned int,
//                              unsigned char*&, unsigned int&)
//   tss_dpsdk::DPMsgHandler::recv_buffer_for_encrypt(UserInfo*, unsigned char*,
//                              unsigned int, unsigned char*, unsigned int&)
//   tss_dpsdk::DPMsgHandler::recv_buffer_for_decrypt(UserInfo*, unsigned char*,
//                              unsigned int, unsigned char*, unsigned int&,
//                              int, int)
//   tss_dpsdk::DPMsgHandler::OnRecvPkgFromClient(UserInfo*, unsigned char*,
//                              unsigned int)
//   tss_dpsdk::DPBillAdapter::dump(char const*, unsigned int,
//                                  unsigned char const*, unsigned int)
//   tss_dpsdk::DPMonitorReporter::report_data(unsigned int, unsigned int)
//   sec_proto::DPInterDataHead::unpack(char const*, unsigned int,
//                                      unsigned int*, unsigned int)
//   tss_dp_comm::DP_CONFIG::glb_dp_config_ (exported OBJECT, pointer to DPConfig)
//
// tss_dpsdk::DPHackMgr::on_special_control is inline-only in the original
// (no out-of-line symbol); the loop below is rebuilt from its inlined copy at
// 0x000c025e..0x000c02e0 and the DWARF abstract instance (dp_hack_mgr.h:56).

#include "src/closing/dp_config.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

#include <stddef.h>

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/layout/dp_hack_mgr.h"

// ---------------------------------------------------------------------------
// sec_proto::DPInterDataHead
//
// The full packet hierarchy is declared once in
// src/origsrc/formmog_proto_tss_sdk_client.h.  That header models the TDR
// write/read buffers itself and cannot be co-included with the configuration
// header (src/closing/dp_config.h -> tsf4g_tdr_compat_cfg.h) in one TU, so
// this TU keeps only the 5-byte header and the one entry point it calls.
// ---------------------------------------------------------------------------
namespace sec_proto {

struct __attribute__((packed)) DPInterDataHead {
    unsigned short packet_size_;    // +0x00
    unsigned short protocol_type_;  // +0x02
    unsigned char data_type_;       // +0x04

    int unpack(const char *buf, unsigned int buf_len, unsigned int *used,
               unsigned int cut_ver);
};

}  // namespace sec_proto


// ---------------------------------------------------------------------------
// DPInterface buffer methods
// ---------------------------------------------------------------------------

int DPInterface::recv_buffer_for_encrypt(tss_sdk_impl::UserInfo *user,
                                         unsigned char *buf,
                                         unsigned int buf_len,
                                         unsigned char *out,
                                         unsigned int &out_len) {
    if (!is_inited_)
        return -1;
    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return -1;

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO : RS_DEBUG,
        "[%u][%s], buf=%p, buf_len=%u, out=%p, out_len=%u",
        user != 0 ? user->base_info_.uin_ : 0u, __func__, buf, buf_len, out,
        out_len);

    if (user != 0 && (user->base_info_.flags_ & 1) != 0)
        tss_dpsdk::DPBillAdapter::dump(
            "recv_buffer_for_encrypt before encrypt", user->base_info_.uin_,
            buf, buf_len);

    if (user != 0) {
        unsigned int cmd_id = 0;
        if (tss_dp_comm::get_cmd_id(
                tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_down_cmd_hack, buf,
                static_cast<unsigned short>(buf_len), cmd_id) == 0) {
            tss_dpsdk::DPHackMgr *const hack_manager =
                reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_);
            // hack_type 2 (encrypt/down); buf and buf_len may be replaced.
            if (hack_manager->hack(user, 2, cmd_id, buf, buf_len) == 1)
                return -1;
        }
    }

    tss_dpsdk::DPMsgHandler *const msg_handler =
        reinterpret_cast<tss_dpsdk::DPMsgHandler *>(msg_handler_);
    const int res =
        msg_handler->recv_buffer_for_encrypt(user, buf, buf_len, out, out_len);

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO : RS_DEBUG,
        "[%u][%s], res=%d, out_len=%u",
        user != 0 ? user->base_info_.uin_ : 0u, __func__, res, out_len);

    if (res == 0) {
        if (user != 0 && (user->base_info_.flags_ & 1) != 0)
            tss_dpsdk::DPBillAdapter::dump(
                "recv_buffer_for_encrypt after encrypt", user->base_info_.uin_,
                out, out_len);
        tss_dpsdk::DPMonitorReporter::report_data(0x3fe, 1);
    } else {
        tss_dpsdk::DPMonitorReporter::report_data(0x3ff, 1);
    }
    return res;
}

int DPInterface::recv_buffer_for_encrypt(tss_sdk_impl::UserInfo *user,
                                         unsigned int cmd_id,
                                         unsigned char *buf,
                                         unsigned int buf_len,
                                         unsigned char *out,
                                         unsigned int &out_len) {
    if (!is_inited_)
        return -1;
    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return -1;

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO : RS_DEBUG,
        "[%u][%s], cmdid=%u, buf=%p, buf_len=%u, out=%p, out_len=%u",
        user != 0 ? user->base_info_.uin_ : 0u, __func__, cmd_id, buf, buf_len,
        out, out_len);

    if (user != 0 && (user->base_info_.flags_ & 1) != 0)
        tss_dpsdk::DPBillAdapter::dump(
            "recv_buffer_for_encrypt before encrypt", user->base_info_.uin_,
            buf, buf_len);

    if (user != 0) {
        tss_dpsdk::DPHackMgr *const hack_manager =
            reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_);
        // hack_type 2 (encrypt/down); buf and buf_len may be replaced.
        if (hack_manager->hack(user, 2, cmd_id, buf, buf_len) == 1)
            return -1;
    }

    if (!is_need_encrypt(user, static_cast<int>(cmd_id)))
        return -1;

    tss_dpsdk::DPMsgHandler *const msg_handler =
        reinterpret_cast<tss_dpsdk::DPMsgHandler *>(msg_handler_);
    const int res =
        msg_handler->recv_buffer_for_encrypt(user, buf, buf_len, out, out_len);

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO : RS_DEBUG,
        "[%u][%s], res=%d, out_len=%u",
        user != 0 ? user->base_info_.uin_ : 0u, __func__, res, out_len);

    if (res == 0) {
        if (user != 0 && (user->base_info_.flags_ & 1) != 0)
            tss_dpsdk::DPBillAdapter::dump(
                "recv_buffer_for_encrypt after encrypt", user->base_info_.uin_,
                out, out_len);
        tss_dpsdk::DPMonitorReporter::report_data(0x3fe, 1);
    } else {
        tss_dpsdk::DPMonitorReporter::report_data(0x3ff, 1);
    }
    return res;
}

int DPInterface::recv_buffer_for_decrypt(tss_sdk_impl::UserInfo *user,
                                         unsigned char *buf,
                                         unsigned int buf_len,
                                         unsigned char *out,
                                         unsigned int &out_len) {
    // Unlike the encrypt overloads, there is no is_use_dp check here.
    if (!is_inited_)
        return -1;

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO : RS_DEBUG,
        "[%u][%s], buf=%p, buf_len=%u, out=%p, out_len=%u",
        user != 0 ? user->base_info_.uin_ : 0u, __func__, buf, buf_len, out,
        out_len);

    if (user != 0 && (user->base_info_.flags_ & 1) != 0)
        tss_dpsdk::DPBillAdapter::dump(
            "recv_buffer_for_decrypt before decrypt", user->base_info_.uin_,
            buf, buf_len);

    size_t used = 0;
    sec_proto::DPInterDataHead head;
    int res = head.unpack(reinterpret_cast<const char *>(buf), buf_len, &used, 0);
    if (res != 0 || used >= buf_len) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], DPInterDataHead unpack fail, res=%d, used=%d, "
            "buf_len=%d",
            user != 0 ? user->base_info_.uin_ : 0u, __func__, res, used,
            buf_len);
        return -1;
    }

    unsigned char *payload = buf + used;
    unsigned int payload_len =
        buf_len - static_cast<unsigned short>(used);

    // protocol_type_ values compared by the original.  The enum names were
    // not present in the DWARF; only the values are certain.
    if (head.protocol_type_ == 0x1a) {  // special control
        if (user == 0)
            return 1;

        tss_dpsdk::DPHackMgr *const hack_manager =
            reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_);
        const int sc_res = hack_manager->on_special_control(
            user, payload, payload_len, out, out_len);

        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1) != 0 ? RS_INFO : RS_DEBUG,
            "[%u][%s], on_special_control res=%d, out_len=%d",
            user->base_info_.uin_, __func__, sc_res, out_len);

        if (sc_res == 1) {
            if ((user->base_info_.flags_ & 1) != 0)
                tss_dpsdk::DPBillAdapter::dump(
                    "on_special_control", user->base_info_.uin_, out, out_len);
            return 0;
        }
        return 1;
    }

    int is_internal_pkg = 0;
    unsigned char internal_buf[65535];

    if (head.protocol_type_ == 0x12) {  // internal package
        if (user == 0)
            return 1;

        // The same head object and used variable are reused.
        res = head.unpack(reinterpret_cast<const char *>(payload), payload_len,
                          &used, 0);
        if (res != 0 || used >= payload_len) {
            ZEN_Trace_LogMsg::debug_output(
                RS_ERROR,
                "[%u][%s], DPInterDataHead unpack fail(2), res=%d, used=%d, "
                "buf_len=%d",
                user->base_info_.uin_, __func__, res, used, payload_len);
            return -1;
        }
        payload += used;
        payload_len -= static_cast<unsigned short>(used);
        out = internal_buf;
        out_len = 0xffff;
        is_internal_pkg = 1;
    }

    if (user != 0) {
        ZEN_Trace_LogMsg::debug_output(
            (user->base_info_.flags_ & 1) != 0 ? RS_INFO : RS_DEBUG,
            "[%u][%s], internal=%d, pkg_type=%d", user->base_info_.uin_,
            __func__, is_internal_pkg,
            static_cast<int>(head.protocol_type_));
    } else {
        ZEN_Trace_LogMsg::debug_output(
            RS_DEBUG, "[%u][%s], internal=%d, pkg_type=%d", 0u, __func__, 0,
            static_cast<int>(head.protocol_type_));
    }

    tss_dpsdk::DPMsgHandler *const msg_handler =
        reinterpret_cast<tss_dpsdk::DPMsgHandler *>(msg_handler_);
    res = msg_handler->recv_buffer_for_decrypt(
        user, payload, payload_len, out, out_len,
        static_cast<int>(head.protocol_type_), is_internal_pkg);

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO : RS_DEBUG,
        "[%u][%s], res=%d, out_len=%d",
        user != 0 ? user->base_info_.uin_ : 0u, __func__, res, out_len);

    if (res != 0) {
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], decrypt fail res=%d, out_len=%d, internal=%d, "
            "pkg_type=%d",
            user != 0 ? user->base_info_.uin_ : 0u, __func__, res, out_len,
            is_internal_pkg, static_cast<int>(head.protocol_type_));
        tss_dpsdk::DPMonitorReporter::report_data(0x401, 1);
        return res;
    }

    if (user != 0 && (user->base_info_.flags_ & 1) != 0)
        tss_dpsdk::DPBillAdapter::dump(
            is_internal_pkg != 0
                ? "recv_buffer_for_decrypt internal pkg"
                : "recv_buffer_for_decrypt after decrypt",
            user->base_info_.uin_, out, out_len);

    tss_dpsdk::DPMonitorReporter::report_data(0x400, 1);

    if (is_internal_pkg != 0) {
        if (user != 0 &&
            user->user_code_info_.client_state_ != tss_sdk_impl::ECS_NOT_READY)
            msg_handler->OnRecvPkgFromClient(user, out, out_len);
        out_len = 0;
        return 1;
    }

    if (user == 0)
        return 0;
    if (!tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_base_conf.is_use_dp)
        return 0;

    unsigned int cmd_id = 0;
    if (tss_dp_comm::get_cmd_id(
            tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_up_cmd_hack, out,
            static_cast<unsigned short>(out_len), cmd_id) != 0)
        return 0;

    tss_dpsdk::DPHackMgr *const hack_manager =
        reinterpret_cast<tss_dpsdk::DPHackMgr *>(hack_manager_);
    // hack_type 1 (decrypt/up).
    return hack_manager->hack(user, 1, cmd_id, out, out_len) == 1 ? 1 : 0;
}
