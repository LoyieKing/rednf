// Semantic reconstruction of tss_dpsdk::DPVMBaseEngine from
// libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.x, DWARF present).
//
// Original translation unit:
//   src/formmog/tss_sdk/plugins/proto_proc/dp_vmbase_engine.cpp
// Original header:
//   src/formmog/tss_sdk/plugins/proto_proc/dp_vmbase_engine.h (line 28)
//
// ORIG ranges covered here (all addresses are virtual addresses in the
// original shared object):
//   DPVMBaseEngine::get_server_code          0x000bce50 - 0x000bcf6f
//   DPVMBaseEngine::get_client_code          0x000bcf70 - 0x000bd08f
//   DPVMBaseEngine::init_encrypt_data        0x000bd130 - 0x000bd1cf
//   DPVMBaseEngine::reload                   0x000bd1d0 - 0x000bd1ff
//   DPVMBaseEngine::init                     0x000bd200 - 0x000bd2bf
//   DPVMBaseEngine::~DPVMBaseEngine (D2/D1)  0x000bd2c0 - 0x000bd31f
//                                            0x000bd320 - 0x000bd37f
//   DPVMBaseEngine::DPVMBaseEngine (C1/C2)   0x000bd380 - 0x000bd41f
//                                            0x000bd420 - 0x000bd4bf
//   DPVMBaseEngine::fix_encrypt              0x000bd4c0 - 0x000bd88f
//   DPVMBaseEngine::check_decrypt_buf        0x000bd890 - 0x000bdd6f
//   DPVMBaseEngine::dynamicy_decrypt         0x000bdd70 - 0x000be09f
//   DPVMBaseEngine::fix_decrypt              0x000be0a0 - 0x000be26f
//   DPVMBaseEngine::decrypt                  0x000be270 - 0x000be35f
//   DPVMBaseEngine::dynamic_encrypt          0x000be360 - 0x000be81f
//   DPVMBaseEngine::encrypt                  0x000be820 - 0x000bea4f
//
// The class layout, code-table layout, UserInfo offsets, sec_proto wire
// structures and DPConfig offsets below are the exact DWARF layouts from the
// original image.  CVMBase, DPCodeMgr's file-backed methods, the VM factories,
// crypto, billing and logging are external boundaries: this file models only
// the parts needed to reconstruct DPVMBaseEngine and leaves the real
// implementations to their own translation units / the original library.

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Exact small layouts used by the original DPVMBaseEngine methods.
// ---------------------------------------------------------------------------

// Single authoritative logging boundary (zen_trace_log_basic.h +
// zen_trace_log_msg.h).  The local enum/struct stubs this TU used to carry are
// gone; every call site below is unchanged.
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/origsrc/formmog_proto_tss_sdk_client.h"

unsigned int Crc32(unsigned char *buf, unsigned int len);

// ORIG .rodata 0x0021d450, const unsigned int (12345678 decimal).
const unsigned int DEFAULT_USR_ID = 0x00bc614eu;

// ORIG .bss 0x002c1c80: Crc32 of the four little-endian bytes of
// DEFAULT_USR_ID, computed at load time (ORIG 0x000bed93..0x000beda9).
// ORIG binding is local (`nm`: lowercase b); keep internal linkage so the
// value cannot be interposed by another shared object.
static unsigned int default_user_id_crc =
    Crc32(const_cast<unsigned char *>(
              reinterpret_cast<const unsigned char *>(&DEFAULT_USR_ID)),
          sizeof(DEFAULT_USR_ID));

#include "src/formmog/tss_sdk/layout/user_info.h"



// ---------------------------------------------------------------------------
// CVMBase (src/VMCore/VM.h, DWARF byte_size 8: vptr + int VMError).
// Only the vtable slot order matters here.  CUPVM's vtable at 0x00293988
// confirms the order used below.
// ---------------------------------------------------------------------------
class CVMBase {
public:
    virtual int Init();                                             // +0x00
    virtual int Release();                                          // +0x04
    virtual int GetDecodeCode(unsigned char *, unsigned char *&,
                              unsigned short &);                    // +0x08
    virtual int GetEncodeCode(unsigned char *, unsigned char *&,
                              unsigned short &);                    // +0x0c
    virtual int GetHeader(unsigned char *, unsigned short *,
                          unsigned short *, unsigned short *);      // +0x10
    virtual int SetHeader(unsigned char *, unsigned short *,
                          unsigned short *, unsigned short *);      // +0x14
    virtual int GetDataBuffer(unsigned char *&, unsigned int &);    // +0x18
    virtual int SetKeyTree(void *, unsigned int);                   // +0x1c
    virtual int SetKeyMatrix(void *, unsigned int, unsigned int);   // +0x20
    virtual int SetKeyLink(void *, unsigned int);                   // +0x24
    virtual int Run();                                              // +0x28
    virtual int Load(void *, size_t, void *, size_t);               // +0x2c
    virtual int GetLastError();                                     // +0x30
    virtual int SetError(int);                                      // +0x34

private:
    int VMError_; // +0x04
};

// ORIG factories: GetVM @ 0x00196ea0, GetUPVM @ 0x00195120.
CVMBase *GetVM();
CVMBase *GetUPVM();

// ---------------------------------------------------------------------------
// ZEN_Thread_Mutex: canonical tag-394 model
// (src/commlib/zenlib/zen_lock_thread_mutex.h, 28 bytes: ZEN_Lock_Base +
// pthread_mutex_t).  The local opaque model this TU used to carry is gone.
// ---------------------------------------------------------------------------
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"

// CommBillRecord is a global class in the original image (mangled
// _ZN14CommBillRecordlsEx); DPBillRecorder (tss_dpsdk) derives from it.
#include "src/formmog/tss_sdk/layout/dp_bill_recorder.h"

namespace tss_dpsdk {

// ---------------------------------------------------------------------------
// DPCodeMgr and its nested code tables (DWARF: dp_code_mgr.h).
//
// DpCodeInfos is the shared-memory block returned by DPCodeMgr::init()
// (0x00928f48 bytes).  code_infos_[0] starts at DpCodeInfos+8; each
// DpCodeInfo is 0x004947a0 bytes.  CodeItems<N> always has 100 entries;
// N is the size of each CodeItem::code_buf_.
// ---------------------------------------------------------------------------
class DPCodeMgr {
public:
    template <unsigned int N>
    struct CodeItem {
        unsigned short weight_;      // +0
        unsigned short code_len_;    // +2
        unsigned char code_buf_[N];  // +4
    };

    template <unsigned int N>
    struct CodeItems {
        unsigned short total_weight_; // +0
        CodeItem<N> items_[100];      // +2

        unsigned char *get_code_buf(unsigned short code_id,
                                    unsigned short &len)
        {
            // ORIG inlined check: (code_id - 1) > 98 => reject.
            if (code_id == 0 || code_id > 99)
                return 0;
            len = items_[code_id].code_len_;
            return items_[code_id].code_buf_;
        }
    };

    struct CodeInfo {
        CodeItems<2000> cryptdata_code_;  // +0, 200402 bytes
        CodeItems<2000> crypt_code_;      // +200402
        CodeItems<20000> bin_file_;       // +400804, 2000402 bytes

        unsigned char *get_bin_code_buf(unsigned short code_id,
                                        unsigned short &len)
        {
            // ORIG inline chain: CodeInfo::get_bin_code_buf ->
            // CodeItems<2000>::get_code_buf on cryptdata_code_.
            return cryptdata_code_.get_code_buf(code_id, len);
        }
    };

    struct DpCodeInfo {
        unsigned short version_count_;        // +0
        unsigned int version_support_list_[8];// +4
        unsigned char version_hash_[16];      // +36
        CodeInfo client_code_;                // +52
        CodeInfo server_code_;                // +2401258

        int is_support(unsigned int version)
        {
            if (version_count_ == 0)
                return 0;
            for (int i = 0; i < (int)version_count_; ++i) {
                if (version_support_list_[i] == version ||
                    version_support_list_[i] == 0)
                    return 1;
            }
            return 0;
        }
    };

    struct DpCodeInfos {
        unsigned int code_update_time_; // +0
        unsigned short code_count_;     // +4
        DpCodeInfo code_infos_[2];      // +8

        DpCodeInfo *get_code_info(unsigned int version)
        {
            if (code_count_ == 0)
                return 0;
            for (int i = 0; i < (int)code_count_; ++i) {
                if (code_infos_[i].is_support(version))
                    return &code_infos_[i];
            }
            return 0;
        }
    };

    DPCodeMgr() : code_infos_(0) {}

    // External ORIG 0x000a7a70 and 0x000a79f0; not reconstructed here.
    int init();
    int reload();

    unsigned char *get_server_code(unsigned int version,
                                   unsigned short code_id,
                                   unsigned short &len)
    {
        DpCodeInfo *info = code_infos_->get_code_info(version);
        if (info == 0)
            return 0;
        // ORIG 0x000bcee2..0x000bcf66: >99 is remapped by subtracting 100
        // before the inlined CodeItems bound check.
        if (code_id > 99)
            return info->server_code_.get_bin_code_buf(code_id - 100, len);
        return info->server_code_.get_bin_code_buf(code_id, len);
    }

    unsigned char *get_client_code(unsigned int version,
                                   unsigned short code_id,
                                   unsigned short &len)
    {
        DpCodeInfo *info = code_infos_->get_code_info(version);
        if (info == 0)
            return 0;
        if (code_id > 99)
            return info->client_code_.get_bin_code_buf(code_id - 100, len);
        return info->client_code_.get_bin_code_buf(code_id, len);
    }

private:
    DpCodeInfos *code_infos_; // +0
};

// ---------------------------------------------------------------------------
// Billing boundary.  The original DPBillAdapter has exactly one member
// (UserInfo *user_ at +0) and writes records through the static DPBillRecorder
// bill_ (ORIG 0x002b6e00), whose inherited CommBillRecord::operator<<(long long)
// does the actual formatting.
// ---------------------------------------------------------------------------

class DPBillAdapter {
public:
    DPBillAdapter(const char *msg, unsigned int type,
                  tss_sdk_impl::UserInfo *user);
    ~DPBillAdapter();

    static DPBillRecorder bill_;

private:
    tss_sdk_impl::UserInfo *user_; // +0
};

class DPMsgPacker {
public:
    static int send_error_report_msg(tss_sdk_impl::UserInfo *user,
                                     int msg_type, int flag);
};

// ---------------------------------------------------------------------------
// DPVMBaseEngine (DWARF byte_size 68 / 0x44, dp_vmbase_engine.h:28).
// Note: the destructor does NOT delete encrypt_vm_/decrypt_vm_/code_manager_.
// The two mutexes are constructed but the reconstructed methods do not lock
// them (matching the original bodies).
// ---------------------------------------------------------------------------
class DPVMBaseEngine {
public:
    DPVMBaseEngine();
    ~DPVMBaseEngine();

    int init();
    int reload();

    unsigned char *get_server_code(tss_sdk_impl::UserInfo *user,
                                   unsigned short code_id,
                                   unsigned short &buf_len);
    unsigned char *get_client_code(tss_sdk_impl::UserInfo *user,
                                   unsigned short code_id,
                                   unsigned short &buf_len);

    void init_encrypt_data(tss_sdk_impl::UserInfo *user,
                           sec_proto::DPAlgorithmData &algorithm_data,
                           unsigned int custom_data);

    int fix_encrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                    unsigned int buf_len, unsigned char *out,
                    unsigned int &out_len);
    int dynamic_encrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                        unsigned int buf_len, unsigned char *out,
                        unsigned int &out_len);
    int check_decrypt_buf(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                          unsigned int buf_len, unsigned char *out,
                          unsigned int &out_len, unsigned int crc,
                          int err_type, int is_internal_pkg);
    int fix_decrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                    unsigned int buf_len, unsigned char *out,
                    unsigned int &out_len, int is_internal_pkg);
    int dynamicy_decrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                         unsigned int buf_len, unsigned char *out,
                         unsigned int &out_len, int is_internal_pkg);

    int decrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                unsigned int buf_len, unsigned char *out,
                unsigned int &out_len, int pkg_type,
                int is_internal_pkg);
    int encrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                unsigned int buf_len, unsigned char *out,
                unsigned int &out_len, bool use_fix_encrypt);

private:
    CVMBase *encrypt_vm_;                    // +0x00
    CVMBase *decrypt_vm_;                    // +0x04
    DPCodeMgr *code_manager_;                // +0x08
    ZEN_Thread_Mutex encrypt_vm_thread_mutex_;// +0x0c
    ZEN_Thread_Mutex decrypt_vm_thread_mutex_;// +0x28
};


}  // namespace tss_dpsdk

namespace tss_dp_comm {

struct DPConfigBase {
    unsigned char is_use_dp;                 // +0
    unsigned char is_restore_usr_space;      // +1
    unsigned char is_use_client_version;     // +2
    unsigned char disable_anticheat_pkg;     // +3
    unsigned char use_punish_notify;         // +4
    unsigned char dp_record_using_time;      // +5
};

struct DPProtoConfig {
    unsigned short dp_fragmentation_size;    // +6 in DPConfig
    unsigned short dp_max_encrypt_size;      // +8 in DPConfig
    unsigned char use_sequence_check;        // +10 in DPConfig
    unsigned char proto_check_switch;        // +11
    unsigned char seq_check_switch;          // +12
    unsigned char raw_check_switch;          // +13
    unsigned char dp_decrypt_fail_protect;   // +14
    unsigned char explicit_use_dynamic_encrypt; // +15
};

struct DPConfig {
    DPConfigBase dp_base_conf;  // +0
    DPProtoConfig dp_proto_conf;// +6
};

struct DP_CONFIG {
    static DPConfig *glb_dp_config_; // ORIG 0x002bb4c0, 4-byte pointer
};

}  // namespace tss_dp_comm

namespace AB_NS_CODEENGINE {
int EncodeInt(char **buf, unsigned int value);
}

// Crypto boundary (src/TenCrypt.cpp).
int Cipher(int algorithm_type, unsigned char *buf, unsigned int len,
           unsigned int key_id, bool encrypt);

// Exact-size layout checks against the DWARF in the original image.
#define RECON_STATIC_ASSERT(cond, name) \
    typedef char recon_static_assert_##name[(cond) ? 1 : -1]

RECON_STATIC_ASSERT(sizeof(tss_sdk_impl::UserInfo) == 400, user_info_size);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo, base_info_) == 0x00,
                    user_base);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             base_info_.client_version_) == 0x18,
                    user_client_version);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             base_info_.client_user_id_) == 0x1c,
                    user_client_user_id);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             base_info_.flags_) == 0x20,
                    user_flags);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_code_info_.client_state_) == 0x64,
                    user_client_state);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_code_info_.last_encrypt_sequence_) == 0x68,
                    user_last_encrypt_seq);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_code_info_.last_game_decrypt_sequence_) == 0x6c,
                    user_last_game_decrypt_seq);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_code_info_.last_internal_decrypt_sequence_) == 0x70,
                    user_last_internal_decrypt_seq);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_code_info_.server_cur_code_) == 0x78,
                    user_server_cur_code);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_code_info_.server_new_code_) == 0x7a,
                    user_server_new_code);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_code_info_.client_code_) == 0x7c,
                    user_client_code);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_stat_.fix_encrypt_key_) == 0xac,
                    user_fix_encrypt_key);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_stat_.dynamic_encrypt_count_) == 0xb8,
                    user_dynamic_encrypt_count);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_stat_.internal_fix_decrypt_count_) == 0xbc,
                    user_internal_fix_decrypt_count);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_stat_.game_fix_decrypt_count_) == 0xc0,
                    user_game_fix_decrypt_count);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_stat_.internal_dynamic_decrypt_count_) == 0xc4,
                    user_internal_dynamic_decrypt_count);
RECON_STATIC_ASSERT(offsetof(tss_sdk_impl::UserInfo,
                             user_stat_.game_dynamic_decrypt_count_) == 0xc8,
                    user_game_dynamic_decrypt_count);

RECON_STATIC_ASSERT(sizeof(sec_proto::DPInterDataHead) == 5, inter_head_size);
RECON_STATIC_ASSERT(sizeof(sec_proto::DPFixAlgorithmHead) == 12, fix_head_size);
RECON_STATIC_ASSERT(sizeof(sec_proto::DPDynamicAlgorithmHead) == 8,
                    dynamic_head_size);
RECON_STATIC_ASSERT(sizeof(sec_proto::DPAlgorithmData) == 265,
                    algorithm_data_size);
RECON_STATIC_ASSERT(offsetof(sec_proto::DPAlgorithmData, usr_id_crc_) == 0,
                    algo_usr_id_crc);
RECON_STATIC_ASSERT(offsetof(sec_proto::DPAlgorithmData, custom_data_) == 4,
                    algo_custom_data);
RECON_STATIC_ASSERT(offsetof(sec_proto::DPAlgorithmData, pad_len_) == 8,
                    algo_pad_len);
RECON_STATIC_ASSERT(offsetof(sec_proto::DPAlgorithmData, pad_data_) == 9,
                    algo_pad_data);
RECON_STATIC_ASSERT(sizeof(sec_proto::DPInterDataBody) == 1580, body_size);
RECON_STATIC_ASSERT(sizeof(sec_proto::DPInterData) == 1585, inter_data_size);
RECON_STATIC_ASSERT(offsetof(sec_proto::DPInterData, head) == 0, inter_head_off);
RECON_STATIC_ASSERT(offsetof(sec_proto::DPInterData, body) == 5, inter_body_off);

RECON_STATIC_ASSERT(sizeof(tss_dpsdk::DPCodeMgr) == 4, code_mgr_size);
RECON_STATIC_ASSERT(sizeof(tss_dpsdk::DPCodeMgr::CodeItems<2000>) == 200402,
                    code_items_2000);
RECON_STATIC_ASSERT(sizeof(tss_dpsdk::DPCodeMgr::CodeItems<20000>) == 2000402,
                    code_items_20000);
RECON_STATIC_ASSERT(sizeof(tss_dpsdk::DPCodeMgr::CodeInfo) == 2401206,
                    code_info_size);
RECON_STATIC_ASSERT(offsetof(tss_dpsdk::DPCodeMgr::CodeInfo, cryptdata_code_) == 0,
                    code_info_cryptdata);
RECON_STATIC_ASSERT(offsetof(tss_dpsdk::DPCodeMgr::CodeInfo, crypt_code_) == 200402,
                    code_info_crypt);
RECON_STATIC_ASSERT(offsetof(tss_dpsdk::DPCodeMgr::CodeInfo, bin_file_) == 400804,
                    code_info_bin);
RECON_STATIC_ASSERT(sizeof(tss_dpsdk::DPCodeMgr::DpCodeInfo) == 4802464,
                    dp_code_info_size);
RECON_STATIC_ASSERT(offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfo,
                             version_support_list_) == 4,
                    dp_code_info_versions);
RECON_STATIC_ASSERT(offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfo,
                             version_hash_) == 36,
                    dp_code_info_hash);
RECON_STATIC_ASSERT(offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfo,
                             client_code_) == 52,
                    dp_code_info_client);
RECON_STATIC_ASSERT(offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfo,
                             server_code_) == 2401258,
                    dp_code_info_server);
RECON_STATIC_ASSERT(sizeof(tss_dpsdk::DPCodeMgr::DpCodeInfos) == 9604936,
                    dp_code_infos_size);
RECON_STATIC_ASSERT(offsetof(tss_dpsdk::DPCodeMgr::DpCodeInfos,
                             code_infos_) == 8,
                    dp_code_infos_array);

RECON_STATIC_ASSERT(sizeof(::ZEN_Thread_Mutex) == 28, mutex_size);
RECON_STATIC_ASSERT(sizeof(tss_dpsdk::DPVMBaseEngine) == 68, engine_size);
RECON_STATIC_ASSERT(offsetof(tss_dp_comm::DPConfig, dp_proto_conf) == 6,
                    dp_config_proto);
RECON_STATIC_ASSERT(offsetof(tss_dp_comm::DPConfig,
                             dp_proto_conf.dp_max_encrypt_size) == 8,
                    dp_config_max_encrypt);
RECON_STATIC_ASSERT(offsetof(tss_dp_comm::DPConfig,
                             dp_proto_conf.use_sequence_check) == 10,
                    dp_config_sequence_check);
RECON_STATIC_ASSERT(offsetof(tss_dp_comm::DPConfig,
                             dp_proto_conf.explicit_use_dynamic_encrypt) == 15,
                    dp_config_dynamic_encrypt);

namespace {

inline ZEN_LOG_PRIORITY log_priority(const tss_sdk_impl::UserInfo *user)
{
    if (user != 0 && (user->base_info_.flags_ & 1) == 1)
        return ZEN_LOG_PRIORITY_LEVEL_3;
    return ZEN_LOG_PRIORITY_LEVEL_2;
}

inline unsigned int log_uin(const tss_sdk_impl::UserInfo *user)
{
    return user != 0 ? user->base_info_.uin_ : 0;
}

inline unsigned int rand32()
{
    // ORIG emits: (rand() << 16) + rand().
    return ((unsigned int)rand() << 16) + (unsigned int)rand();
}

}  // namespace

namespace tss_dpsdk {

// ---------------------------------------------------------------------------
// ORIG 0x000bce50: get_server_code(UserInfo*, u16 code_id, u16& len)
//   version = user->base_info_.client_version_ (UserInfo+0x18)
//   code_data = code_manager_->get_server_code(version, code_id, len)
//   if (!code_data) return 0
//   return decrypt_vm_->GetEncodeCode(code_data, buf, len) ? buf : 0
// The VM call is vtable slot +0x0c (GetEncodeCode) on decrypt_vm_ (+0x04).
// ---------------------------------------------------------------------------
unsigned char *DPVMBaseEngine::get_server_code(tss_sdk_impl::UserInfo *user,
                                               unsigned short code_id,
                                               unsigned short &buf_len)
{
    unsigned char *buf = 0;
    unsigned char *code_data =
        code_manager_->get_server_code(user->base_info_.client_version_,
                                       code_id, buf_len);
    if (code_data == 0)
        return 0;
    if (decrypt_vm_->GetEncodeCode(code_data, buf, buf_len) != 0)
        return buf;
    return 0;
}

// ---------------------------------------------------------------------------
// ORIG 0x000bcf70: get_client_code(UserInfo*, u16 code_id, u16& len)
// Same structure, but code_manager_->get_client_code and encrypt_vm_
// vtable slot +0x08 (GetDecodeCode).
// ---------------------------------------------------------------------------
unsigned char *DPVMBaseEngine::get_client_code(tss_sdk_impl::UserInfo *user,
                                               unsigned short code_id,
                                               unsigned short &buf_len)
{
    unsigned char *buf = 0;
    unsigned char *code_data =
        code_manager_->get_client_code(user->base_info_.client_version_,
                                       code_id, buf_len);
    if (code_data == 0)
        return 0;
    if (encrypt_vm_->GetDecodeCode(code_data, buf, buf_len) != 0)
        return buf;
    return 0;
}

// ---------------------------------------------------------------------------
// ORIG 0x000bd130: init_encrypt_data(UserInfo*, DPAlgorithmData&, uint)
//   usr_id_crc_ = (user && user->client_state_ != ECS_NOT_INIT)
//                 ? user->base_info_.client_user_id_ : default_user_id_crc
//   pad_len_    = rand() % 16
//   for (i = 0; i < (pad_len_ >> 2) + 1; ++i)
//       ((int*)pad_data_)[i] = (rand() << 16) + rand()
//   custom_data_ = custom_data
// Note the 4-byte pad stores start at byte offset 9 (unaligned by design).
// ---------------------------------------------------------------------------
void DPVMBaseEngine::init_encrypt_data(
    tss_sdk_impl::UserInfo *user,
    sec_proto::DPAlgorithmData &algorithm_data,
    unsigned int custom_data)
{
    if (user == 0 ||
        user->user_code_info_.client_state_ == tss_sdk_impl::ECS_NOT_INIT) {
        algorithm_data.usr_id_crc_ = default_user_id_crc;
    } else {
        algorithm_data.usr_id_crc_ = user->base_info_.client_user_id_;
    }

    algorithm_data.pad_len_ = (unsigned char)(rand() % 16);
    int pad_count = (algorithm_data.pad_len_ >> 2) + 1;
    for (int i = 0; i < pad_count; ++i) {
        unsigned int value = rand32();
        memcpy(algorithm_data.pad_data_ + i * 4, &value, sizeof(value));
    }

    algorithm_data.custom_data_ = custom_data;
}

// ---------------------------------------------------------------------------
// ORIG 0x000bd1d0: return code_manager_->reload();
// ---------------------------------------------------------------------------
int DPVMBaseEngine::reload()
{
    return code_manager_->reload();
}

// ---------------------------------------------------------------------------
// ORIG 0x000bd200:
//   if (code_manager_->init() != 0) { log "DPCodeMgr init fail"; return -1; }
//   encrypt_vm_ = GetVM();
//   if (!encrypt_vm_ || encrypt_vm_->Init() == 0) {
//       log "Init encrypt_vm failed! %p"; return -1; }
//   decrypt_vm_ = GetUPVM();
//   if (!decrypt_vm_ || decrypt_vm_->Init() == 0) {
//       log "Init decrypt_vm failed! %p"; return -1; }
//   return 0;
// CVMBase::Init is vtable slot +0x00.  GetVM/GetUPVM are the original
// process-wide factories; the VMs are not released or deleted here.
// ---------------------------------------------------------------------------
int DPVMBaseEngine::init()
{
    if (code_manager_->init() != 0) {
        ZEN_Trace_LogMsg::debug_errorex("DPCodeMgr init fail");
        return -1;
    }

    encrypt_vm_ = GetVM();
    if (encrypt_vm_ == 0 || encrypt_vm_->Init() == 0) {
        ZEN_Trace_LogMsg::debug_errorex("Init encrypt_vm failed! %p",
                                        encrypt_vm_);
        return -1;
    }

    decrypt_vm_ = GetUPVM();
    if (decrypt_vm_ == 0 || decrypt_vm_->Init() == 0) {
        ZEN_Trace_LogMsg::debug_errorex("Init decrypt_vm failed! %p",
                                        decrypt_vm_);
        return -1;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// ORIG 0x000bd2c0 (D2) / 0x000bd320 (D1): both bodies are identical.
// Only the two ZEN_Thread_Mutex members are destroyed, in reverse declaration
// order (decrypt mutex at +0x28 first, then encrypt mutex at +0x0c).
// encrypt_vm_, decrypt_vm_ and code_manager_ are intentionally leaked, exactly
// as in the original.
// ---------------------------------------------------------------------------
DPVMBaseEngine::~DPVMBaseEngine()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x000bd380 (C1) / 0x000bd420 (C2): both bodies are identical.
//   encrypt_vm_ = 0; decrypt_vm_ = 0;
//   ZEN_Thread_Mutex(encrypt_vm_thread_mutex_, 0);
//   ZEN_Thread_Mutex(decrypt_vm_thread_mutex_, 0);
//   code_manager_ = new DPCodeMgr();        // 4-byte object, [0] = 0
// No other member is touched; in particular the constructor does not call
// DPCodeMgr::init().
// ---------------------------------------------------------------------------
DPVMBaseEngine::DPVMBaseEngine()
    : encrypt_vm_(0),
      decrypt_vm_(0),
      encrypt_vm_thread_mutex_(0),
      decrypt_vm_thread_mutex_(0)
{
    code_manager_ = new DPCodeMgr();
}

// ---------------------------------------------------------------------------
// ORIG 0x000bd4c0: fix_encrypt(UserInfo*, buf, buf_len, out, out_len&)
//
// Guard before any write: out_len < buf_len + 0x11a (17 + 265), matching
// ORIG 0x000bd4df..0x000bd4e6.  The 17-byte prefix is:
// 5-byte DPInterDataHead + 12-byte DPFixAlgorithmHead.
// Wire values:
//   head.protocol_type_ = 0x11, head.data_type_ = 0
//   head.packet_size_   = (u16)(payload_len + 17)  (set after packing)
//   body.fix_algorithm_data_.key_id_:
//       user  ? (user->user_stat_.fix_encrypt_key_ += (rand() >> 8) + 1)
//             : (rand() << 16) + rand()
//   body.fix_algorithm_data_.algorithm_type_ = rand() % 19 + 1
//   body.fix_algorithm_data_.crc = Crc32(dp_data, payload_len)
// Payload:
//   algorithm_data = init_encrypt_data(user, algorithm_data, 0)
//   algorithm_data.pack(dp_data, out_len - 17, &used, 0)
//   memcpy(dp_data + used, buf, buf_len); used += buf_len
//   Cipher(algorithm_type_, dp_data, used, key_id_, true); zero => bill/fail
// Final:
//   head.packet_size_ = (u16)(used + 17)
//   fix_encrypt.pack(out, out_len, 0, 0)
//   out_len = head.packet_size_
// ---------------------------------------------------------------------------
int DPVMBaseEngine::fix_encrypt(tss_sdk_impl::UserInfo *user,
                                unsigned char *buf, unsigned int buf_len,
                                unsigned char *out, unsigned int &out_len)
{
    const size_t head_buf_len = 17;

    // ORIG 0x000bd4df..0x000bd4e6: `add eax,0x11a; cmp edx,eax; jb fail`.
    // The guard reserves the whole maximum prefix (17-byte DPInterData +
    // 265-byte DPAlgorithmData), not just the 17-byte DPInterData head.
    // The failure log still reports head_buf_len=17 (0x000bd81b).
    if (out_len < buf_len + 0x11au) {
        // ORIG format string 0x0021d0e0.
        ZEN_Trace_LogMsg::debug_output(
            ZEN_LOG_PRIORITY_ERROR,
            "[%u][%s], Too small outsize, buf_len=%u out_len=%u, "
            "head_buf_len=%u",
            log_uin(user), __func__, buf_len, out_len,
            (unsigned int)head_buf_len);
        return -1;
    }

    sec_proto::DPInterData fix_encrypt;
    fix_encrypt.head.protocol_type_ = 0x11;
    fix_encrypt.head.data_type_ = 0;

    fix_encrypt.body.fix_algorithm_data_.algorithm_type_ =
        (unsigned int)(rand() % 19 + 1);

    if (user != 0) {
        unsigned int key = user->user_stat_.fix_encrypt_key_ +
                           (unsigned int)(rand() >> 8) + 1;
        user->user_stat_.fix_encrypt_key_ = key;
        fix_encrypt.body.fix_algorithm_data_.key_id_ = key;
    } else {
        fix_encrypt.body.fix_algorithm_data_.key_id_ = rand32();
    }

    const sec_proto::DPFixAlgorithmHead &fix_data =
        fix_encrypt.body.fix_algorithm_data_;
    unsigned char *dp_data =
        out + head_buf_len;  // ORIG: lea out+0x11

    sec_proto::DPAlgorithmData algorithm_data;
    init_encrypt_data(user, algorithm_data, 0);

    unsigned int used = 0;
    algorithm_data.pack((char *)dp_data,
                        (unsigned int)(out_len - head_buf_len),
                        &used, 0);
    memcpy(dp_data + used, buf, buf_len);
    used += buf_len;

    // ORIG writes through fix_encrypt.body.fix_algorithm_data_; fix_data is a
    // const view of the same object.
    fix_encrypt.body.fix_algorithm_data_.crc = Crc32(dp_data, used);

    ZEN_Trace_LogMsg::debug_output(
        log_priority(user),
        "[%u][%s], key_id=%u, algorithm_type_=%u, crc=%u, pad=%u",
        log_uin(user), __func__, fix_data.key_id_,
        fix_data.algorithm_type_, fix_data.crc, algorithm_data.pad_len_);

    const char *err_msg = "cipher encrypt fail";
    int cipher_ok = 0;
    try {
        cipher_ok = Cipher((int)fix_data.algorithm_type_, dp_data, used,
                           fix_data.key_id_, true);
    } catch (...) {
        err_msg = "cipher encrypt exception";
        cipher_ok = 0;
    }

    if (cipher_ok == 0) {
        DPBillAdapter bill(err_msg, 0x1d, user);
        DPBillAdapter::bill_ << (long long)fix_data.algorithm_type_
                             << (long long)fix_data.key_id_
                             << (long long)used;
        return -1;
    }

    fix_encrypt.head.packet_size_ =
        (unsigned short)(used + head_buf_len);
    fix_encrypt.pack((char *)out, out_len, 0, 0);
    out_len = fix_encrypt.head.packet_size_;

    ZEN_Trace_LogMsg::debug_output(
        log_priority(user), "[%u][%s], packet_size_=%u",
        log_uin(user), __func__, fix_encrypt.head.packet_size_);

    return 0;
}

// ---------------------------------------------------------------------------
// ORIG 0x000bd890: check_decrypt_buf(UserInfo*, buf, buf_len, out, out_len&,
//                                    crc, err_type, is_internal_pkg)
//
//   crc_value = Crc32(buf, buf_len)
//   log(priority, crc=%u, crc_value=%u, buf_len=%d)
//   if (crc != crc_value) {
//       bill("decrypt CRC fail", err_type) << crc << crc_value << buf_len;
//       if (user) send_error_report_msg(user, 0x1a1, 1);
//       return -1;
//   }
//   decrypt_data.unpack(buf, buf_len, &used, 0)
//   if (ret != 0 || buf_len <= used) { log(4, unpack fail...); return -1; }
//   remaining = buf_len - (u16)used;
//   if (user) {
//       log(priority, used, usr_id_crc_, custom_data_, pad)
//       if (client_state_ > 1 && client_user_id_ != usr_id_crc_) {
//           bill(err_type) << client_user_id_ << usr_id_crc_;
//           send_error_report_msg(user, 0x1a0, 1); return -1;
//       }
//       seq = is_internal_pkg ? last_internal_decrypt_sequence_
//                             : last_game_decrypt_sequence_;
//       if (seq < custom_data_ || seq == 0) { /* accept */ }
//       else if (glb_dp_config_->dp_proto_conf.use_sequence_check) {
//           bill(err_type) << is_internal_pkg << seq << custom_data_;
//           send_error_report_msg(user, 0x19f, 1); return -1;
//       }
//       seq = custom_data_;             // updated before the out_len check
//   } else {
//       log(2, used, usr_id_crc_, custom_data_, pad)
//       // no user-id and no sequence checks for a user-less package
//   }
//   if (out_len < remaining) { log(4, too small...); return -1; }
//   out_len = remaining;
//   memcpy(out, buf + used, remaining);
//   return 0;
// ---------------------------------------------------------------------------
int DPVMBaseEngine::check_decrypt_buf(tss_sdk_impl::UserInfo *user,
                                      unsigned char *buf,
                                      unsigned int buf_len,
                                      unsigned char *out,
                                      unsigned int &out_len,
                                      unsigned int crc, int err_type,
                                      int is_internal_pkg)
{
    unsigned int crc_value = Crc32(buf, buf_len);

    ZEN_Trace_LogMsg::debug_output(
        log_priority(user),
        "[%u][%s], crc=%u, crc_value=%u, buf_len=%d",
        log_uin(user), __func__, crc, crc_value, buf_len);

    if (crc != crc_value) {
        DPBillAdapter bill("decrypt CRC fail", (unsigned int)err_type, user);
        DPBillAdapter::bill_ << (long long)crc
                             << (long long)crc_value
                             << (long long)buf_len;
        if (user != 0)
            DPMsgPacker::send_error_report_msg(user, 0x1a1, 1);
        return -1;
    }

    size_t used = 0;
    sec_proto::DPAlgorithmData decrypt_data;
    int ret = decrypt_data.unpack((const char *)buf, buf_len,
                                  (unsigned int *)&used, 0);

    if (ret != 0 || buf_len <= used) {
        ZEN_Trace_LogMsg::debug_output(
            ZEN_LOG_PRIORITY_ERROR,
            "[%u][%s], DPAlgorithmData unpack fail, ret=%d, used=%d, "
            "pad=%d, buf_len=%d",
            log_uin(user), __func__, ret, (int)used,
            (int)decrypt_data.pad_len_, buf_len);
        return -1;
    }

    unsigned int remaining = buf_len - (unsigned short)used;
    unsigned char *dp_data = buf + used;

    if (user != 0) {
        ZEN_Trace_LogMsg::debug_output(
            log_priority(user),
            "[%u][%s], used=%d, usr_id_crc_=%d, custom_data_=%u, pad=%u",
            log_uin(user), __func__, (int)used,
            (int)decrypt_data.usr_id_crc_, decrypt_data.custom_data_,
            decrypt_data.pad_len_);

        if (user->user_code_info_.client_state_ >
                tss_sdk_impl::ECS_NOT_INIT &&
            user->base_info_.client_user_id_ != decrypt_data.usr_id_crc_) {
            DPBillAdapter bill("client user_id crc fail",
                               (unsigned int)err_type, user);
            DPBillAdapter::bill_ << (long long)user->base_info_.client_user_id_
                                 << (long long)decrypt_data.usr_id_crc_;
            DPMsgPacker::send_error_report_msg(user, 0x1a0, 1);
            return -1;
        }

        unsigned int &last_sequence =
            is_internal_pkg != 0
                ? user->user_code_info_.last_internal_decrypt_sequence_
                : user->user_code_info_.last_game_decrypt_sequence_;

        if (last_sequence < decrypt_data.custom_data_ ||
            last_sequence == 0) {
            // Accept a new/zero sequence.
        } else if (tss_dp_comm::DP_CONFIG::glb_dp_config_->
                       dp_proto_conf.use_sequence_check != 0) {
            DPBillAdapter bill("client sequence error",
                               (unsigned int)err_type, user);
            DPBillAdapter::bill_ << (long long)is_internal_pkg
                                 << (long long)last_sequence
                                 << (long long)decrypt_data.custom_data_;
            DPMsgPacker::send_error_report_msg(user, 0x19f, 1);
            return -1;
        }

        // ORIG stores the sequence before checking the output capacity.
        last_sequence = decrypt_data.custom_data_;
    } else {
        ZEN_Trace_LogMsg::debug_output(
            ZEN_LOG_PRIORITY_LEVEL_2,
            "[%u][%s], used=%d, usr_id_crc_=%d, custom_data_=%u, pad=%u",
            0, __func__, (int)used, (int)decrypt_data.usr_id_crc_,
            decrypt_data.custom_data_, decrypt_data.pad_len_);
    }

    if (out_len < remaining) {
        ZEN_Trace_LogMsg::debug_output(
            ZEN_LOG_PRIORITY_ERROR,
            "[%u][%s], too small out buf in decrypt, out=%d, buf_len=%d",
            log_uin(user), __func__, out_len, remaining);
        return -1;
    }

    out_len = remaining;
    memcpy(out, dp_data, remaining);
    return 0;
}

// ---------------------------------------------------------------------------
// ORIG 0x000bdd70: dynamicy_decrypt (spelling preserved)
//
//   DPDynamicAlgorithmHead.unpack(buf, buf_len, &used, 0)
//   if (fail || buf_len <= used) { log(4, ...); return -1; }
//   remaining = buf_len - (u16)used
//   if (dynamic_head.code_id_ not in user->client_code_[0..4]) {
//       bill("client use other encode id", 0x20)
//           << code_id_ << remaining;
//       send_error_report_msg(user, 0x193, 1); return -1;
//   }
//   code_buf = get_client_code(user, (u16)code_id_, &code_len)
//   log(priority, get_client_code, code_buf, code_len, code_id_)
//   if (!code_buf || !code_len) err = "get client code fail"
//   else decrypt_vm_->Load(code_buf, code_len, buf + used, remaining)
//        -> "decrypt load fail"; then Run() -> "decrypt run fail";
//        then GetDataBuffer(&data_buf, &data_len)
//   if (!data_buf || !data_len) { bill(err, 0x20) << code_id_ << remaining;
//                                  return -1; }
//   return check_decrypt_buf(user, data_buf, data_len, out, out_len,
//                            dynamic_head.crc_, 0x20, is_internal_pkg)
// The VM calls are vtable +0x2c (Load), +0x28 (Run), +0x18 (GetDataBuffer)
// on decrypt_vm_.  Any exception becomes err "decrypt Exception".
// ---------------------------------------------------------------------------
int DPVMBaseEngine::dynamicy_decrypt(tss_sdk_impl::UserInfo *user,
                                     unsigned char *buf,
                                     unsigned int buf_len,
                                     unsigned char *out,
                                     unsigned int &out_len,
                                     int is_internal_pkg)
{
    size_t used = 0;
    sec_proto::DPDynamicAlgorithmHead dynamic_head;
    int ret = dynamic_head.unpack((const char *)buf, buf_len,
                                  (unsigned int *)&used, 0);

    if (ret != 0 || buf_len <= used) {
        ZEN_Trace_LogMsg::debug_output(
            ZEN_LOG_PRIORITY_ERROR,
            "[%u][%s], DPDynamicAlgorithmHead unpack fail, buf_len=%d",
            log_uin(user), __func__, buf_len);
        return -1;
    }

    unsigned int remaining = buf_len - (unsigned short)used;
    unsigned char *dp_data = buf + used;

    int i = 0;
    for (; i < 5; ++i) {
        if ((unsigned int)user->user_code_info_.client_code_[i] ==
            dynamic_head.code_id_)
            break;
    }
    if (i == 5) {
        DPBillAdapter bill("client use other encode id", 0x20, user);
        DPBillAdapter::bill_ << (long long)dynamic_head.code_id_
                             << (long long)remaining;
        DPMsgPacker::send_error_report_msg(user, 0x193, 1);
        return -1;
    }

    unsigned short code_len = 0;
    unsigned int data_len = 0;
    unsigned char *data_buf = 0;
    unsigned char *code_buf =
        get_client_code(user, (unsigned short)dynamic_head.code_id_,
                        code_len);

    ZEN_Trace_LogMsg::debug_output(
        log_priority(user),
        "[%u][%s], get_client_code, code_buf=%p, len=%u, code_id=%u",
        log_uin(user), __func__, code_buf, code_len,
        dynamic_head.code_id_);

    const char *err_msg = "get client code fail";
    if (code_buf != 0 && code_len != 0) {
        try {
            if (decrypt_vm_->Load(code_buf, code_len, dp_data,
                                  remaining) != 0) {
                if (decrypt_vm_->Run() != 0) {
                    decrypt_vm_->GetDataBuffer(data_buf, data_len);
                    err_msg = "";
                } else {
                    err_msg = "decrypt run fail";
                }
            } else {
                err_msg = "decrypt load fail";
            }
        } catch (...) {
            err_msg = "decrypt Exception";
        }
    }

    if (data_buf == 0 || data_len == 0) {
        DPBillAdapter bill(err_msg, 0x20, user);
        DPBillAdapter::bill_ << (long long)dynamic_head.code_id_
                             << (long long)remaining;
        return -1;
    }

    return check_decrypt_buf(user, data_buf, data_len, out, out_len,
                             dynamic_head.crc_, 0x20, is_internal_pkg);
}

// ---------------------------------------------------------------------------
// ORIG 0x000be0a0: fix_decrypt(UserInfo*, buf, buf_len, out, out_len&,
//                              is_internal_pkg)
//
//   DPFixAlgorithmHead.unpack(buf, buf_len, &used, 0)
//   if (fail || buf_len <= used) { log(4, ...); return -1; }
//   remaining = buf_len - (u16)used; dp_data = buf + used
//   if (!Cipher(algorithm_type_, dp_data, remaining, key_id_, false)) {
//       bill("cipher decrypt fail", 0x1e)
//           << algorithm_type_ << key_id_ << remaining;
//       return -1;
//   }
//   return check_decrypt_buf(user, dp_data, remaining, out, out_len,
//                            fix_data.crc, 0x1e, is_internal_pkg);
// Cipher exception message is "cipher decrypt exception".
// ---------------------------------------------------------------------------
int DPVMBaseEngine::fix_decrypt(tss_sdk_impl::UserInfo *user,
                                unsigned char *buf, unsigned int buf_len,
                                unsigned char *out, unsigned int &out_len,
                                int is_internal_pkg)
{
    size_t used = 0;
    sec_proto::DPFixAlgorithmHead fix_data;
    int ret = fix_data.unpack((const char *)buf, buf_len,
                              (unsigned int *)&used, 0);

    if (ret != 0 || buf_len <= used) {
        ZEN_Trace_LogMsg::debug_output(
            ZEN_LOG_PRIORITY_ERROR,
            "[%u][%s], DPFixAlgorithmHead unpack fail, ret=%d, buf_len=%d",
            log_uin(user), __func__, ret, buf_len);
        return -1;
    }

    unsigned int remaining = buf_len - (unsigned short)used;
    unsigned char *dp_data = buf + used;

    const char *err_msg = "cipher decrypt fail";
    int cipher_ok = 0;
    try {
        cipher_ok = Cipher((int)fix_data.algorithm_type_, dp_data,
                           remaining, fix_data.key_id_, false);
    } catch (...) {
        err_msg = "cipher decrypt exception";
        cipher_ok = 0;
    }

    if (cipher_ok == 0) {
        DPBillAdapter bill(err_msg, 0x1e, user);
        DPBillAdapter::bill_ << (long long)fix_data.algorithm_type_
                             << (long long)fix_data.key_id_
                             << (long long)remaining;
        return -1;
    }

    return check_decrypt_buf(user, dp_data, remaining, out, out_len,
                             fix_data.crc, 0x1e, is_internal_pkg);
}

// ---------------------------------------------------------------------------
// ORIG 0x000be270: decrypt(UserInfo*, buf, buf_len, out, out_len&,
//                          pkg_type, is_internal_pkg)
//   pkg_type == 0x10: fix_decrypt; on success count
//       is_internal_pkg ? internal_fix_decrypt_count_++ :
//                         game_fix_decrypt_count_++
//   user && pkg_type == 0x08: dynamicy_decrypt; on success count
//       is_internal_pkg ? internal_dynamic_decrypt_count_++ :
//                         game_dynamic_decrypt_count_++
//   otherwise -1.
// Counter increments happen only when the inner call returns 0 and user != 0.
// ---------------------------------------------------------------------------
int DPVMBaseEngine::decrypt(tss_sdk_impl::UserInfo *user,
                            unsigned char *buf, unsigned int buf_len,
                            unsigned char *out, unsigned int &out_len,
                            int pkg_type, int is_internal_pkg)
{
    if (pkg_type == 0x10) {
        int ret = fix_decrypt(user, buf, buf_len, out, out_len,
                              is_internal_pkg);
        if (ret == 0 && user != 0) {
            if (is_internal_pkg != 0)
                user->user_stat_.internal_fix_decrypt_count_ += 1;
            else
                user->user_stat_.game_fix_decrypt_count_ += 1;
        }
        return ret;
    }

    if (user != 0 && pkg_type == 0x08) {
        int ret = dynamicy_decrypt(user, buf, buf_len, out, out_len,
                                   is_internal_pkg);
        if (ret == 0 && user != 0) {
            if (is_internal_pkg != 0)
                user->user_stat_.internal_dynamic_decrypt_count_ += 1;
            else
                user->user_stat_.game_dynamic_decrypt_count_ += 1;
        }
        return ret;
    }

    return -1;
}

// ---------------------------------------------------------------------------
// ORIG 0x000be360: dynamic_encrypt(UserInfo*, buf, buf_len, out, out_len&)
//
// Guards (all return -1):
//   !user
//   client_state_ <= 3 or client_state_ == 6
//   server_cur_code_ == 0
//   glb_dp_config_->dp_proto_conf.explicit_use_dynamic_encrypt &&
//       server_cur_code_ != server_new_code_
//   out_len < buf_len + 278  (278 = 5+8+265: DPInterDataHead +
//        DPDynamicAlgorithmHead + DPAlgorithmData)
//
// Then:
//   init_encrypt_data(user, algorithm_data, last_encrypt_sequence_ + 1)
//   algorithm_data.pack(send_buf, 0xffff, &used, 0)
//   memcpy(send_buf + used, buf, buf_len); used += buf_len
//   log pad_len_/custom_data_
//   server_code = get_server_code(user, server_cur_code_, &code_len)
//   log get_server_code result
//   encrypt_vm_->Load(server_code, code_len, send_buf, used)
//   encrypt_vm_->Run(); encrypt_vm_->GetDataBuffer(&data_buf, &data_len)
//   if (data_buf && data_len && data_len + 13 <= out_len):
//       DPInterData head:
//         packet_size_ = (u16)(data_len + 13)
//         protocol_type_ = 2, data_type_ = 0
//         dynamic_algorithm_data_.code_id_ = server_cur_code_
//         dynamic_algorithm_data_.crc_ = Crc32(send_buf, used)
//       DPInterData::pack(out, out_len, &used, 0)   // used becomes 13
//       memcpy(out + used, data_buf, data_len)
//       out_len = used + data_len
//       last_encrypt_sequence_ += 1; return 0
//   otherwise bill(err_msg, 0x1f, user)
//       << server_cur_code_ << used << data_len << out_len; return -1
// VM vtable slots: Load +0x2c, Run +0x28, GetDataBuffer +0x18 on encrypt_vm_.
// Error messages: get server code fail / encrypt load fail / encrypt run fail /
// encrypt run exception (empty string after a successful GetDataBuffer).
// ---------------------------------------------------------------------------
int DPVMBaseEngine::dynamic_encrypt(tss_sdk_impl::UserInfo *user,
                                    unsigned char *buf,
                                    unsigned int buf_len,
                                    unsigned char *out,
                                    unsigned int &out_len)
{
    if (user == 0)
        return -1;

    int client_state = user->user_code_info_.client_state_;
    if (client_state <= 3 || client_state == 6)
        return -1;

    if (user->user_code_info_.server_cur_code_ == 0)
        return -1;

    if (tss_dp_comm::DP_CONFIG::glb_dp_config_->
            dp_proto_conf.explicit_use_dynamic_encrypt != 0 &&
        user->user_code_info_.server_cur_code_ !=
            user->user_code_info_.server_new_code_) {
        return -1;
    }

    const size_t head_buf_len = 0x116;  // 5 + 8 + 265 = 278
    if (out_len < buf_len + head_buf_len) {
        ZEN_Trace_LogMsg::debug_output(
            ZEN_LOG_PRIORITY_ERROR,
            "[%u][%s], Too small outsize, buf_len=%u out_len=%u",
            log_uin(user), __func__, buf_len, out_len);
        return -1;
    }

    sec_proto::DPAlgorithmData algorithm_data;
    init_encrypt_data(user, algorithm_data,
                      user->user_code_info_.last_encrypt_sequence_ + 1);

    unsigned int used = 0;
    unsigned char send_buf[0xffff];
    algorithm_data.pack((char *)send_buf, 0xffff, &used, 0);
    memcpy(send_buf + used, buf, buf_len);
    used += buf_len;

    ZEN_Trace_LogMsg::debug_output(
        log_priority(user), "[%u][%s], pad_len_=%u, custom_data_=%u",
        log_uin(user), __func__, algorithm_data.pad_len_,
        algorithm_data.custom_data_);

    unsigned short server_code_len = 0;
    unsigned char *server_code =
        get_server_code(user, user->user_code_info_.server_cur_code_,
                        server_code_len);

    ZEN_Trace_LogMsg::debug_output(
        log_priority(user),
        "[%u][%s], get_server_code, code_buf=%p, len=%u, "
        "server_cur_code_=%u",
        log_uin(user), __func__, server_code, server_code_len,
        user->user_code_info_.server_cur_code_);

    unsigned int data_len = 0;
    unsigned char *data_buf = 0;
    const char *err_msg = "get server code fail";

    if (server_code != 0 && server_code_len != 0) {
        try {
            if (encrypt_vm_->Load(server_code, server_code_len, send_buf,
                                  used) != 0) {
                if (encrypt_vm_->Run() != 0) {
                    encrypt_vm_->GetDataBuffer(data_buf, data_len);
                    err_msg = "";
                } else {
                    err_msg = "encrypt run fail";
                }
            } else {
                err_msg = "encrypt load fail";
            }
        } catch (...) {
            err_msg = "encrypt run exception";
        }
    }

    if (data_buf == 0 || data_len == 0 ||
        data_len + 13 > out_len) {
        DPBillAdapter bill(err_msg, 0x1f, user);
        DPBillAdapter::bill_ << (long long)user->user_code_info_.server_cur_code_
                             << (long long)used
                             << (long long)data_len
                             << (long long)out_len;
        return -1;
    }

    sec_proto::DPInterData dynamic_head;
    dynamic_head.head.packet_size_ = (unsigned short)(data_len + 13);
    dynamic_head.head.protocol_type_ = 2;
    dynamic_head.head.data_type_ = 0;
    dynamic_head.body.dynamic_algorithm_data_.code_id_ =
        user->user_code_info_.server_cur_code_;
    dynamic_head.body.dynamic_algorithm_data_.crc_ =
        Crc32(send_buf, used);

    dynamic_head.pack((char *)out, out_len, &used, 0);  // used = 13
    memcpy(out + used, data_buf, data_len);
    out_len = used + data_len;

    ZEN_Trace_LogMsg::debug_output(
        log_priority(user),
        "[%u][%s], packet_size_=%u, crc_=%u, code_id_=%u, out_len=%u",
        log_uin(user), __func__, dynamic_head.head.packet_size_,
        dynamic_head.body.dynamic_algorithm_data_.crc_,
        dynamic_head.body.dynamic_algorithm_data_.code_id_, out_len);

    user->user_code_info_.last_encrypt_sequence_ += 1;
    return 0;
}

// ---------------------------------------------------------------------------
// ORIG 0x000be820: encrypt(UserInfo*, buf, buf_len, out, out_len&,
//                           use_fix_encrypt)
//
//   real_buf_len = out + 4
//   out_len -= 4                       // unconditional
//   max = glb_dp_config_->dp_proto_conf.dp_max_encrypt_size
//   chunk_len = buf_len; data_left_size = 0
//   if (max < buf_len) {
//       data_left_size = buf_len - max;
//       chunk_len = max;
//       log(priority, dp_max_encrypt_size=%d, data_left_size=%d)
//   }
//   temp_out_len = out_len
//   if (!use_fix_encrypt && dynamic_encrypt(user, buf, chunk_len,
//                                           real_buf_len, temp_out_len) == 0) {
//       user->user_stat_.dynamic_encrypt_count_++;      // no null check
//   } else {
//       ret = fix_encrypt(user, buf, chunk_len, real_buf_len, temp_out_len);
//       if (ret != 0) return ret;
//       if (user) user->user_stat_.fix_encrypt_count_++;
//   }
//   if (data_left_size) {
//       if (out_len < temp_out_len + data_left_size) { log(4); return -1; }
//       memcpy(real_buf_len + temp_out_len, buf + chunk_len, data_left_size);
//       temp_out_len += data_left_size;
//   }
//   log(priority, encrypt data left=%d temp_out_len=%d out_len=%d)
//   out_len = temp_out_len + 4
//   AB_NS_CODEENGINE::EncodeInt((char**)&out, out_len)
//   return 0
// ---------------------------------------------------------------------------
int DPVMBaseEngine::encrypt(tss_sdk_impl::UserInfo *user,
                            unsigned char *buf, unsigned int buf_len,
                            unsigned char *out, unsigned int &out_len,
                            bool use_fix_encrypt)
{
    char *real_buf_len = (char *)(out + 4);
    unsigned int data_left_size = 0;

    out_len -= 4;

    unsigned int max_encrypt_size =
        tss_dp_comm::DP_CONFIG::glb_dp_config_->
            dp_proto_conf.dp_max_encrypt_size;
    unsigned int chunk_len = buf_len;

    if (max_encrypt_size < buf_len) {
        data_left_size = buf_len - max_encrypt_size;
        chunk_len = max_encrypt_size;
        ZEN_Trace_LogMsg::debug_output(
            log_priority(user),
            "[%u][%s], dp_max_encrypt_size=%d, data_left_size=%d",
            log_uin(user), __func__, max_encrypt_size, data_left_size);
    }

    unsigned int temp_out_len = out_len;
    bool used_dynamic = false;

    if (!use_fix_encrypt) {
        if (dynamic_encrypt(user, buf, chunk_len,
                            (unsigned char *)real_buf_len,
                            temp_out_len) == 0) {
            // ORIG increments without a NULL check: dynamic_encrypt only
            // succeeds when user != 0.
            user->user_stat_.dynamic_encrypt_count_ += 1;
            used_dynamic = true;
        }
    }

    if (!used_dynamic) {
        int ret = fix_encrypt(user, buf, chunk_len,
                              (unsigned char *)real_buf_len,
                              temp_out_len);
        if (ret != 0)
            return ret;
        if (user != 0)
            user->user_stat_.fix_encrypt_count_ += 1;
    }

    if (data_left_size != 0) {
        if (out_len < temp_out_len + data_left_size) {
            ZEN_Trace_LogMsg::debug_output(
                ZEN_LOG_PRIORITY_ERROR,
                "[%u][%s], encrypt data left=%d  temp_out_len=%d "
                "out_len=%d",
                log_uin(user), __func__, data_left_size,
                temp_out_len, out_len);
            return -1;
        }
        memcpy((unsigned char *)real_buf_len + temp_out_len,
               buf + chunk_len, data_left_size);
        temp_out_len += data_left_size;
    }

    ZEN_Trace_LogMsg::debug_output(
        log_priority(user),
        "[%u][%s], encrypt data left=%d  temp_out_len=%d out_len=%d",
        log_uin(user), __func__, data_left_size, temp_out_len, out_len);

    out_len = temp_out_len + 4;
    AB_NS_CODEENGINE::EncodeInt((char **)&out, out_len);

    return 0;
}

}  // namespace tss_dpsdk

// ---------------------------------------------------------------------------
// ORIG CU dp_vmbase_engine.cpp static-initialization block.
//
// Decoded ORIG order (audit/report/init_brief.md, 27 calls, target size 1023):
//   ios_base::Init::Init(__ioinit) + atexit
//   16 x ZEN_STATUS_ITEM_WITHNAME ctor (0x238d..0x239c) + atexit
//   Crc32(DEFAULT_USR_ID, 4) -> default_user_id_crc  (defined above)
//   __cxa_guard_acquire / TssSdkCommSvc::TssSdkCommSvc() /
//     __cxa_guard_release + atexit; guard_abort + _Unwind_Resume pad
// ---------------------------------------------------------------------------
#include "src/static_init_block.h"

#include "src/commlib/zenlib/zen_singleton.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_comm_svc.h"

// ORIG .bss 0x0029d800 (weak): Singleton<TssSdkCommSvc>::object_creator.
template Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator
    Singleton<tss_sdk_impl::TssSdkCommSvc>::object_creator;
