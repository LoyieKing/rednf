// Canonical layout of tss_dpsdk::DPMsgHandler.
//
// ORIG evidence: python3 audit/tu_report.py --type 'tss_dpsdk::DPMsgHandler'
//   struct tss_dpsdk::DPMsgHandler size=4 bases=0 vfuncs=0
//     +0   vm_engine_   <anon>*   size=4
//   DWARF definition DIE (CU dp_msg_handler.cpp) shows one private member
//   `vm_engine_` at +0 whose type is a pointer to the DPVMBaseEngine
//   definition DIE.  GCC 4.1 emitted that definition unnamed, so the model
//   spells the member type as `<anon>*`.
//
// Method set and signatures come from /tmp/orig_model.json 'funcs'
// (mangled names, so the parameters are exact):
//   _ZN9tss_dpsdk12DPMsgHandler4initEv                        int init()
//   _ZN9tss_dpsdk12DPMsgHandler6reloadEv                      int reload()
//   _ZN9tss_dpsdk12DPMsgHandler15is_need_decryptEPN12tss_sdk_impl8UserInfoEi
//   _ZN9tss_dpsdk12DPMsgHandler15is_need_encryptEPN12tss_sdk_impl8UserInfoEi
//   _ZN9tss_dpsdk12DPMsgHandler13anticheck_pkgEPN12tss_sdk_impl8UserInfoEi
//   _ZN9tss_dpsdk12DPMsgHandler11check_timerEPN12tss_sdk_impl8UserInfoEl
// The remaining ORIG members (SendBinCount/SendBinFile/SendClientCode/
// SendServerCode/SendCmdList/SendDpMsgToClient/OnRecvMsgFromClient/
// OnRecvPkgFromClient/recv_buffer_for_encrypt/recv_buffer_for_decrypt) need
// sec_proto packet types and are declared by the TUs that implement them.
#ifndef TSS_SDK_LAYOUT_DP_MSG_HANDLER_H_
#define TSS_SDK_LAYOUT_DP_MSG_HANDLER_H_

#include <time.h>

#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_sdk_impl { struct UserInfo; }

// Packet/report types taken by reference; the complete definitions are
// generated protocol types (sec_proto).
namespace sec_proto {
struct DPClientDecryptInfo;
struct DPClientInitDataPtc;
struct DPClientLogPtc;
struct DPClientStatPtc;
struct DPInterData;
struct DPProtocolRequestCode;
}  // namespace sec_proto

namespace tss_dpsdk {

class DPVMBaseEngine;   // full layout: layout/dp_vmbase_engine.h

class DPMsgHandler {
public:
    // ORIG exports the implicit ctor/dtor pair
    //   _ZN9tss_dpsdk12DPMsgHandlerC1Ev @0x000ae2f0  C2Ev @0x000ae350
    //   _ZN9tss_dpsdk12DPMsgHandlerD1Ev @0x000ae070  D2Ev @0x000ae060
    // The ctor allocates the DPVMBaseEngine at +0; the dtor is empty.  Declared
    // so dp_msg_handler_core_agent.cpp can define them out of line; no layout
    // change (the class stays 4 bytes and has no virtuals).
    DPMsgHandler();
    ~DPMsgHandler();

    int init();
    int reload();
    bool is_need_decrypt(tss_sdk_impl::UserInfo *user, int cmd_id);
    bool is_need_encrypt(tss_sdk_impl::UserInfo *user, int cmd_id);
    int anticheck_pkg(tss_sdk_impl::UserInfo *user, int cmd_id);
    int check_timer(tss_sdk_impl::UserInfo *user, time_t now);

    // Every signature below is exactly the ORIG mangled form, e.g.
    //   _ZN9tss_dpsdk12DPMsgHandler23recv_buffer_for_encryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rj
    //   _ZN9tss_dpsdk12DPMsgHandler23recv_buffer_for_decryptEPN12tss_sdk_impl8UserInfoEPhjS4_Rjii
    //   _ZN9tss_dpsdk12DPMsgHandler19OnRecvPkgFromClientEPN12tss_sdk_impl8UserInfoEPhj
    //   _ZN9tss_dpsdk12DPMsgHandler19OnRecvMsgFromClientEPN12tss_sdk_impl8UserInfoERN9sec_proto14DPClientLogPtcE
    //     (and the DPClientStatPtc / DPClientDecryptInfo / DPClientInitDataPtc /
    //      DPProtocolRequestCode overloads)
    //   _ZN9tss_dpsdk12DPMsgHandler17SendDpMsgToClientEPN12tss_sdk_impl8UserInfoEPht
    //   _ZN9tss_dpsdk12DPMsgHandler17SendDpMsgToClientEPN12tss_sdk_impl8UserInfoERN9sec_proto11DPInterDataEPht
    //   _ZN9tss_dpsdk12DPMsgHandler12SendBinCountEPN12tss_sdk_impl8UserInfoE
    //   _ZN9tss_dpsdk12DPMsgHandler14SendClientCodeEPN12tss_sdk_impl8UserInfoE
    //   _ZN9tss_dpsdk12DPMsgHandler14SendServerCodeEPN12tss_sdk_impl8UserInfoE
    //   _ZN9tss_dpsdk12DPMsgHandler11SendBinFileEPN12tss_sdk_impl8UserInfoE
    //   _ZN9tss_dpsdk12DPMsgHandler11SendCmdListEPN12tss_sdk_impl8UserInfoE
    int recv_buffer_for_encrypt(tss_sdk_impl::UserInfo *user,
                                unsigned char *buf, unsigned int buf_len,
                                unsigned char *out, unsigned int &out_len);
    int recv_buffer_for_decrypt(tss_sdk_impl::UserInfo *user,
                                unsigned char *buf, unsigned int buf_len,
                                unsigned char *out, unsigned int &out_len,
                                int pkg_type, int is_internal_pkg);
    int OnRecvPkgFromClient(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                            unsigned int buf_len);
    int OnRecvMsgFromClient(tss_sdk_impl::UserInfo *user,
                            sec_proto::DPClientLogPtc &pkg);
    int OnRecvMsgFromClient(tss_sdk_impl::UserInfo *user,
                            sec_proto::DPClientStatPtc &pkg);
    int OnRecvMsgFromClient(tss_sdk_impl::UserInfo *user,
                            sec_proto::DPClientDecryptInfo &pkg);
    int OnRecvMsgFromClient(tss_sdk_impl::UserInfo *user,
                            sec_proto::DPClientInitDataPtc &pkg);
    int OnRecvMsgFromClient(tss_sdk_impl::UserInfo *user,
                            sec_proto::DPProtocolRequestCode &pkg);
    // ORIG spells the length argument `short unsigned int`, not `short`
    // (mangled ...EPht, not ...EPhs).
    int SendDpMsgToClient(tss_sdk_impl::UserInfo *user, unsigned char *buf,
                          unsigned short buf_len);
    int SendDpMsgToClient(tss_sdk_impl::UserInfo *user,
                          sec_proto::DPInterData &pkg, unsigned char *buf,
                          unsigned short buf_len);
    int SendBinCount(tss_sdk_impl::UserInfo *user);
    int SendClientCode(tss_sdk_impl::UserInfo *user);
    int SendServerCode(tss_sdk_impl::UserInfo *user);
    int SendBinFile(tss_sdk_impl::UserInfo *user);
    int SendCmdList(tss_sdk_impl::UserInfo *user);

private:
    DPVMBaseEngine *vm_engine_;   // +0x00

    // Compile-time layout contract; evaluated where the class is complete.
    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(DPMsgHandler, vm_engine_, 0,
                                dpmsghandler_vm_engine_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(DPMsgHandler) == 4,
                      dpmsghandler_size_must_be_4);

}  // namespace tss_dpsdk

#endif  // TSS_SDK_LAYOUT_DP_MSG_HANDLER_H_
