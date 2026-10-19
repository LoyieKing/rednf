// Canonical layout of tss_dpsdk::DPVMBaseEngine.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_dpsdk::DPVMBaseEngine'
//   struct DPVMBaseEngine size=68 bases=0 vfuncs=0
//     +0   encrypt_vm_              CVMBase*          size=4
//     +4   decrypt_vm_              CVMBase*          size=4
//     +8   code_manager_            <anon>*          size=4
//     +12  encrypt_vm_thread_mutex_ ZEN_Thread_Mutex  size=28
//     +40  decrypt_vm_thread_mutex_ ZEN_Thread_Mutex  size=28
//   The model prints size 0 for both mutex members because the DIE in those
//   CUs is a declaration; the model's own ZEN_Thread_Mutex is 28 bytes and
//   the member offsets (12 -> 40 -> 68) fix the real extent.
//   Out-of-line members (orig_model 'funcs'):
//     init(), reload(), encrypt(UserInfo*,u8*,u32,u8*,u32&,bool),
//     decrypt(UserInfo*,u8*,u32,u8*,u32&,int,int),
//     get_server_code/get_client_code(UserInfo*,u16,u16&),
//     fix_encrypt/dynamic_encrypt(UserInfo*,u8*,u32,u8*,u32&),
//     fix_decrypt/dynamicy_decrypt(UserInfo*,u8*,u32,u8*,u32&,int),
//     check_decrypt_buf(UserInfo*,u8*,u32,u8*,u32&,u32,int,int),
//     init_encrypt_data(UserInfo*, sec_proto::DPAlgorithmData&, u32).
//
// ZEN_Thread_Mutex comes from the shared zenlib declaration
// (commlib/zenlib/zen_lock_thread_mutex.h); CVMBase is only ever used through
// a pointer.
#ifndef TSS_SDK_LAYOUT_DP_VMBASE_ENGINE_H_
#define TSS_SDK_LAYOUT_DP_VMBASE_ENGINE_H_

#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_sdk_impl { struct UserInfo; }
namespace sec_proto { struct DPAlgorithmData; }

namespace tss_dpsdk {

class CVMBase;

class DPVMBaseEngine {
public:
    // ORIG exports the implicit ctor/dtor pair
    //   _ZN9tss_dpsdk14DPVMBaseEngineC1Ev @0x000bd380  C2Ev @0x000bd420
    //   _ZN9tss_dpsdk14DPVMBaseEngineD1Ev @0x000bd2c0  D2Ev @0x000bd320
    // Declared so dp_vmbase_engine_agent.cpp can define them out of line and
    // DPMsgHandler's ctor can `new DPVMBaseEngine()`.  No layout change: the
    // class stays 68 bytes and has no virtuals.
    DPVMBaseEngine();
    ~DPVMBaseEngine();

    int init();
    int reload();
    int encrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                unsigned int buf_len, unsigned char *out,
                unsigned int &out_len, bool is_internal_pkg);
    int decrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                unsigned int buf_len, unsigned char *out,
                unsigned int &out_len, int crc, int is_internal_pkg);
    unsigned char *get_server_code(tss_sdk_impl::UserInfo *user,
                                   unsigned short code_id,
                                   unsigned short &buf_len);
    unsigned char *get_client_code(tss_sdk_impl::UserInfo *user,
                                   unsigned short code_id,
                                   unsigned short &buf_len);

    // ORIG dp_vmbase_engine.h, header-inline code-cache selectors.  The ORIG
    // DWARF declares them (DW_AT_declaration only, decl_file 439 =
    // dp_vmbase_engine.h, decl lines 84/90/96/101/110) and the target emits no
    // out-of-line symbol:
    //   _ZN9tss_dpsdk14DPVMBaseEngine21select_server_code_idEPN12tss_sdk_impl8UserInfoE
    //   _ZN9tss_dpsdk14DPVMBaseEngine21select_client_code_idEPN12tss_sdk_impl8UserInfoE
    //   _ZN9tss_dpsdk14DPVMBaseEngine25select_server_bin_file_idEPN12tss_sdk_impl8UserInfoE
    //   _ZN9tss_dpsdk14DPVMBaseEngine17check_server_codeEPN12tss_sdk_impl8UserInfoEt
    //   _ZN9tss_dpsdk14DPVMBaseEngine17check_client_codeEPN12tss_sdk_impl8UserInfoEt
    //   _ZN9tss_dpsdk14DPVMBaseEngine18check_version_hashEPN12tss_sdk_impl8UserInfoEPh
    // The bodies read code_manager_ and are defined by the TU that owns the
    // DPCodeMgr code-table model (dp_msg_handler_recv_agent.cpp); declaring
    // them here adds no data member, so the 68-byte layout is unchanged.
    int select_server_code_id(tss_sdk_impl::UserInfo *user);
    int select_client_code_id(tss_sdk_impl::UserInfo *user);
    int select_server_bin_file_id(tss_sdk_impl::UserInfo *user);
    bool check_server_code(tss_sdk_impl::UserInfo *user,
                           unsigned short code_id);
    bool check_client_code(tss_sdk_impl::UserInfo *user,
                           unsigned short code_id);
    bool check_version_hash(tss_sdk_impl::UserInfo *user, unsigned char *hash);
    void init_encrypt_data(tss_sdk_impl::UserInfo *user,
                           sec_proto::DPAlgorithmData &algorithm_data,
                           unsigned int custom_data);
    int fix_encrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                    unsigned int buf_len, unsigned char *out,
                    unsigned int &out_len);
    int dynamic_encrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                        unsigned int buf_len, unsigned char *out,
                        unsigned int &out_len);
    int fix_decrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                    unsigned int buf_len, unsigned char *out,
                    unsigned int &out_len, int err_type);
    int dynamicy_decrypt(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                         unsigned int buf_len, unsigned char *out,
                         unsigned int &out_len, int err_type);
    int check_decrypt_buf(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                          unsigned int buf_len, unsigned char *out,
                          unsigned int &out_len, unsigned int crc,
                          int err_type, int is_internal_pkg);

private:
    CVMBase *encrypt_vm_;                        // +0x00
    CVMBase *decrypt_vm_;                        // +0x04
    void *code_manager_;                         // +0x08
    ZEN_Thread_Mutex encrypt_vm_thread_mutex_;   // +0x0c
    ZEN_Thread_Mutex decrypt_vm_thread_mutex_;   // +0x28

    // Compile-time layout contract; evaluated where the class is complete.
    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(DPVMBaseEngine, encrypt_vm_, 0,
                                dpvmbaseengine_encrypt_vm_offset);
        TSS_SDK_OFFSET_CONTRACT(DPVMBaseEngine, decrypt_vm_, 4,
                                dpvmbaseengine_decrypt_vm_offset);
        TSS_SDK_OFFSET_CONTRACT(DPVMBaseEngine, code_manager_, 8,
                                dpvmbaseengine_code_manager_offset);
        TSS_SDK_OFFSET_CONTRACT(DPVMBaseEngine, encrypt_vm_thread_mutex_, 12,
                                dpvmbaseengine_encrypt_mutex_offset);
        TSS_SDK_OFFSET_CONTRACT(DPVMBaseEngine, decrypt_vm_thread_mutex_, 40,
                                dpvmbaseengine_decrypt_mutex_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(DPVMBaseEngine) == 68,
                      dpvmbaseengine_size_must_be_68);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_VMBASE_ENGINE_H_
