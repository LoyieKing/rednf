// Reconstruction of tss_dpsdk::DPMsgHandler client-message and timer paths
// from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, -O2 -fPIC).
//
// Functions covered (exact ORIG symbols):
//   _ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto19DPClientDecryptInfoE   @0x000af180
//   _ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto14DPClientLogPtcE        @0x000af2d0
//   _ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto21DPProtocolRequestCodeE @0x000af410
//   _ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto15DPClientStatPtcE       @0x000af5b0
//   _ZN9tss_dpsdk12DPMsgHandler11check_timerEPN12tss_sdk_impl8UserInfoEl                                           @0x000afe70
//   _ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto19DPClientInitDataPtcE   @0x000b0b50
//
// This TU intentionally carries only partial declarations of the SDK types:
// the class layouts/field offsets are the DWARF/objdump-verified subset used by
// these six functions.  Everything else (billing, packing, send methods and the
// code-manager container methods) is declared extern or modelled as an inline
// header-style body so the original out-of-line call targets are preserved.
//
// INTEGRATION BOUNDARY: replace these partial declarations with the real SDK
// headers; no shared header was modified here.

#include <stdlib.h>
#include <string.h>
#include <time.h>

// Canonical tag-394 layouts: tss_dpsdk::DPBillAdapter / CommBillRecord /
// DPBillRecorder (layout/dp_bill.h -> dp_bill_recorder.h),
// tss_dpsdk::DPMsgHandler (layout/dp_msg_handler.h),
// tss_dpsdk::DPVMBaseEngine (layout/dp_vmbase_engine.h) and
// tss_sdk_impl::UserInfo (layout/user_info.h).  The local models this TU
// used to carry are gone.
#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/origsrc/formmog_proto_tss_sdk_client.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/layout/dp_vmbase_engine.h"
#include "src/formmog/tss_sdk/layout/user_info.h"

// ---------------------------------------------------------------------------
// sec_proto wire packets (offsets/sizes from DWARF; fields used by this TU).
// DPClientStatPtc and DPProtocolRequestCode contain unaligned 32-bit fields,
// hence pack(1).
// ---------------------------------------------------------------------------


// tss_sdk_impl::UserInfo (400 bytes) comes from layout/user_info.h; the
// partial model with the named BaseInfo/UserCodeInfo/UserTimer/UserStat/
// HackData substructures is gone.

// ---------------------------------------------------------------------------
// tss_dp_comm::DPConfig (only the offsets used by these functions).  The
// discarded regions are represented by byte arrays with their DWARF sizes so
// is_use_client_version (+0x02), the three proto switches (+0x0b..+0x0d) and
// DPTimeConfig (+0x138) keep their absolute offsets.
// ---------------------------------------------------------------------------
namespace tss_dp_comm {

struct DPConfigBase {
    unsigned char is_use_dp;              // +0
    unsigned char is_restore_usr_space;   // +1
    unsigned char is_use_client_version;  // +2
    unsigned char disable_anticheat_pkg;  // +3
    unsigned char use_punish_notify;      // +4
    unsigned char dp_record_using_time;   // +5
};

struct DPProtoConfig {
    unsigned short dp_fragmentation_size;    // +0
    unsigned short dp_max_encrypt_size;      // +2
    unsigned char use_sequence_check;        // +4
    unsigned char proto_check_switch;        // +5
    unsigned char seq_check_switch;          // +6
    unsigned char raw_check_switch;          // +7
    unsigned char dp_decrypt_fail_protect;   // +8
    unsigned char explicit_use_dynamic_encrypt; // +9
};

struct DPLogConf {
    unsigned char raw_[296];
};

struct DPTimeConfig {
    unsigned int max_init_pkg_timeout;      // +0
    unsigned int max_static_pkg_timeout;    // +4
    unsigned int max_confirm_pkg_timeout;   // +8
    unsigned int max_send_test_pkg_timeout; // +12
    unsigned int check_time;                // +16
    unsigned int send_bin_file_gap;         // +20
    unsigned int send_bin_code_gap;         // +24
};

struct DPCodeInfoConf {
    unsigned char raw_[14884];
};

struct DPValueArray128 {
    unsigned char raw_[514];
};

struct DPGameCmdStruct {
    unsigned short raw_;
};

struct DPCmdHackConf {
    unsigned char raw_[7100];
};

struct __attribute__((packed)) DPConfig {
    DPConfigBase dp_base_conf;          // +0
    DPProtoConfig dp_proto_conf;        // +6
    DPLogConf dp_log_conf;              // +16
    DPTimeConfig dp_time_confg;         // +312
    DPCodeInfoConf dp_code_info;        // +340
    DPValueArray128 uin_monitor_list;   // +15224
    DPGameCmdStruct dp_up_cmd_hack;     // +15738
    DPGameCmdStruct dp_down_cmd_hack;   // +15740
    DPCmdHackConf cmd_hack;             // +15742
};

struct DP_CONFIG {
    static DPConfig *glb_dp_config_;
};

} // namespace tss_dp_comm

// ---------------------------------------------------------------------------
// tss_dpsdk code tables.  Layouts are DWARF verified:
//   CodeItem<N>   = 2 + 2 + N
//   CodeItems<N>  = 2 + 100 * CodeItem<N>
//   CodeInfo      = CodeItems<2000> + CodeItems<2000> + CodeItems<20000>
//   DpCodeInfo    = ushort + pad2 + uint[8] + uchar[16] + CodeInfo*2
//   DpCodeInfos   = uint + ushort + pad2 + DpCodeInfo[2]
// The selection helpers below are the inlined header bodies attributed by the
// ORIG line table to dp_code_mgr.h/dp_vmbase_engine.h.  They are inline members
// of the real classes; -fno-implement-inlines keeps them from becoming new
// out-of-line symbols.
// CommBillRecord / tss_dpsdk::DPBillRecorder / DPBillAdapter come from
// layout/dp_bill.h (-> layout/dp_bill_recorder.h, 5984/5988 bytes); the
// local 1-byte stand-ins and the reduced streaming-operator set are gone.

namespace tss_dpsdk {

template <int N>
struct CodeItem {
    unsigned short weight_;    // +0
    unsigned short code_len_;  // +2
    unsigned char code_buf_[N]; // +4
};

template <int N>
struct CodeItems {
    unsigned short total_weight_; // +0
    CodeItem<N> items_[100];      // +2

    int select_code_id(unsigned int r) {
        int i = 0;
        CodeItem<N> *item = items_;
        while (i < 100) {
            unsigned int weight = item->weight_;
            if (r < weight)
                return i;
            r -= weight;
            ++i;
            ++item;
        }
        return 100;
    }

    unsigned char *get_code_buf(unsigned short id, unsigned short &len) {
        if ((unsigned short)(id - 1) > 0x62) {
            len = 0;
            return 0;
        }
        CodeItem<N> *item = &items_[id];
        len = item->code_len_;
        return item->code_buf_;
    }
};

struct CodeInfo {
    CodeItems<2000> cryptdata_code_; // +0
    CodeItems<2000> crypt_code_;     // +200402
    CodeItems<20000> bin_file_;      // +400804

    int select_bin_file_id() {
        unsigned int r = (unsigned int)::rand();
        r %= (unsigned int)bin_file_.total_weight_;
        return bin_file_.select_code_id(r);
    }

    int select_bin_code_id(unsigned short code_id) {
        unsigned int r = (unsigned int)::rand();
        unsigned int cryptdata_total = (unsigned int)cryptdata_code_.total_weight_;
        unsigned int crypt_total = (unsigned int)crypt_code_.total_weight_;
        r %= cryptdata_total + crypt_total;
        if (r < cryptdata_total && code_id != 0)
            return (int)code_id;
        r %= crypt_total;
        return crypt_code_.select_code_id(r) + 100;
    }

    unsigned char *get_bin_code_buf(unsigned short code_id,
                                    unsigned short &len) {
        if (code_id <= 99)
            return cryptdata_code_.get_code_buf(code_id, len);
        return crypt_code_.get_code_buf((unsigned short)(code_id - 100), len);
    }
};

struct DpCodeInfo {
    unsigned short version_count_;        // +0
    unsigned int version_support_list_[8]; // +4
    unsigned char version_hash_[16];      // +36
    CodeInfo client_code_;                // +52
    CodeInfo server_code_;                // +2401258

    bool is_support(unsigned int version) {
        if (version_count_ == 0)
            return false;
        if (version_support_list_[0] == version ||
            version_support_list_[0] == 0)
            return true;
        for (int i = 1; i < (int)version_count_; ++i) {
            if (version_support_list_[i] == version ||
                version_support_list_[i] == 0)
                return true;
        }
        return false;
    }
};

struct DpCodeInfos {
    unsigned int code_update_time_; // +0
    unsigned short code_count_;     // +4
    DpCodeInfo code_infos_[2];      // +8
};

class DPCodeMgr {
public:
    DpCodeInfos *code_infos_; // +0

    int select_server_bin_file_id(unsigned int version) {
        for (int i = 0; i < (int)code_infos_->code_count_; ++i) {
            DpCodeInfo *info = &code_infos_->code_infos_[i];
            if (info->is_support(version))
                return info->server_code_.select_bin_file_id();
        }
        return 0;
    }

    int select_client_code_id(unsigned int version, unsigned short code_id) {
        for (int i = 0; i < (int)code_infos_->code_count_; ++i) {
            DpCodeInfo *info = &code_infos_->code_infos_[i];
            if (info->is_support(version))
                return info->client_code_.select_bin_code_id(code_id);
        }
        return 0;
    }

    int select_server_code_id(unsigned int version, unsigned short code_id) {
        for (int i = 0; i < (int)code_infos_->code_count_; ++i) {
            DpCodeInfo *info = &code_infos_->code_infos_[i];
            if (info->is_support(version))
                return info->server_code_.select_bin_code_id(code_id);
        }
        return 0;
    }

    bool check_version_hash(unsigned int version, unsigned char *hash) {
        for (int i = 0; i < (int)code_infos_->code_count_; ++i) {
            DpCodeInfo *info = &code_infos_->code_infos_[i];
            if (info->is_support(version))
                return ::memcmp(info->version_hash_, hash, 16) == 0;
        }
        return false;
    }

    bool check_client_code(unsigned int version, unsigned short code_id) {
        for (int i = 0; i < (int)code_infos_->code_count_; ++i) {
            DpCodeInfo *info = &code_infos_->code_infos_[i];
            if (info->is_support(version)) {
                unsigned short len = 0;
                return info->client_code_.get_bin_code_buf(code_id, len) == 0;
            }
        }
        return true;
    }

    bool check_server_code(unsigned int version, unsigned short code_id) {
        for (int i = 0; i < (int)code_infos_->code_count_; ++i) {
            DpCodeInfo *info = &code_infos_->code_infos_[i];
            if (info->is_support(version)) {
                unsigned short len = 0;
                return info->server_code_.get_bin_code_buf(code_id, len) == 0;
            }
        }
        return true;
    }
};
// DPVMBaseEngine's code-selection members.  ORIG dp_vmbase_engine.h declares
// them inline and the target has no out-of-line symbol for any of them
// (DWARF subprograms, DW_AT_declaration only):
//   select_server_bin_file_id(UserInfo*), select_client_code_id(UserInfo*),
//   select_server_code_id(UserInfo*), check_server_code(UserInfo*, u16),
//   check_client_code(UserInfo*, u16), check_version_hash(UserInfo*, u8*).
// The bodies are the DPCodeMgr code-table model above, reached through the
// class's code_manager_ (+0x08) exactly as ORIG does.
int DPVMBaseEngine::select_server_bin_file_id(
        tss_sdk_impl::UserInfo *user) {
    DPCodeMgr *code_manager = static_cast<DPCodeMgr *>(code_manager_);
    return code_manager->select_server_bin_file_id(
        user->base_info_.client_version_);
}

int DPVMBaseEngine::select_client_code_id(tss_sdk_impl::UserInfo *user) {
    DPCodeMgr *code_manager = static_cast<DPCodeMgr *>(code_manager_);
    return code_manager->select_client_code_id(
        user->base_info_.client_version_,
        user->user_code_info_.bin_file_code_);
}

int DPVMBaseEngine::select_server_code_id(tss_sdk_impl::UserInfo *user) {
    DPCodeMgr *code_manager = static_cast<DPCodeMgr *>(code_manager_);
    return code_manager->select_server_code_id(
        user->base_info_.client_version_,
        user->user_code_info_.bin_file_code_);
}

bool DPVMBaseEngine::check_version_hash(tss_sdk_impl::UserInfo *user,
                                        unsigned char *hash) {
    DPCodeMgr *code_manager = static_cast<DPCodeMgr *>(code_manager_);
    return code_manager->check_version_hash(
        user->base_info_.client_version_, hash);
}

bool DPVMBaseEngine::check_client_code(tss_sdk_impl::UserInfo *user,
                                       unsigned short code_id) {
    DPCodeMgr *code_manager = static_cast<DPCodeMgr *>(code_manager_);
    return code_manager->check_client_code(
        user->base_info_.client_version_, code_id);
}

bool DPVMBaseEngine::check_server_code(tss_sdk_impl::UserInfo *user,
                                       unsigned short code_id) {
    DPCodeMgr *code_manager = static_cast<DPCodeMgr *>(code_manager_);
    return code_manager->check_server_code(
        user->base_info_.client_version_, code_id);
}



class DPMsgPacker {
public:
    static int send_error_report_msg(tss_sdk_impl::UserInfo *user,
                                     int error_id, int value);
};



} // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// External helpers used by the functions below.
// ---------------------------------------------------------------------------
enum ZEN_LOG_PRIORITY {
    RS_TRACE = 1,
    RS_DEBUG = 2,
    RS_INFO = 3,
    RS_ERROR = 4,
    RS_ALERT = 5,
    RS_FATAL = 6
};

class ZEN_Trace_LogMsg {
public:
    static void debug_output(ZEN_LOG_PRIORITY priority,
                             const char *fmt, ...);
};

unsigned int Crc32(unsigned char *buf, unsigned int len);

// ---------------------------------------------------------------------------
// 0x000af180 .. 0x000af2c8: DPClientDecryptInfo (client confirmation).
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::OnRecvMsgFromClient(
        tss_sdk_impl::UserInfo *user,
        sec_proto::DPClientDecryptInfo &cmd) {
    DPBillAdapter bill("user confirm", 0x14, user);
    bill << user->user_code_info_.server_new_code_
         << user->user_code_info_.server_cur_code_
         << user->user_code_info_.client_code_[0]
         << user->user_code_info_.client_code_[1]
         << cmd.decrypt_id_
         << cmd.encrypt_id_
         << user->user_code_info_.client_state_;

    if (cmd.decrypt_id_ != (unsigned int)user->user_code_info_.server_new_code_) {
        DPMsgPacker::send_error_report_msg(user, 0x19d, 1);
        return 0;
    }

    user->user_code_info_.server_cur_code_ =
        (unsigned short)cmd.decrypt_id_;
    user->user_code_info_.client_state_ =
        tss_sdk_impl::ECS_INIT_CONFIRM;
    return 0;
}

// ---------------------------------------------------------------------------
// 0x000af2d0 .. 0x000af407: DPClientLogPtc (client-side error report).
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::OnRecvMsgFromClient(
        tss_sdk_impl::UserInfo *user,
        sec_proto::DPClientLogPtc &cmd) {
    const char *msg;
    if (cmd.is_encrypt_ == 0) {
        if (tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_proto_conf.dp_decrypt_fail_protect != 0 &&
            user->user_code_info_.client_state_ >
                tss_sdk_impl::ECS_INIT_MSG2) {
            user->user_code_info_.client_state_ =
                tss_sdk_impl::ECS_INIT_FAILED;
        }
        msg = "client decrypt fail";
    } else {
        msg = "client encrypt fail";
    }

    DPBillAdapter bill(msg, 0x22, user);
    bill << user->user_code_info_.client_state_
         << cmd.error_id_
         << cmd.is_encrypt_
         << cmd.error_msg_len_
         << (const char *)cmd.error_msg_;

    DPMsgPacker::send_error_report_msg(user, 0x19b, 1);
    return 0;
}

// ---------------------------------------------------------------------------
// 0x000af410 .. 0x000af5ad: DPProtocolRequestCode (client code request).
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::OnRecvMsgFromClient(
        tss_sdk_impl::UserInfo *user,
        sec_proto::DPProtocolRequestCode &cmd) {
    ++user->user_code_info_.request_code_count_;
    if (user->user_code_info_.request_code_count_ > 4)
        return -1;

    {
        DPBillAdapter bill("user request code", 0x1b, user);
        bill << cmd.encrypt_code_id_
             << cmd.decrypt_code_id_
             << cmd.bin_file_id_
             << user->user_code_info_.client_code_[0]
             << user->user_code_info_.server_cur_code_
             << user->user_code_info_.bin_file_code_
             << user->user_code_info_.request_code_count_;
    }

    if (user->user_code_info_.bin_file_code_ != cmd.bin_file_id_)
        SendBinFile(user);
    if (cmd.encrypt_code_id_ !=
        (unsigned int)user->user_code_info_.client_code_[0])
        SendClientCode(user);
    if (cmd.decrypt_code_id_ !=
        (unsigned int)user->user_code_info_.server_cur_code_)
        SendServerCode(user);

    return 0;
}

// ---------------------------------------------------------------------------
// 0x000af5b0 .. 0x000afe63: DPClientStatPtc (client statistics).
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::OnRecvMsgFromClient(
        tss_sdk_impl::UserInfo *user,
        sec_proto::DPClientStatPtc &cmd) {
    ::time(&user->user_timer_.last_recv_stat_pkg_time_);

    int send_missing =
        (int)user->user_stat_.internal_msg_send_count_ -
        (int)cmd.internal_msg_recv_count_;
    int recv_missing =
        (int)cmd.internal_msg_send_count_ -
        (int)user->user_stat_.internal_fix_decrypt_count_ -
        (int)user->user_stat_.internal_dynamic_decrypt_count_ + 1;

    if (send_missing > 2 || recv_missing > 2) {
        DPBillAdapter bill("missing dp package", 0x28, user);
        bill << send_missing
             << recv_missing
             << user->user_stat_.internal_msg_send_count_
             << cmd.internal_msg_recv_count_
             << cmd.internal_msg_send_count_
             << user->user_stat_.internal_fix_decrypt_count_
             << user->user_stat_.internal_dynamic_decrypt_count_;
    }

    if (user->user_code_info_.down_cmd_list_crc_ !=
        cmd.down_cmd_list_crc_) {
        if (user->user_code_info_.client_state_ >
                tss_sdk_impl::ECS_INIT_ING ||
            user->user_stat_.game_dynamic_decrypt_count_ != 0 ||
            user->user_stat_.internal_dynamic_decrypt_count_ != 0) {
            DPBillAdapter bill("cmd list crc error", 0x25, user);
            bill << user->user_code_info_.down_cmd_list_crc_
                 << cmd.down_cmd_list_crc_;
            DPMsgPacker::send_error_report_msg(user, 0x198, 1);
        }
    }

    if (cmd.seq_less_count_ != 0) {
        DPBillAdapter bill("server seq error", 0x27, user);
        bill << cmd.seq_less_count_
             << cmd.seq_greater_count_
             << cmd.seq_discard_count_;
        DPMsgPacker::send_error_report_msg(user, 0x197,
                                            (int)cmd.seq_less_count_);
    }

    int fix_encrypt_mismatch =
        (int)cmd.game_fix_decrypt_count_ -
        ((int)user->user_stat_.fix_encrypt_count_ -
         (int)user->user_stat_.internal_fix_encrypt_count_);
    // ORIG 0xaf7fc reads cmd.fix_encrypt_count_ (+0x0c), not the dynamic
    // decrypt counter (+0x14): the client's fix-decrypt acknowledgement is
    // compared against this user's fix+internal decrypt counters.
    int fix_decrypt_mismatch =
        (int)user->user_stat_.game_fix_decrypt_count_ +
        (int)user->user_stat_.internal_fix_decrypt_count_ -
        (int)cmd.fix_encrypt_count_;

    if (fix_encrypt_mismatch > 2 || fix_decrypt_mismatch > 2) {
        DPBillAdapter bill("fix encrypt mismatch", 0x29, user);
        bill << fix_encrypt_mismatch
             << fix_decrypt_mismatch
             << cmd.game_fix_decrypt_count_
             << user->user_stat_.fix_encrypt_count_
             << user->user_stat_.internal_fix_encrypt_count_
             << user->user_stat_.internal_fix_decrypt_count_
             << user->user_stat_.game_fix_decrypt_count_;
        int total = (fix_encrypt_mismatch < 0 ? -fix_encrypt_mismatch
                                              : fix_encrypt_mismatch);
        total += (fix_decrypt_mismatch < 0 ? -fix_decrypt_mismatch
                                           : fix_decrypt_mismatch);
        DPMsgPacker::send_error_report_msg(user, 0x199, total);
    }

    if (cmd.need_encrypt_count_ != 0) {
        DPBillAdapter bill("server no encrypt", 0x26, user);
        int total = 0;
        for (int i = 0; i < (int)cmd.need_encrypt_count_; ++i) {
            bill << cmd.need_encrypt_list_[i].cmd_id_
                 << cmd.need_encrypt_list_[i].cmd_count_;
            total += (int)cmd.need_encrypt_list_[i].cmd_count_;
        }
        bill << total;
        DPMsgPacker::send_error_report_msg(user, 0x195, total);
    }

    int simulate_count =
        (int)cmd.game_dynamic_decrypt_count_ +
        (int)cmd.game_fix_decrypt_count_ -
        (int)user->user_stat_.dynamic_encrypt_count_ -
        ((int)user->user_stat_.fix_encrypt_count_ -
         (int)user->user_stat_.internal_fix_encrypt_count_);

    if (simulate_count > 0) {
        DPBillAdapter bill("simulate encrypt data", 0x2a, user);
        bill << simulate_count
             << cmd.game_fix_decrypt_count_
             << cmd.game_dynamic_decrypt_count_
             << user->user_stat_.dynamic_encrypt_count_
             << user->user_stat_.fix_encrypt_count_
             << user->user_stat_.internal_fix_encrypt_count_;
        DPMsgPacker::send_error_report_msg(user, 0x199, simulate_count);
    }

    if (cmd.client_error_count_ != 0) {
        DPBillAdapter bill("client report error", 0x24, user);
        for (int i = 0; i < (int)cmd.client_error_count_; ++i) {
            bill << cmd.client_error_list_[i].cmd_id_
                 << cmd.client_error_list_[i].cmd_count_;
            DPMsgPacker::send_error_report_msg(
                user,
                (int)cmd.client_error_list_[i].cmd_id_ + 0x1c2,
                (int)cmd.client_error_list_[i].cmd_count_);
        }
    }

    ZEN_LOG_PRIORITY priority =
        (user->base_info_.flags_ & 1) ? RS_INFO : RS_DEBUG;
    ZEN_Trace_LogMsg::debug_output(
        priority,
        "[%u][%s], client statsend_missing=%u, recv_missing=%u, "
        "fix_encrypt_mismatch=%u, fix_decrypt_mismatch=%u, "
        "simulate_count=%u, internal_msg_send_count_=%u, "
        "internal_msg_recv_count_=%u, game_pkg_recv_count_=%u, "
        "fix_encrypt_count_=%u, game_fix_decrypt_count_=%u, "
        "game_dynamic_decrypt_count_=%u, seq_less_count_=%u, "
        "seq_greater_count_=%u, seq_discard_count_=%u, "
        "down_cmd_list_crc_=%u, need_encrypt_count_=%u, "
        "client_error_count_=%u, internal_msg_send_count_=%u, "
        "internal_fix_encrypt_count_=%u, internal_fix_decrypt_count_=%u, "
        "internal_dynamic_decrypt_count_=%u, fix_encrypt_count_=%u, "
        "dynamic_encrypt_count_=%u, game_fix_decrypt_count_=%u, "
        "game_dynamic_decrypt_count_",
        user->base_info_.uin_, __func__,
        send_missing, recv_missing,
        fix_encrypt_mismatch, fix_decrypt_mismatch, simulate_count,
        cmd.internal_msg_send_count_,
        cmd.internal_msg_recv_count_,
        cmd.game_pkg_recv_count_,
        cmd.fix_encrypt_count_,
        cmd.game_fix_decrypt_count_,
        cmd.game_dynamic_decrypt_count_,
        cmd.seq_less_count_,
        cmd.seq_greater_count_,
        cmd.seq_discard_count_,
        cmd.down_cmd_list_crc_,
        cmd.need_encrypt_count_,
        cmd.client_error_count_,
        user->user_stat_.internal_msg_send_count_,
        user->user_stat_.internal_fix_encrypt_count_,
        user->user_stat_.internal_fix_decrypt_count_,
        user->user_stat_.internal_dynamic_decrypt_count_,
        user->user_stat_.fix_encrypt_count_,
        user->user_stat_.dynamic_encrypt_count_,
        user->user_stat_.game_fix_decrypt_count_,
        user->user_stat_.game_dynamic_decrypt_count_);

    return 0;
}

// ---------------------------------------------------------------------------
// 0x000afe70 .. 0x000b0b50: per-user timer state machine.
// States are UserInfo::E_CLIENT_STATE.  All paths return 0; the common tail
// reports 0x1a5 when the user's logging flag (base_info_.flags_ bit 0) is set.
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::check_timer(
        tss_sdk_impl::UserInfo *user, time_t now) {
    switch (user->user_code_info_.client_state_) {
    case tss_sdk_impl::ECS_NOT_INIT: {
        unsigned int elapsed =
            (unsigned int)now - (unsigned int)user->user_timer_.last_check_init_pkg_time_;
        if (elapsed >= tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_time_confg.max_init_pkg_timeout) {
            {
                DPBillAdapter bill("user init timeout", 8, user);
                bill << user->user_timer_.last_check_init_pkg_time_;
            }
            DPMsgPacker::send_error_report_msg(user, 0x19c, 1);
            user->user_timer_.last_check_init_pkg_time_ = now;
        }
        break;
    }

    case tss_sdk_impl::ECS_INIT_MSG: {
        if (user->user_code_info_.bin_file_code_ == 0) {
            int new_bin_file_code =
                vm_engine_->select_server_bin_file_id(user);
            user->user_code_info_.bin_file_code_ =
                (unsigned short)new_bin_file_code;
            if (new_bin_file_code != 0 && SendBinFile(user) == 0) {
                user->user_timer_.last_send_bin_file_time_ = now;
            } else {
                user->user_code_info_.bin_file_code_ = 0;
                DPMsgPacker::send_error_report_msg(user, 0x1a2, 1);
                break;
            }
        }

        if (user->user_code_info_.up_cmd_list_pos_ < 0) {
            if (SendCmdList(user) != 0)
                break;
        }

        if (user->user_code_info_.client_code_[0] == 0) {
            int new_client_code =
                vm_engine_->select_client_code_id(user);
            user->user_code_info_.client_code_[0] =
                (unsigned short)new_client_code;
            if (new_client_code != 0) {
                if (SendClientCode(user) != 0) {
                    user->user_code_info_.client_code_[0] = 0;
                    break;
                }
            } else {
                user->user_code_info_.client_code_[0] = 0;
            }
        }

        {
            int new_server_code =
                vm_engine_->select_server_code_id(user);
            user->user_code_info_.server_new_code_ =
                (unsigned short)new_server_code;
            if (new_server_code != 0) {
                if (SendServerCode(user) != 0) {
                    user->user_code_info_.server_new_code_ = 0;
                    break;
                }
                user->user_code_info_.client_state_ =
                    tss_sdk_impl::ECS_INIT_ING;
                user->user_timer_.last_send_bin_code_time_ = now;
            } else {
                user->user_code_info_.server_new_code_ = 0;
            }
        }
        break;
    }

    case tss_sdk_impl::ECS_INIT_MSG2: {
        if (user->user_code_info_.up_cmd_list_pos_ < 0) {
            if (SendCmdList(user) != 0)
                break;
        }
        if (SendServerCode(user) != 0)
            break;
        user->user_code_info_.client_state_ =
            tss_sdk_impl::ECS_INIT_ING;
        user->user_timer_.last_send_bin_code_time_ = now;
        break;
    }

    case tss_sdk_impl::ECS_INIT_ING: {
        unsigned int elapsed =
            (unsigned int)now - (unsigned int)user->user_timer_.last_send_bin_code_time_;
        if (elapsed >= tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_time_confg.max_confirm_pkg_timeout) {
            {
                DPBillAdapter bill("confirm overtime", 0x15, user);
                bill << user->user_timer_.last_send_bin_code_time_;
            }
            DPMsgPacker::send_error_report_msg(user, 0x19a, 1);
            user->user_timer_.last_send_bin_code_time_ = now;
        }
        break;
    }

    case tss_sdk_impl::ECS_INIT_CONFIRM: {
        unsigned int bin_file_elapsed =
            (unsigned int)now - (unsigned int)user->user_timer_.last_send_bin_file_time_;
        if (bin_file_elapsed >=
            tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_time_confg.send_bin_file_gap) {
            int old_bin_id = user->user_code_info_.bin_file_code_;
            int new_bin_id = vm_engine_->select_server_bin_file_id(user);
            user->user_code_info_.bin_file_code_ =
                (unsigned short)new_bin_id;
            if (new_bin_id != old_bin_id && new_bin_id != 0) {
                if (SendBinFile(user) != 0) {
                    user->user_code_info_.bin_file_code_ =
                        (unsigned short)old_bin_id;
                } else {
                    user->user_timer_.last_send_bin_file_time_ = now;
                }
            } else {
                user->user_code_info_.bin_file_code_ =
                    (unsigned short)old_bin_id;
            }
        }

        unsigned int code_elapsed =
            (unsigned int)now - (unsigned int)user->user_timer_.last_send_bin_code_time_;
        if (code_elapsed >= tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_time_confg.send_bin_code_gap) {
            int new_server_code =
                vm_engine_->select_server_code_id(user);
            int new_client_code =
                vm_engine_->select_client_code_id(user);
            if (new_client_code > 0 && new_server_code > 0) {
                if (new_server_code !=
                        (int)user->user_code_info_.server_cur_code_ ||
                    new_client_code !=
                        (int)user->user_code_info_.client_code_[0]) {
                    int old_client_code =
                        user->user_code_info_.client_code_[0];
                    user->user_code_info_.client_code_[0] =
                        (unsigned short)new_client_code;
                    int res = SendClientCode(user);
                    user->user_code_info_.client_code_[0] =
                        (unsigned short)old_client_code;
                    if (res == 0) {
                        ::memmove(&user->user_code_info_.client_code_[1],
                                  &user->user_code_info_.client_code_[0], 8);
                        user->user_code_info_.client_code_[0] =
                            (unsigned short)new_client_code;
                        user->user_code_info_.server_new_code_ =
                            (unsigned short)new_server_code;
                        if (SendServerCode(user) == 0) {
                            user->user_code_info_.client_state_ =
                                tss_sdk_impl::ECS_INIT_ING;
                            user->user_timer_.last_send_bin_code_time_ = now;
                        }
                    }
                }
            }
        }

        unsigned int test_pkg_elapsed =
            (unsigned int)now - (unsigned int)user->user_timer_.last_send_test_pkg_time_;
        if (test_pkg_elapsed >=
            tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_time_confg.max_send_test_pkg_timeout) {
            user->user_timer_.last_send_test_pkg_time_ = now;

            sec_proto::DPInterData pkg;
            pkg.head.packet_size_ = 15;
            pkg.head.protocol_type_ = 15;
            pkg.head.data_type_ = 0;
            pkg.body.cs_test_data_[0] = '0';
            pkg.body.cs_test_data_[1] = '1';
            pkg.body.cs_test_data_[2] = '2';
            pkg.body.cs_test_data_[3] = '3';
            pkg.body.cs_test_data_[4] = '4';
            pkg.body.cs_test_data_[5] = '5';
            pkg.body.cs_test_data_[6] = '6';
            pkg.body.cs_test_data_[7] = '7';
            pkg.body.cs_test_data_[8] = '8';
            pkg.body.cs_test_data_[9] = '9';
            SendDpMsgToClient(user, pkg, 0, 0);

            pkg.head.packet_size_ = 8;
            pkg.head.protocol_type_ = 0x14;
            pkg.head.data_type_ = 0;
            pkg.body.config_data_.proto_check_switch_ =
                tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_proto_conf.proto_check_switch;
            pkg.body.config_data_.seq_check_switch_ =
                tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_proto_conf.seq_check_switch;
            pkg.body.config_data_.raw_check_switch_ =
                tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_proto_conf.raw_check_switch;
            SendDpMsgToClient(user, pkg, 0, 0);
        }

        unsigned int stat_elapsed =
            (unsigned int)now - (unsigned int)user->user_timer_.last_recv_stat_pkg_time_;
        if (stat_elapsed >=
            tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_time_confg.max_static_pkg_timeout) {
            user->user_timer_.last_recv_stat_pkg_time_ = now;
            {
                DPBillAdapter bill("static pkg overtime", 0x17, user);
            }
            DPMsgPacker::send_error_report_msg(user, 0x196, 1);
        }
        break;
    }

    default:
        break;
    }

    if (user->base_info_.flags_ & 1)
        DPMsgPacker::send_error_report_msg(user, 0x1a5, 1);
    return 0;
}

// ---------------------------------------------------------------------------
// 0x000b0b50 .. 0x000b0fbf: DPClientInitDataPtc (client initialization).
// ---------------------------------------------------------------------------
int tss_dpsdk::DPMsgHandler::OnRecvMsgFromClient(
        tss_sdk_impl::UserInfo *user,
        sec_proto::DPClientInitDataPtc &cmd) {
    user->user_code_info_.client_state_ =
        tss_sdk_impl::ECS_INIT_MSG;

    user->base_info_.client_user_id_ =
        Crc32((unsigned char *)&cmd.client_user_id_, 4);

    if (tss_dp_comm::DP_CONFIG::glb_dp_config_->
            dp_base_conf.is_use_client_version)
        user->base_info_.client_version_ = cmd.client_info_;

    int is_bin_count_mismatch = -1;
    int is_client_code_mismatch = 1;
    int is_server_code_mismatch = 1;
    long long new_code = -1;

    if (vm_engine_->check_version_hash(user, cmd.code_ver_)) {
        is_client_code_mismatch = vm_engine_->check_client_code(
            user, (unsigned short)cmd.client_code_id_);
        is_server_code_mismatch = vm_engine_->check_server_code(
            user, (unsigned short)cmd.server_code_id_);
        is_bin_count_mismatch = 0;
        new_code = 0;
    }

    {
        DPBillAdapter bill("user init msg", 7, user);
        bill << new_code
             << is_client_code_mismatch
             << is_server_code_mismatch
             << cmd.client_info_
             << cmd.client_code_id_
             << cmd.server_code_id_
             << cmd.bin_code_id_
             << cmd.bin_count_
             << cmd.client_user_id_
             << user->base_info_.client_user_id_;
    }

    time_t now = ::time(0);

    if (is_bin_count_mismatch != 0)
        SendBinCount(user);

    if (is_client_code_mismatch == 0 && is_server_code_mismatch == 0) {
        user->user_code_info_.bin_file_code_ = cmd.bin_code_id_;
        user->user_code_info_.client_code_[0] =
            (unsigned short)cmd.client_code_id_;
        user->user_code_info_.server_cur_code_ =
            (unsigned short)cmd.server_code_id_;
        user->user_code_info_.server_new_code_ =
            (unsigned short)cmd.server_code_id_;
        user->user_timer_.last_send_bin_file_time_ = now;
        user->user_timer_.last_send_bin_code_time_ = now;
        user->user_code_info_.client_state_ =
            tss_sdk_impl::ECS_INIT_MSG2;
    }

    return check_timer(user, now);
}
