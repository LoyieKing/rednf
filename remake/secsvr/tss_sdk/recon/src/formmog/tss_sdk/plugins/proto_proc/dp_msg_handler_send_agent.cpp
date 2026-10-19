// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.0 family,
// -O2 -fPIC).  ORIG symbols:
//   tss_dpsdk::DPMsgHandler::SendDpMsgToClient(UserInfo*, unsigned char*, unsigned short)       @0x000ae3b0
//   tss_dpsdk::DPMsgHandler::SendDpMsgToClient(UserInfo*, DPInterData&, unsigned char*, unsigned short) @0x000ae580
//   tss_dpsdk::DPMsgHandler::SendBinCount(UserInfo*)       @0x000ae630
//   tss_dpsdk::DPMsgHandler::SendClientCode(UserInfo*)     @0x000ae900
//   tss_dpsdk::DPMsgHandler::SendBinFile(UserInfo*)        @0x000aeaa0
//   tss_dpsdk::DPMsgHandler::SendCmdList(UserInfo*)        @0x000aed00
//   tss_dpsdk::DPMsgHandler::SendServerCode(UserInfo*)     @0x000aee50
//
// This TU intentionally contains only partial declarations of the external
// classes whose layout/entry points are needed to emit the seven exact
// symbols.  It is not a definition of the real DPMsgHandler/DPVMBaseEngine/
// DPBillAdapter classes and must not be used as such when the authoritative
// headers are integrated.

#include <string.h>

#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"
#include "src/origsrc/formmog_proto_tss_sdk_client.h"

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/layout/dp_vmbase_engine.h"

namespace tss_sdk_impl {
typedef UserInfo UserInfoType;
}

// ---------------------------------------------------------------------------
// sec_proto wire structures (DWARF: DPInterDataHead size 5, DPInterData size
// 1585 with body at +5; all protocol payloads are byte-packed).
// ---------------------------------------------------------------------------


typedef char Check_DPInterDataHead_size[sizeof(sec_proto::DPInterDataHead) == 5 ? 1 : -1];
typedef char Check_DPInterData_size[sizeof(sec_proto::DPInterData) == 1585 ? 1 : -1];

// ---------------------------------------------------------------------------
// tss_dpsdk code table layouts used by SendBinFile/SendServerCode.  DWARF
// sizes/offsets: CodeItem<20000> 20004, CodeItems<20000> 2000402,
// DpCodeInfo 4802464, DpCodeInfos 9604936 (code_infos_ at +8).
// ---------------------------------------------------------------------------
namespace tss_dpsdk {

template <int N>
struct CodeItem {
    unsigned short weight_;         // +0
    unsigned short code_len_;       // +2
    unsigned char code_buf_[N];     // +4
};

template <int N>
struct CodeItems {
    unsigned short total_weight_;   // +0
    CodeItem<N> items_[100];        // +2
};

struct CodeInfo {
    CodeItems<2000> cryptdata_code_;    // +0
    CodeItems<2000> crypt_code_;        // +200402
    CodeItems<20000> bin_file_;         // +400804
};

struct DpCodeInfo {
    unsigned short version_count_;          // +0
    unsigned int version_support_list_[8];  // +4
    unsigned char version_hash_[16];        // +36
    CodeInfo client_code_;                  // +52
    CodeInfo server_code_;                  // +2401258
};

struct DpCodeInfos {
    unsigned int code_update_time_;     // +0
    unsigned short code_count_;         // +4
    DpCodeInfo code_infos_[2];          // +8
};

typedef char Check_CodeItem20000_size[sizeof(CodeItem<20000>) == 20004 ? 1 : -1];
typedef char Check_CodeInfo_size[sizeof(CodeInfo) == 2401206 ? 1 : -1];
typedef char Check_DpCodeInfo_size[sizeof(DpCodeInfo) == 4802464 ? 1 : -1];
typedef char Check_DpCodeInfos_size[sizeof(DpCodeInfos) == 9604936 ? 1 : -1];
typedef char Check_DpCodeInfo_version_hash[
    __builtin_offsetof(DpCodeInfo, version_hash_) == 0x24 ? 1 : -1];
typedef char Check_DpCodeInfo_server_bin_len0[
    __builtin_offsetof(DpCodeInfo,
        server_code_.bin_file_.items_[0].code_len_) == 0x2ac192 ? 1 : -1];
typedef char Check_DpCodeInfo_server_bin_buf0[
    __builtin_offsetof(DpCodeInfo,
        server_code_.bin_file_.items_[0].code_buf_) == 0x2ac194 ? 1 : -1];
typedef char Check_DpCodeInfo_server_bin_len1[
    __builtin_offsetof(DpCodeInfo,
        server_code_.bin_file_.items_[1].code_len_) == 0x2b0fb6 ? 1 : -1];
typedef char Check_DpCodeInfo_server_bin_buf1[
    __builtin_offsetof(DpCodeInfo,
        server_code_.bin_file_.items_[1].code_buf_) == 0x2b0fb8 ? 1 : -1];
typedef char Check_DpCodeInfos_code_count[
    __builtin_offsetof(DpCodeInfos, code_count_) == 4 ? 1 : -1];
typedef char Check_UserInfo_flags[
    __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.flags_) == 0x20 ? 1 : -1];
typedef char Check_UserInfo_client_version[
    __builtin_offsetof(tss_sdk_impl::UserInfo, base_info_.client_version_) == 0x18 ? 1 : -1];
typedef char Check_UserInfo_server_new_code[
    __builtin_offsetof(tss_sdk_impl::UserInfo, user_code_info_.server_new_code_) == 0x7a ? 1 : -1];
typedef char Check_UserInfo_client_code0[
    __builtin_offsetof(tss_sdk_impl::UserInfo, user_code_info_.client_code_[0]) == 0x7c ? 1 : -1];
typedef char Check_UserInfo_bin_file_code[
    __builtin_offsetof(tss_sdk_impl::UserInfo, user_code_info_.bin_file_code_) == 0x86 ? 1 : -1];
typedef char Check_UserInfo_down_crc[
    __builtin_offsetof(tss_sdk_impl::UserInfo, user_code_info_.down_cmd_list_crc_) == 0x8c ? 1 : -1];
typedef char Check_UserInfo_up_pos[
    __builtin_offsetof(tss_sdk_impl::UserInfo, user_code_info_.up_cmd_list_pos_) == 0x90 ? 1 : -1];
typedef char Check_UserInfo_down_pos[
    __builtin_offsetof(tss_sdk_impl::UserInfo, user_code_info_.down_cmd_list_pos_) == 0x92 ? 1 : -1];
typedef char Check_UserInfo_fix_encrypt[
    __builtin_offsetof(tss_sdk_impl::UserInfo, user_stat_.internal_fix_encrypt_count_) == 0xb0 ? 1 : -1];

class DPCodeMgr {
public:
    DpCodeInfos* code_infos_;           // +0
};

// layout/dp_vmbase_engine.h keeps code_manager_ private; the send methods
// read the same pointer at +0x08.  This layout view is not an ODR definition
// of the class (the canonical class remains the only DPVMBaseEngine).
struct DPVMBaseEngineLayout {
    void* encrypt_vm_;          // +0x00
    void* decrypt_vm_;          // +0x04
    DPCodeMgr* code_manager_;   // +0x08
};

inline DPCodeMgr* code_manager_of(tss_dpsdk::DPVMBaseEngine* vm) {
    return reinterpret_cast<DPVMBaseEngineLayout*>(vm)->code_manager_;
}

class DPMsgPacker {
public:
    static int send_dp_msg_to_client(tss_sdk_impl::UserInfo* user,
                                     unsigned char* buf,
                                     unsigned short buf_len);
};

}  // namespace tss_dpsdk


namespace AB_NS_CODEENGINE {
int EncodeInt(char** buf, unsigned int value);
}

// ---------------------------------------------------------------------------
// tss_dp_comm configuration prefix needed by SendCmdList.  Only the
// dp_code_info member is modelled; the four leading pads preserve the ORIG
// offsets (dp_code_info at +0x154).
// ---------------------------------------------------------------------------
namespace tss_dp_comm {

struct DPValueArray_128 {
    unsigned short length;              // +0
    unsigned int values[128];           // +2
} __attribute__((packed));

struct DPCmdLists {
    unsigned short cur_posi;            // +0
    unsigned short lists_count;         // +2
    unsigned int cur_crc;               // +4
    DPValueArray_128 cmd_lists[4];      // +8
} __attribute__((packed));

struct DPCodeInfo {
    unsigned short cmd_list_buf_size;   // +0
    unsigned char cmd_list_buf[10240];  // +2
    DPCmdLists up_cmd_list;             // +10242
    DPCmdLists down_cmd_list;           // +12306
    unsigned short code_dir_count;      // +14370
    char code_dir_path[2][256];         // +14372
} __attribute__((packed));

struct DPConfigPrefixBase {
    unsigned char raw_[6];              // dp_base_conf, +0
} __attribute__((packed));

struct DPConfigPrefixProto {
    unsigned char raw_[10];             // dp_proto_conf, +6
} __attribute__((packed));

struct DPConfigPrefixLog {
    unsigned char raw_[296];            // dp_log_conf, +16
} __attribute__((packed));

struct DPConfigPrefixTime {
    unsigned char raw_[28];             // dp_time_confg, +312
} __attribute__((packed));

struct DPConfig {
    DPConfigPrefixBase dp_base_conf;    // +0
    DPConfigPrefixProto dp_proto_conf;  // +6
    DPConfigPrefixLog dp_log_conf;      // +16
    DPConfigPrefixTime dp_time_confg;   // +312
    DPCodeInfo dp_code_info;            // +340 (0x154)
} __attribute__((packed));

struct DP_CONFIG {
    // ORIG: _ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E, B 4 bytes.
    static DPConfig* glb_dp_config_;
};

}  // namespace tss_dp_comm

typedef char Check_DPCodeInfo_size[sizeof(tss_dp_comm::DPCodeInfo) == 14884 ? 1 : -1];
typedef char Check_DPCmdLists_size[sizeof(tss_dp_comm::DPCmdLists) == 2064 ? 1 : -1];
typedef char Check_DPConfig_dp_code_info_offset[
    __builtin_offsetof(tss_dp_comm::DPConfig, dp_code_info) == 0x154 ? 1 : -1];
typedef char Check_DPConfig_up_pos_offset[
    __builtin_offsetof(tss_dp_comm::DPConfig, dp_code_info.up_cmd_list.cur_posi) == 0x2956 ? 1 : -1];
typedef char Check_DPConfig_down_pos_offset[
    __builtin_offsetof(tss_dp_comm::DPConfig, dp_code_info.down_cmd_list.cur_posi) == 0x3166 ? 1 : -1];
typedef char Check_DPConfig_down_crc_offset[
    __builtin_offsetof(tss_dp_comm::DPConfig, dp_code_info.down_cmd_list.cur_crc) == 0x316a ? 1 : -1];

// ---------------------------------------------------------------------------
// Implementations.
// ---------------------------------------------------------------------------
namespace tss_dpsdk {

int DPMsgHandler::SendDpMsgToClient(tss_sdk_impl::UserInfo* user,
                                    unsigned char* buf,
                                    unsigned short buf_len) {
    unsigned int uin = 0;
    int priority = 2;
    if (user) {
        uin = user->base_info_.uin_;
        priority = (user->base_info_.flags_ & 1)
                       ? RS_INFO
                       : RS_DEBUG;
    }
    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)priority, "[%u][%s], buf_len=%u",
        uin, __func__, (unsigned int)buf_len);

    // ORIG dereferences user unconditionally at this point.
    if (user->base_info_.flags_ & 1) {
        DPBillAdapter::dump("SendDpMsgToClient before encrypt",
                            user->base_info_.uin_, buf, buf_len);
    }

    unsigned char out_buf[65536];
    // ORIG initial constant 0xfff6; payload starts at out_buf + 9.
    unsigned int out_len = 0xfff6;

    sec_proto::DPInterDataHead head;
    head.protocol_type_ = 0x13;
    head.data_type_ = 0;

    int res = vm_engine_->encrypt(user, buf, buf_len, out_buf + 9,
                                  out_len, true);
    if (res != 0) {
        return -1;
    }

    head.packet_size_ = (unsigned short)(out_len + 5);
    head.pack((char*)(out_buf + 4), 0xfffb, 0, 0);

    char* ptr = (char*)out_buf;
    AB_NS_CODEENGINE::EncodeInt(&ptr, out_len + 9);

    unsigned short total_len = (unsigned short)(out_len + 9);
    if (user->base_info_.flags_ & 1) {
        DPBillAdapter::dump("SendDpMsgToClient after encrypt",
                            user->base_info_.uin_, out_buf, total_len);
    }

    res = DPMsgPacker::send_dp_msg_to_client(user, out_buf, total_len);
    if (res != 0) {
        return res;
    }
    ++user->user_stat_.internal_fix_encrypt_count_;
    return 0;
}

int DPMsgHandler::SendDpMsgToClient(tss_sdk_impl::UserInfo* user,
                                    sec_proto::DPInterData& data,
                                    unsigned char* buf,
                                    unsigned short buf_len) {
    unsigned char out_buf[0xffff];
    unsigned int len = 0;
    data.pack((char*)out_buf, 0xffff, &len, 0);
    if (buf != 0 && buf_len != 0) {
        memcpy(out_buf + len, buf, buf_len);
        len += buf_len;
    }
    return SendDpMsgToClient(user, out_buf, (unsigned short)len);
}

int DPMsgHandler::SendBinCount(tss_sdk_impl::UserInfo* user) {
    sec_proto::DPInterData data;
    data.head.packet_size_ = 7;
    data.head.protocol_type_ = 5;
    data.head.data_type_ = 0;
    data.body.bin_count_.bin_count_ = 5;

    int res = SendDpMsgToClient(user, data, 0, 0);
    if (res != 0) {
        DPBillAdapter bill("send bin count fail", 0xd, user);
        return -1;
    }

    unsigned int uin = 0;
    int priority = 2;
    if (user) {
        uin = user->base_info_.uin_;
        priority = (user->base_info_.flags_ & 1)
                       ? RS_INFO
                       : RS_DEBUG;
    }
    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)priority, "[%u][%s], send bin count",
        uin, __func__);
    DPMonitorReporter::report_data(0x3f6, 1);
    return 0;
}

int DPMsgHandler::SendClientCode(tss_sdk_impl::UserInfo* user) {
    unsigned short code_len = 0;
    const char* fail_msg = 0;
    unsigned char* code = vm_engine_->get_client_code(
        user, user->user_code_info_.client_code_[0], code_len);
    if (code == 0 || code_len == 0) {
        fail_msg = "get client code fail";
        goto bill_fail;
    }

    {
        sec_proto::DPInterData data;
        data.head.packet_size_ = (unsigned short)(code_len + 9);
        data.head.protocol_type_ = 1;
        data.head.data_type_ = 0;
        data.body.client_code_.code_id_ =
            user->user_code_info_.client_code_[0];

        int res = SendDpMsgToClient(user, data, code, code_len);
        if (res != 0) {
            fail_msg = "send client code fail";
            goto bill_fail;
        }

        int priority = (user->base_info_.flags_ & 1)
                           ? RS_INFO
                           : RS_DEBUG;
        ZEN_Trace_LogMsg::debug_output(
            (ZEN_LOG_PRIORITY)priority,
            "[%u][%s], send client code, code_len=%u, client_code_=%u",
            user->base_info_.uin_, __func__, (unsigned int)code_len,
            (unsigned int)user->user_code_info_.client_code_[0]);
        DPMonitorReporter::report_data(0x3f8, 1);
        return 0;
    }

bill_fail:
    DPBillAdapter bill(fail_msg, 0xf, user);
    DPBillAdapter::bill_ << code_len;
    DPBillAdapter::bill_ << user->user_code_info_.client_code_[0];
    return -1;
}

int DPMsgHandler::SendBinFile(tss_sdk_impl::UserInfo* user) {
    unsigned short bin_file_code = user->user_code_info_.bin_file_code_;
    unsigned int client_version = user->base_info_.client_version_;
    unsigned short code_len = 0;
    const char* fail_msg = 0;

    DpCodeInfos* code_infos = code_manager_of(vm_engine_)->code_infos_;
    const DpCodeInfo* found_info = 0;
    for (unsigned int i = 0; i < code_infos->code_count_; ++i) {
        const DpCodeInfo& info = code_infos->code_infos_[i];
        if (info.version_count_ == 0) {
            continue;
        }
        for (unsigned int j = 0; j < info.version_count_; ++j) {
            unsigned int v = info.version_support_list_[j];
            if (v == client_version || v == 0) {
                found_info = &info;
                break;
            }
        }
        if (found_info != 0) {
            break;
        }
    }

    if (found_info == 0) {
        fail_msg = "get bin file fail";
        goto bill_fail;
    }
    if ((unsigned short)(bin_file_code - 1) > 0x62) {
        fail_msg = "get bin file fail";
        goto bill_fail;
    }

    {
        const CodeItem<20000>& item =
            found_info->server_code_.bin_file_.items_[bin_file_code];
        code_len = item.code_len_;
        if (code_len == 0) {
            fail_msg = "get bin file fail";
            goto bill_fail;
        }

        sec_proto::DPInterData data;
        data.head.packet_size_ = (unsigned short)(code_len + 9);
        data.head.protocol_type_ = 3;
        data.head.data_type_ = 0;
        data.body.bin_file_.length = (unsigned short)(code_len + 4);
        data.body.bin_file_.bin_code_id_ = bin_file_code;

        int res = SendDpMsgToClient(user, data,
                                    (unsigned char*)item.code_buf_, code_len);
        if (res != 0) {
            fail_msg = "send bin file fail";
            goto bill_fail;
        }

        int priority = (user->base_info_.flags_ & 1)
                           ? RS_INFO
                           : RS_DEBUG;
        ZEN_Trace_LogMsg::debug_output(
            (ZEN_LOG_PRIORITY)priority,
            "[%u][%s], send bin file, code_len=%u, bin_file_code_=%u",
            user->base_info_.uin_, __func__, (unsigned int)code_len,
            (unsigned int)bin_file_code);
        DPMonitorReporter::report_data(0x3f7, 1);
        return 0;
    }

bill_fail:
    DPBillAdapter bill(fail_msg, 0xb, user);
    DPBillAdapter::bill_ << code_len;
    DPBillAdapter::bill_ << bin_file_code;
    return -1;
}

int DPMsgHandler::SendCmdList(tss_sdk_impl::UserInfo* user) {
    int res = SendDpMsgToClient(
        user, tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.cmd_list_buf,
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.cmd_list_buf_size);
    if (res != 0) {
        DPBillAdapter bill("send cmd list fail", 0x13, user);
        DPBillAdapter::bill_ <<
            tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.cmd_list_buf_size;
        return -1;
    }

    user->user_code_info_.up_cmd_list_pos_ =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.up_cmd_list.cur_posi;
    user->user_code_info_.down_cmd_list_pos_ =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.down_cmd_list.cur_posi;
    user->user_code_info_.down_cmd_list_crc_ =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->dp_code_info.down_cmd_list.cur_crc;

    int priority = (user->base_info_.flags_ & 1)
                       ? RS_INFO
                       : RS_DEBUG;
    ZEN_Trace_LogMsg::debug_output(
        (ZEN_LOG_PRIORITY)priority,
        "[%u][%s], send cmd list, up_cmd_list_pos_=%u, "
        "down_cmd_list_pos_=%u, down_cmd_list_crc_=%u",
        user->base_info_.uin_, __func__,
        user->user_code_info_.up_cmd_list_pos_,
        user->user_code_info_.down_cmd_list_pos_,
        user->user_code_info_.down_cmd_list_crc_);
    DPMonitorReporter::report_data(0x3fa, 1);
    return 0;
}

int DPMsgHandler::SendServerCode(tss_sdk_impl::UserInfo* user) {
    unsigned short code_len = 0;
    const char* fail_msg = 0;
    unsigned char* code = vm_engine_->get_server_code(
        user, user->user_code_info_.server_new_code_, code_len);
    if (code == 0 || code_len == 0) {
        fail_msg = "get server code fail";
        goto bill_fail;
    }

    {
        sec_proto::DPInterData data;
        data.head.packet_size_ = (unsigned short)(code_len + 0x19);
        data.head.protocol_type_ = 6;
        data.head.data_type_ = 0;
        data.body.server_code_.code_id_ =
            user->user_code_info_.server_new_code_;

        unsigned int client_version = user->base_info_.client_version_;
        DpCodeInfos* code_infos = code_manager_of(vm_engine_)->code_infos_;
        for (unsigned int i = 0; i < code_infos->code_count_; ++i) {
            const DpCodeInfo& info = code_infos->code_infos_[i];
            if (info.version_count_ == 0) {
                continue;
            }
            for (unsigned int j = 0; j < info.version_count_; ++j) {
                unsigned int v = info.version_support_list_[j];
                if (v == client_version || v == 0) {
                    memcpy(data.body.server_code_.client_version_,
                           info.version_hash_, 16);
                    goto send_message;
                }
            }
        }

send_message:
        int res = SendDpMsgToClient(user, data, code, code_len);
        if (res != 0) {
            fail_msg = "send server code fail";
            goto bill_fail;
        }

        {
            DPBillAdapter bill("send server code", 0x10, user);
            DPBillAdapter::bill_ << code_len;
            DPBillAdapter::bill_ << user->user_code_info_.bin_file_code_;
            DPBillAdapter::bill_ << user->user_code_info_.client_code_[0];
            DPBillAdapter::bill_ << user->user_code_info_.server_new_code_;
            DPBillAdapter::bill_ << user->user_code_info_.up_cmd_list_pos_;
            DPBillAdapter::bill_ << user->user_code_info_.down_cmd_list_pos_;
            DPBillAdapter::bill_ << user->user_code_info_.down_cmd_list_crc_;
            DPMonitorReporter::report_data(0x3f9, 1);
        }
        return 0;
    }

bill_fail:
    DPBillAdapter bill(fail_msg, 0x11, user);
    DPBillAdapter::bill_ << code_len;
    DPBillAdapter::bill_ << user->user_code_info_.server_new_code_;
    return -1;
}

}  // namespace tss_dpsdk
