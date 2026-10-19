// Canonical layout of the uic plugin types.
//
// ORIG evidence:
//   python3 audit/tu_report.py --type 'tss_sdk_impl::UicUserInputInfo'
//     struct UicUserInputInfo size=36
//       +0 uin_ u32, +4 seq_ u32, +8 input_type_ enum(4), +12 msg_result_ enum(4),
//       +16 if_replace_ char, (3 padding) +20 message_ u8*, +24 message_len_ int,
//       +28 callback_data_ u8*, +32 callback_data_len_ int
//     The two enum types are the unnamed DIEs `._488` / `._489` of CU
//     uic_user_input_info.cpp; their enumerators are EW:
//       ._488 input_type_: EUITYPE_CHAT 0, EUITYPE_NAME 1, EUITYPE_IMAGE 2,
//                          EUITYPE_VIDEO 3
//       ._489 msg_result_: EMSG_NORMAL_FLAG 0, EMSG_EVIL_FLAG 1,
//                          EMSG_DIRTY_FLAG 2, EMSG_NOT_OPERATE_FLAG 3
//   python3 audit/tu_report.py --type 'tss_sdk_impl::UicUserInputInfoMgr'
//     struct UicUserInputInfoMgr size=32
//       +0 uic_user_info_hashmap_buf_ u8*, +4 uic_user_info_hashmap_
//          ZEN_LIB::shm_hashtable_expire<UicUserInputInfo, unsigned long long,
//          ZEN_LIB::smem_hash<unsigned long long>, UicExtractUserInfoKey,
//          std::equal_to<unsigned long long>, UicTimeOutProc>*,
//       +8 user_info_mem_pool_ BuddyAlloc*, +12 callback_proc_
//          TssSdkPluginCallbackProc, +16 sequence_ volatile u32,
//       +20 add_input_num_ volatile u32, +24 recv_input_total_num_ volatile u32,
//       +28 recv_input_valid_num_ volatile u32
//   python3 audit/tu_report.py --type 'tss_sdk_impl::TssSdkUicPlugin'
//     struct TssSdkUicPlugin size=76 bases=1
//       base +0 ITssSdkPlugin (16)
//       +16 user_input_mgr_ UicUserInputInfoMgr (32)
//       +48 thread_mutex_   ZEN_Thread_Mutex (28)
//

namespace ZEN_LIB {
template <class KEY>
struct smem_hash;
template <class T, class KEY, class HASH, class KEY_EXTRACT, class EQUAL,
          class TIMEOUT>
class shm_hashtable_expire;
}  // namespace ZEN_LIB
// TODO(migrate): uic_plugin_agent.cpp and uic_manager_agent.cpp still declare
// their own UicUserInputInfo / UicUserInputInfoMgr / TssSdkUicPlugin models.
#ifndef TSS_SDK_LAYOUT_TSS_SDK_UIC_H_
#define TSS_SDK_LAYOUT_TSS_SDK_UIC_H_

#include <ext/functional>
#include <functional>

#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_plugin_mgr.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"
// sec_proto::UserInputCode (uic_tdirty.h / formmog_proto_tss_sdk_uic.h) and
// the UserInputCheckPkg* wire types come from their single canonical
// declaration: the reconstruction of the ORIG protocol CU
// src/protocol/formmog/formmog_proto_tss_sdk_uic.cpp.  UicTdirty stores the
// enum by value, so it must be complete here; redefining it would be a second
// definition of the same enum in every TU that uses the packets.
#include "src/origsrc/formmog_proto_tss_sdk_uic.h"

class BuddyAlloc;
// uic_tdirty.h: global enum (C++98 cannot forward declare an unscoped enum,
// and UicTdirty stores it by value).  ORIG emits the same four enumerators.
enum MsgResultFlag {
    EMSG_NORMAL_FLAG = 0,
    EMSG_EVIL_FLAG = 1,
    EMSG_DIRTY_FLAG = 2,
    EMSG_NOT_OPERATE_FLAG = 3
};
struct UserMessage;     // uic_plugin.h, only used through pointers


namespace tss_sdk_impl {

struct UicExtractUserInfoKey;   // 1-byte key extractor
struct UicTimeOutProc;          // 1-byte timeout predicate

enum EUI_INPUT_TYPE {
    EUITYPE_CHAT = 0,
    EUITYPE_NAME = 1,
    EUITYPE_IMAGE = 2,
    EUITYPE_VIDEO = 3
};

// The user-input record's result field is the same global MsgResultFlag enum
// uic_tdirty.h declares (above): ORIG emits the message-result enum unnamed in
// CU uic_user_input_info.cpp only because the definition DIE is tied to its
// declaration by DW_AT_specification.  One enum, four enumerators - declaring
// a second one here would shadow EMSG_* for every TU that uses both.

struct UicUserInputInfo {
    unsigned int uin_;                 // +0x00
    unsigned int seq_;                 // +0x04
    EUI_INPUT_TYPE input_type_;        // +0x08
    MsgResultFlag msg_result_;         // +0x0c
    char if_replace_;                  // +0x10
    unsigned char *message_;           // +0x14
    int message_len_;                  // +0x18
    unsigned char *callback_data_;     // +0x1c
    int callback_data_len_;            // +0x20
};

// Callback payload handed to TssSdkPluginCallbackProc by
// TssSdkUicPlugin::handle_chat_user_input_local and by the manager's
// on_recv_check_result/expire_user_input: 28 bytes.
// ORIG: unnamed struct (formmog_proto_tss_sdk_uic.h, ._490 in CU
// uic_user_input_info.cpp); the plugin CU's local copy has the same layout -
// disassembly of TssSdkUicPlugin::handle_chat_user_input_local (0x000c6fb0)
// stores uin_@0, msg_result_flag_@4, message_len_@8, message_@0xc,
// callback_data_len_ (u16)@0x10, callback_data_@0x14, input_type_@0x18 into
// the buffer whose address is passed to callback_proc_.
struct UicChatJudgeResult {
    unsigned int uin_;                  // +0x00
    int msg_result_flag_;               // +0x04
    unsigned int message_len_;          // +0x08
    unsigned char *message_;            // +0x0c
    unsigned short callback_data_len_;  // +0x10
    unsigned char *callback_data_;      // +0x14
    int input_type_;                    // +0x18
};

struct UicUserInputInfoMgr {
    typedef ZEN_LIB::shm_hashtable_expire<
        UicUserInputInfo, unsigned long long,
        ZEN_LIB::smem_hash<unsigned long long>, UicExtractUserInfoKey,
        std::equal_to<unsigned long long>, UicTimeOutProc>
        UicUserInputInfoHashTable;

    // ORIG symbols (nm --defined-only):
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgrC1Ev / C2Ev / D1Ev / D2Ev
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr4initEjPFiPvE   (unsigned, callback)
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr4finiEv
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr14add_user_inputEPK11UserMessagePj
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr20on_recv_check_resultEPKN9sec_proto17UserInputCheckPkgE
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr17expire_user_inputEv
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr12get_user_numEv
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr12get_now_timeEv
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr11reset_countEv
    //   _ZN12tss_sdk_impl19UicUserInputInfoMgr10print_infoEv
    // Return types are not encoded in the mangled names.
    UicUserInputInfoMgr();
    ~UicUserInputInfoMgr();

    int init(unsigned int user_count, TssSdkPluginCallbackProc callback);
    int fini();
    int add_user_input(const UserMessage *user_message, unsigned int *seq);
    int on_recv_check_result(const sec_proto::UserInputCheckPkg *pkg);
    int expire_user_input();
    unsigned int get_user_num();
    unsigned int get_now_time();
    void reset_count();
    void print_info();

    unsigned char *uic_user_info_hashmap_buf_;         // +0x00
    UicUserInputInfoHashTable *uic_user_info_hashmap_;  // +0x04
    BuddyAlloc *user_info_mem_pool_;                   // +0x08
    TssSdkPluginCallbackProc callback_proc_;           // +0x0c
    volatile unsigned int sequence_;                   // +0x10
    volatile unsigned int add_input_num_;              // +0x14
    volatile unsigned int recv_input_total_num_;       // +0x18
    volatile unsigned int recv_input_valid_num_;       // +0x1c
};

// uic_tdirty.h, sizeof == 8 (ORIG: UicTdirty 8).
//   _ZN12tss_sdk_impl9UicTdirtyC1Ev / C2Ev / D1Ev / D2Ev / 8instanceEv
//   _ZN12tss_sdk_impl9UicTdirty4initEPcjN9sec_proto13UserInputCodeE
//   _ZN12tss_sdk_impl9UicTdirty15uic_dirty_checkEPhP13MsgResultFlag
//   _ZN12tss_sdk_impl9UicTdirty15uic_dirty_checkEiPhPiP13MsgResultFlag
//   _ZN12tss_sdk_impl9UicTdirty17uic_dirty_replaceEPh
//   _ZN12tss_sdk_impl9UicTdirty17uic_dirty_replaceEiPhPi
//   static instance_ (_ZN12tss_sdk_impl9UicTdirty9instance_E)
class UicTdirty {
public:
    UicTdirty();
    ~UicTdirty();

    static UicTdirty *instance();

    int init(char *buf, unsigned int buf_size, sec_proto::UserInputCode code);
    bool uic_dirty_check(unsigned char *buf, MsgResultFlag *flag);
    bool uic_dirty_check(int len, unsigned char *buf, int *pos,
                         MsgResultFlag *flag);
    bool uic_dirty_replace(unsigned char *buf);
    bool uic_dirty_replace(int len, unsigned char *buf, int *pos);

private:
    sec_proto::UserInputCode code_;   // +0x00
    bool is_inited_;                  // +0x04

    // ORIG singleton pointer object: _ZN12tss_sdk_impl9UicTdirty9instance_E.
    // Static, so it does not contribute to sizeof(UicTdirty) == 8.
    static UicTdirty *instance_;

    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(UicTdirty, code_, 0, uictdirty_code_offset);
        TSS_SDK_OFFSET_CONTRACT(UicTdirty, is_inited_, 4,
                                uictdirty_is_inited_offset);
    }
};

class TssSdkUicPlugin : public ITssSdkPlugin {
public:
    TssSdkUicPlugin();
    virtual ~TssSdkUicPlugin();

    virtual int init(void *init_data);
    virtual int fini();
    virtual int proc(void *data);
    virtual int notify(void *data);

private:

    // Out-of-line definitions live in uic_plugin_agent.cpp; ORIG emits them as
    // local (non-virtual) methods of this class.
    int handle_name_user_input(UserMessage *user_message);
    int handle_chat_user_input(UserMessage *user_message);
    int handle_chat_user_input_local(UserMessage *user_message);
    UicUserInputInfoMgr user_input_mgr_;   // +0x10
    ZEN_Thread_Mutex thread_mutex_;        // +0x30
};

TSS_SDK_STATIC_ASSERT(sizeof(UicUserInputInfo) == 36,
                      uicuserinputinfo_size_must_be_36);
TSS_SDK_STATIC_ASSERT(sizeof(UicUserInputInfoMgr) == 32,
                      uicuserinputinfomgr_size_must_be_32);
TSS_SDK_STATIC_ASSERT(sizeof(UicChatJudgeResult) == 28,
                      uicchatjudgeresult_size_must_be_28);
TSS_SDK_STATIC_ASSERT(sizeof(UicTdirty) == 8, uictdirty_size_must_be_8);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkUicPlugin) == 76,
                      tsssdkuicplugin_size_must_be_76);

}  // namespace tss_sdk_impl

#endif  // TSS_SDK_LAYOUT_TSS_SDK_UIC_H_
