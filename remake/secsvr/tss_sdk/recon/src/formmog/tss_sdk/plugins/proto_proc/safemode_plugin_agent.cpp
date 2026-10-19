// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1 ABI).
//
// Translation unit: src/formmog/tss_sdk/plugins/safemode/safemode_plugin.cpp
//
// Reconstructed definitions (shipped symbol addresses):
//   TssSdkSafeModePlugin::TssSdkSafeModePlugin() (C1) 0x000c4a50-0x000c4ab0
//   TssSdkSafeModePlugin::TssSdkSafeModePlugin() (C2) 0x000c4b70-0x000c4bd0
//   get_obj_TssSdkSafeModePlugin()                   0x000c4ad0-0x000c4b66
//   TssSdkSafeModePlugin::fini()                     0x000c49a0-0x000c49a6
//   TssSdkSafeModePlugin::init(void*)                0x000c5040-0x000c512f
//   TssSdkSafeModePlugin::notify(void*)              0x000c5130-0x000c63af
//   TssSdkSafeModePlugin::proc(void*)                0x000c63b0-0x000c6e2f
//   TssSdkSafeModePlugin::~TssSdkSafeModePlugin() D0  0x000c6e30-0x000c6e6e
//   TssSdkSafeModePlugin::~TssSdkSafeModePlugin() D1  0x000c6e70-0x000c6e9d
//
// Class layout (DWARF, sizeof == 24):
//   +0x00 vptr / +0x04 callback_proc_ / +0x08 error_info_ /
//   +0x0c sdk_plugin_id_ (SAFEMODE == 4)
//   +0x10 DPInterface *dp_if_
//   +0x14 unsigned int channel_id_
//
// notify() receives the plugin package directly (the framework unwraps
// TssSdkPluginNotifyData::notify_data_).  Its 16-bit head_.msg_type_ selects:
//   18 (0x12) : safemode status response  -- built by proc(), ignored here
//   20 (0x14) : acknowledged/ignored
//   21 (0x15) : limit-status response     -- built by proc(), ignored here
//   22 (0x16) : append-parameter request  -- rewrites append_param_.url_
//   24 (0x18) : acknowledged/ignored
// Any other type sets error code 0x496a4 (300708).
//
// proc() receives Comm_App_Frame* from the channel plugin:
//   0x493ec (300012) : appdata<SafemodeStatusRsp>; build msg type 18,
//                      invoke callback, then DPInterface::on_recv_msg.
//   0x493ee (300014) : appdata<SafemodeLimitStatusRsp>; build msg type 21,
//                      rewrite unlock_url_, invoke callback.
// Other commands are logged and return 0.
//
// The append-parameter path is a faithful model of the shipped code,
// including its uninitialised 1024-byte scratch buffer on the branch where
// the URL contains no "seckey=" parameter (the first strstr() observes
// uninitialised stack bytes in ORIG).

#include <cstring>
#include <string>
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ORIG TU-local static-init block: __ioinit + COMM_STAT_ITEM_WITH_NAME[16].
#include "src/static_init_block.h"

// ---------------------------------------------------------------------------
// External ZEN/plugin boundary: canonical tag-394 declarations.
// ---------------------------------------------------------------------------

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"

// ORIG static-init block, part 2: Singleton<TssSdkCommSvc>::object_creator.
// zen_singleton.h defines instance() in-class, so this TU's dynamic
// initializer inlines the guarded sequence exactly like the ORIG
// (audit/report/init_brief.md).


// ORIG .rodata literals of this CU (const char[] at global scope -> internal
// linkage, exactly the three local DWARF variables of safemode_plugin.cpp).
// 0x21e960 / 0x21e967 / 0x21e980.
static const char TSS_SDK_SAFEMODE_PLUGIN_PROMPT_PARAM_KEY[7] = "seckey";
static const char TSS_SDK_SAFEMODE_PLUGIN_ENCODE_DECODE_KEY[16] =
    "sadffsdadfefffe";
static const char TSS_SDK_SAFEMODE_PLUGIN_VERIFY_CGI[40] =
    "/cgi-bin/libtobacgi.so.1.0.0?req=query&";

// zen_base64.cpp exports C++-mangled symbols
// (_Z13base64_encodePKhjPcPj / _Z13base64_decodePKcjPhPj); declaring them
// inside extern "C" produced C imports that do not exist in ORIG.
int base64_encode(const unsigned char *input, unsigned int input_len,
                  char *output, unsigned int *output_len);
int base64_decode(const char *input, unsigned int input_len,
                  unsigned char *output, unsigned int *output_len);

void TenTean(unsigned char *key, bool encrypt, unsigned char *buf,
             unsigned int len);

namespace tss_sdk_impl {

class UserInfo;

}  // namespace tss_sdk_impl

#include "src/formmog/tss_sdk/layout/dp_interface.h"


// ---------------------------------------------------------------------------
// Frame/protocol types shared with the channel plugin.
// ---------------------------------------------------------------------------


#include "src/commlib/framework/comm_app_frame.h"

// sec_proto::SafemodeStatusRsp / SafemodeLimitStatusRsp: the canonical
// declarations of the protocol TU (formmog_proto_public_tsssdk.cpp); the local
// copies that split these wire types away from it are gone.
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"

// ---------------------------------------------------------------------------
// Plugin package (DWARF anonymous struct, sizeof == 90).
// ---------------------------------------------------------------------------

namespace {

struct SafeModeHead {
    unsigned short msg_type_;   // +0
    unsigned int uin_;          // +2
} __attribute__((packed));      // sizeof == 6

struct SafeModeUpdate {
    unsigned short ipc_days_;       // +0
    signed char safemode_level_;    // +2
    signed char has_verify_mb_;     // +3
    unsigned short event_type_;     // +4
    signed char reserve_[32];       // +6
} __attribute__((packed));          // sizeof == 38

struct SafeModeLimitStatus {
    int ret_;                       // +0
    unsigned int uin_;              // +4
    int limit_status_;              // +8
    unsigned int unlock_url_len_;   // +12
    char *unlock_url_;              // +16
} __attribute__((packed));          // sizeof == 20

struct SafeModeParam {
    const char *param_key_;         // +0
    const char *param_value_;       // +4
};

struct SafeModeAppendParam {
    char *url_;                     // +0
    unsigned int url_buf_max_len_;  // +4
    unsigned int param_count_;      // +8
    SafeModeParam *param_;          // +12
} __attribute__((packed));          // sizeof == 16

struct SafeModeMbaSafeStatus {
    unsigned int uin_;              // +0
    unsigned int status_;           // +4
};

struct SafeModeMbaVerifyInfo {
    unsigned int uin_;              // +0
    unsigned int seq_;              // +4
    short verify_url_len_;          // +8
    char *verify_url_;              // +10
} __attribute__((packed));          // sizeof == 14

struct SafeModeMbaLaunchVerify {
    unsigned int uin_;              // +0
    unsigned int seq_;              // +4
    unsigned int op_level_;         // +8
    unsigned int op_scene_len_;     // +12
    char op_scene_[64];             // +16
    unsigned int client_ip_;        // +80
};                                  // sizeof == 84

struct SafeModeMbaVerifyStatus {
    unsigned int uin_;              // +0
    unsigned int seq_;              // +4
    unsigned int result_;           // +8
};

union SafeModeBody {
    SafeModeUpdate safemode_update_;
    SafeModeUpdate safemode_ack_;
    SafeModeLimitStatus limit_status_;
    SafeModeAppendParam append_param_;
    SafeModeMbaSafeStatus mba_safe_status_;
    SafeModeMbaVerifyInfo mba_verify_info_;
    SafeModeMbaLaunchVerify mba_launch_verify_;
    SafeModeMbaVerifyStatus mba_verify_status_;
} __attribute__((packed));          // sizeof == 84

struct SafeModePkg {
    SafeModeHead head_;             // +0
    SafeModeBody body_;             // +6
} __attribute__((packed));          // sizeof == 90

}  // namespace

namespace tss_sdk_impl {

class TssSdkSafeModePlugin : public ITssSdkPlugin {
public:
    TssSdkSafeModePlugin();
    virtual ~TssSdkSafeModePlugin() {}

    virtual int init(void *init_data);
    virtual int fini();
    virtual int proc(void *data);
    virtual int notify(void *data);

private:
    DPInterface *dp_if_;         // +0x10
    unsigned int channel_id_;    // +0x14
};

TssSdkSafeModePlugin *get_obj_TssSdkSafeModePlugin();

}  // namespace tss_sdk_impl

using tss_sdk_impl::TssSdkSafeModePlugin;
using tss_sdk_impl::TssSdkCommSvc;

// ---------------------------------------------------------------------------
// ctor / dtor / singleton accessor.
// ---------------------------------------------------------------------------

namespace tss_sdk_impl {

TssSdkSafeModePlugin::TssSdkSafeModePlugin()
    : ITssSdkPlugin(tss_sdk_if::ETSS_SDK_PLUGIN_ID_SAFEMODE),
      dp_if_(0),
      channel_id_(0) {
    ZEN_Trace_LogMsg::debug_debugEx("In CTssSdkSafeModePlugin ctor.");
}

TssSdkSafeModePlugin *get_obj_TssSdkSafeModePlugin() {
    static TssSdkSafeModePlugin s_obj_TssSdkSafeModePlugin;
    return &s_obj_TssSdkSafeModePlugin;
}

int TssSdkSafeModePlugin::init(void *init_data) {
    (void)init_data;
    dp_if_ = ::Singleton<TssSdkCommSvc>::instance()->dp_if_;
    channel_id_ = ::Singleton<TssSdkCommSvc>::instance()->channel_id_;
    return 0;
}

int TssSdkSafeModePlugin::fini() {
    return 0;
}

// ---------------------------------------------------------------------------
// notify: append-parameter request (msg type 22).
// ---------------------------------------------------------------------------

int TssSdkSafeModePlugin::notify(void *data) {
    if (data == 0) {
        std::string message(
            "In CTssSdkSafeModePlugin::notify, proc_data is NULL.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x4969d;  // 300701
        return error_info_.last_error_info()->error_code;
    }

    SafeModePkg *pkg = static_cast<SafeModePkg *>(data);
    if (pkg->head_.msg_type_ == 20 || pkg->head_.msg_type_ == 24) {
        return 0;
    }
    if (pkg->head_.msg_type_ != 22) {
        std::string message(
            "In CTssSdkSafeModePlugin::notify, unknown msg type.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x496a4;  // 300708
        return error_info_.last_error_info()->error_code;
    }

    SafeModeAppendParam &append = pkg->body_.append_param_;

    // Every key and value must be a non-empty C string shorter than
    // TSS_SDK_SAFEMODE_PLUGIN_PARAM_MAX_LEN (128).
    for (unsigned int i = 0; i < append.param_count_; ++i) {
        const char *key = append.param_[i].param_key_;
        const char *value = append.param_[i].param_value_;
        size_t key_len = strlen(key);
        size_t value_len = strlen(value);
        if (key_len == 0 || key_len >= 0x80 ||
            value_len == 0 || value_len >= 0x80) {
            std::string message(
                "In CTssSdkSafeModePlugin::notify, len >= "
                "TSS_SDK_SAFEMODE_PLUGIN_PARAM_MAX_LEN || len == 0.");
            error_info_.set_last_error_msg(message.c_str());
            error_info_.last_error_info()->error_code = 0x4969d;  // 300701
            return error_info_.last_error_info()->error_code;
        }
    }

    std::string pre_encode_buf("");
    std::string url(append.url_);
    size_t seckey_pos =
        url.find(TSS_SDK_SAFEMODE_PLUGIN_PROMPT_PARAM_KEY, 0, 6);
    size_t erase_len = static_cast<size_t>(-1);

    // ORIG leaves this scratch buffer uninitialised when no seckey parameter
    // exists; the first strstr() in that branch reads the stack image.
    char encoded_value_buf[0x400];
    unsigned int used = 0;

    if (seckey_pos != std::string::npos) {
        size_t equal_pos = url.find("=", seckey_pos, 1);
        if (equal_pos == std::string::npos) {
            std::string message(
                "In CTssSdkSafeModePlugin::notify, not find =.");
            error_info_.set_last_error_msg(message.c_str());
            error_info_.last_error_info()->error_code = 0x4969d;  // 300701
            return error_info_.last_error_info()->error_code;
        }

        size_t amp_pos = url.find("&", equal_pos, 1);
        if (amp_pos == std::string::npos) {
            if (url[equal_pos + 1] == 0) {
                std::string message(
                    "In CTssSdkSafeModePlugin::notify, NULL after =.");
                error_info_.set_last_error_msg(message.c_str());
                error_info_.last_error_info()->error_code = 0x4969d;
                return error_info_.last_error_info()->error_code;
            }
            pre_encode_buf = url.substr(equal_pos + 1, std::string::npos);
            erase_len = url.size() - 1;
        } else {
            if (url[equal_pos + 1] == '&') {
                std::string message(
                    "In CTssSdkSafeModePlugin::notify, & after =.");
                error_info_.set_last_error_msg(message.c_str());
                error_info_.last_error_info()->error_code = 0x4969d;
                return error_info_.last_error_info()->error_code;
            }
            // This is the exact shipped substring extent: the third argument
            // is amp_pos - 1, not amp_pos - equal_pos - 1.
            pre_encode_buf = url.substr(equal_pos + 1, amp_pos - 1);
            erase_len = amp_pos;
        }

        unsigned int decoded_len = 0x400;
        int ret = base64_decode(
            pre_encode_buf.data(), pre_encode_buf.size(),
            reinterpret_cast<unsigned char *>(encoded_value_buf),
            &decoded_len);
        if (ret != 0) {
            std::string message(
                "In CTssSdkSafeModePlugin::notify, base64_decode error.");
            error_info_.set_last_error_msg(message.c_str());
            error_info_.last_error_info()->error_code = 0x496a3;  // 300707
            return error_info_.last_error_info()->error_code;
        }

        TenTean(reinterpret_cast<unsigned char *>(
                    const_cast<char *>(
                        TSS_SDK_SAFEMODE_PLUGIN_ENCODE_DECODE_KEY)),
                false,
                reinterpret_cast<unsigned char *>(encoded_value_buf),
                decoded_len);
        encoded_value_buf[decoded_len] = '&';
        ++decoded_len;
        encoded_value_buf[decoded_len] = 0;
        used = decoded_len;
    }

    // Append every parameter that is not already present in the scratch
    // buffer, base64-encoding only the value.
    for (unsigned int i = 0; i < append.param_count_; ++i) {
        const char *key = append.param_[i].param_key_;
        const char *value = append.param_[i].param_value_;
        if (strstr(encoded_value_buf, key) != 0) {
            continue;
        }

        size_t key_len = strlen(key);
        memcpy(encoded_value_buf + used, key, key_len);
        used += static_cast<unsigned int>(key_len);
        encoded_value_buf[used++] = '=';

        char value_b64[0x80];
        unsigned int value_b64_len = 0x80;
        int ret = base64_encode(
            reinterpret_cast<const unsigned char *>(value),
            static_cast<unsigned int>(strlen(value)),
            value_b64, &value_b64_len);
        if (ret != 0) {
            std::string message(
                "In CTssSdkSafeModePlugin::notify, encoded_value_buf "
                "base64_encode error.");
            error_info_.set_last_error_msg(message.c_str());
            error_info_.last_error_info()->error_code = 0x496a1;  // 300705
            return error_info_.last_error_info()->error_code;
        }

        memcpy(encoded_value_buf + used, value_b64, value_b64_len);
        used += value_b64_len;
        encoded_value_buf[used++] = '&';
        encoded_value_buf[used] = 0;
    }

    if (encoded_value_buf[used - 1] == '&') {
        --used;
        encoded_value_buf[used] = 0;
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "pre_encode_buf after add param: %s\n", encoded_value_buf);

    TenTean(reinterpret_cast<unsigned char *>(
                const_cast<char *>(
                    TSS_SDK_SAFEMODE_PLUGIN_ENCODE_DECODE_KEY)),
            true,
            reinterpret_cast<unsigned char *>(encoded_value_buf), used);

    char url_b64[0x400];
    unsigned int url_b64_len = 0x400;
    int ret = base64_encode(
        reinterpret_cast<const unsigned char *>(encoded_value_buf), used,
        url_b64, &url_b64_len);
    if (ret != 0) {
        std::string message(
            "In CTssSdkSafeModePlugin::notify, base64_encode error.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x496a1;  // 300705
        return error_info_.last_error_info()->error_code;
    }
    url_b64[url_b64_len] = 0;

    if (seckey_pos != std::string::npos) {
        size_t erase_pos =
            (url.at(seckey_pos - 1) == '&') ? seckey_pos - 1 : seckey_pos;
        url.erase(erase_pos, erase_len);
    }

    url.append("&", 1);
    url.append(TSS_SDK_SAFEMODE_PLUGIN_PROMPT_PARAM_KEY, 6);
    url.append("=", 1);
    url.append(url_b64, strlen(url_b64));

    if (url.size() >= append.url_buf_max_len_) {
        std::string message(
            "In CTssSdkSafeModePlugin::notify, pkg->url_buf_max_len_ error.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x496a2;  // 300706
        return error_info_.last_error_info()->error_code;
    }

    memcpy(append.url_, url.data(), url.size());
    append.url_[url.size()] = 0;
    return 0;
}

// ---------------------------------------------------------------------------
// proc: channel-plugin frames.
// ---------------------------------------------------------------------------

int TssSdkSafeModePlugin::proc(void *data) {
    if (data == 0) {
        std::string message(
            "In CTssSdkSafeModePlugin::proc, proc_data is NULL.");
        error_info_.set_last_error_msg(message.c_str());
        error_info_.last_error_info()->error_code = 0x4969d;  // 300701
        return error_info_.last_error_info()->error_code;
    }

    Comm_App_Frame *frame = static_cast<Comm_App_Frame *>(data);

    if (frame->frame_command_ == 0x493ec) {  // 300012
        sec_proto::SafemodeStatusRsp status;
        int ret = frame->appdata_decode(status, 0, 0);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "unpack safemode status pkg failed,uin=%u",
                frame->frame_uin_);
            return ret;
        }

        SafeModePkg out;
        out.head_.msg_type_ = 18;
        out.head_.uin_ = frame->frame_uin_;
        out.body_.safemode_update_.ipc_days_ = status.ipc_days_;
        out.body_.safemode_update_.safemode_level_ =
            static_cast<signed char>(status.stat_level_);
        out.body_.safemode_update_.has_verify_mb_ =
            static_cast<signed char>(status.has_verify_mb_);
        out.body_.safemode_update_.event_type_ = status.event_type_;
        memcpy(out.body_.safemode_update_.reserve_, status.reserve_,
               sizeof(status.reserve_));

        callback_proc_(&out);

        UserInfo *user = ::Singleton<TssSdkCommSvc>::instance()->
            get_user_info_by_uin(frame->frame_uin_);
        if (user == 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "in RSP_TSSSDK_SAFEMODE_STATUS, user is not exist, "
                "uin = %u",
                frame->frame_uin_);
            return 0;
        }

        if (dp_if_->on_recv_msg(user, frame) < 0) {
            std::string message(
                "In CTssSdkSafeModePlugin::proc, update safe mode level "
                "failed.");
            error_info_.set_last_error_msg(message.c_str());
            error_info_.last_error_info()->error_code = 0x4969e;  // 300702
            return error_info_.last_error_info()->error_code;
        }
        return 0;
    }

    if (frame->frame_command_ == 0x493ee) {  // 300014
        sec_proto::SafemodeLimitStatusRsp limit;
        int ret = frame->appdata_decode(limit, 0, 0);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "unpack safemode status pkg failed,uin=%u",
                frame->frame_uin_);
            return ret;
        }

        SafeModePkg out;
        out.head_.msg_type_ = 21;
        out.head_.uin_ = frame->frame_uin_;
        out.body_.limit_status_.ret_ = limit.ret_;
        out.body_.limit_status_.uin_ = limit.uin_;
        out.body_.limit_status_.limit_status_ = limit.limit_status_;
        out.body_.limit_status_.unlock_url_len_ = limit.unlock_url_len_;
        out.body_.limit_status_.unlock_url_ =
            reinterpret_cast<char *>(limit.unlock_url_);
        limit.unlock_url_[limit.unlock_url_len_] = 0;

        ZEN_Trace_LogMsg::debug_debugEx(
            "\n--- unlock_url_: %s ---\n",
            out.body_.limit_status_.unlock_url_);

        std::string url(out.body_.limit_status_.unlock_url_);
        size_t pos = url.find("?", 0, 1);
        std::string param_str(url, pos + 1, std::string::npos);
        ZEN_Trace_LogMsg::debug_debugEx("--- param_str: %s ---\n",
                                        param_str.c_str());

        char param_b64[0x400];
        unsigned int param_b64_len = 0x400;
        ret = base64_encode(
            reinterpret_cast<const unsigned char *>(param_str.data()),
            static_cast<unsigned int>(param_str.size()),
            param_b64, &param_b64_len);
        if (ret != 0) {
            std::string message(
                "In CTssSdkSafeModePlugin::proc, base64_encode error.");
            error_info_.set_last_error_msg(message.c_str());
            error_info_.last_error_info()->error_code = 0x496a1;  // 300705
            return error_info_.last_error_info()->error_code;
        }
        param_b64[param_b64_len] = 0;

        std::string new_url(url, 0, pos);
        new_url.append(TSS_SDK_SAFEMODE_PLUGIN_VERIFY_CGI, 39);
        new_url.append(TSS_SDK_SAFEMODE_PLUGIN_PROMPT_PARAM_KEY, 6);
        new_url.append("=", 1);
        new_url.append(param_b64, strlen(param_b64));

        if (new_url.size() >= 0x400) {
            std::string message(
                "In CTssSdkSafeModePlugin::proc, len >= "
                "TSS_SDK_SAFEMODE_PLUGIN_URL_MAX_LEN.");
            error_info_.set_last_error_msg(message.c_str());
            error_info_.last_error_info()->error_code = 0x496a2;  // 300706
            return error_info_.last_error_info()->error_code;
        }

        memcpy(out.body_.limit_status_.unlock_url_, new_url.data(),
               new_url.size());
        out.body_.limit_status_.unlock_url_len_ =
            static_cast<unsigned int>(new_url.size());
        out.body_.limit_status_.unlock_url_[new_url.size()] = 0;

        callback_proc_(&out);
        return 0;
    }

    ZEN_Trace_LogMsg::debug_errorex(
        "safemode plugin: recv unknow cmd=%d", frame->frame_command_);
    return 0;
}

}  // namespace tss_sdk_impl

// ORIG static-init block, part 2 (definition above the first use): the weak
// object_creator template static member (ORIG .bss 0x0029d800).
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
