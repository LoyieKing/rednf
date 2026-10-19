// Canonical layout of ::DPInterface.
//
// ORIG evidence (libtss_sdk.so.1.0.394, reference/libtss_sdk.so.1.0.394.orig):
//   python3 audit/tu_report.py --type 'DPInterface'
//     struct DPInterface size=12 bases=0 vfuncs=0
//       +0   is_inited_    unsigned int               size=4   (private)
//       +4   msg_handler_  tss_dpsdk::DPMsgHandler*   size=4   (private)
//       +8   hack_manager_ tss_dpsdk::DPHackMgr*      size=4   (private)
//   DWARF definition DIE: dpinterface.cpp (source line 34 for the members).
//   The DWARF declaration lists the class in the global namespace; the two
//   handler names are scoped in tss_dpsdk (nm -C:
//   _ZN9tss_dpsdk12DPMsgHandlerC1Ev / _ZN9tss_dpsdk9DPHackMgrC1Ev).
//
// The member functions below are the exact declarations the reconstruction
// already used, so every mangled name stays unchanged
// (_ZN11DPInterface6instanceEv, _ZN11DPInterface4initEPKc, ...).
#ifndef TSS_SDK_LAYOUT_DP_INTERFACE_H_
#define TSS_SDK_LAYOUT_DP_INTERFACE_H_

#include <time.h>

#include "src/formmog/tss_sdk/layout/layout_contract.h"

namespace tss_dpsdk { class DPMsgHandler; class DPHackMgr; }
namespace tss_sdk_impl { struct UserInfo; }

struct DPInterface {
private:
    unsigned int is_inited_;                 // +0x00
    tss_dpsdk::DPMsgHandler *msg_handler_;   // +0x04
    tss_dpsdk::DPHackMgr *hack_manager_;     // +0x08

public:
    static DPInterface *instance();          // line 36
    int init(const char *arg0);              // line 38
    int add_user(tss_sdk_impl::UserInfo *arg0);   // line 41
    int del_user(tss_sdk_impl::UserInfo *arg0);   // line 42
    int check_timer(time_t arg0);                 // line 44
    int check_timer(tss_sdk_impl::UserInfo *arg0, time_t arg1);  // line 45
    int recv_buffer_for_encrypt(tss_sdk_impl::UserInfo *arg0,
                                unsigned char *arg1, unsigned int arg2,
                                unsigned char *arg3, unsigned int &arg4);
    int recv_buffer_for_encrypt(tss_sdk_impl::UserInfo *arg0,
                                unsigned int arg1, unsigned char *arg2,
                                unsigned int arg3, unsigned char *arg4,
                                unsigned int &arg5);
    int recv_buffer_for_decrypt(tss_sdk_impl::UserInfo *arg0,
                                unsigned char *arg1, unsigned int arg2,
                                unsigned char *arg3, unsigned int &arg4);
    bool is_need_decrypt(tss_sdk_impl::UserInfo *arg0, int arg1);
    bool is_need_encrypt(tss_sdk_impl::UserInfo *arg0, int arg1);
    int anticheck_pkg(tss_sdk_impl::UserInfo *arg0, int arg1,
                      unsigned char *arg2, unsigned int arg3);
    int on_recv_msg(tss_sdk_impl::UserInfo *arg0, void *arg1);
    int write_bill(unsigned int arg0, char *arg1);   // line 62

private:
    int check_and_reload_config();           // line 65
    DPInterface();                           // line 74
    ~DPInterface();                          // line 75
    DPInterface(const DPInterface &arg0);    // line 76
    DPInterface &operator=(const DPInterface &arg0);  // line 77

    // Compile-time layout contract; evaluated where the class is complete.
    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(DPInterface, is_inited_, 0,
                                dpinterface_is_inited_offset);
        TSS_SDK_OFFSET_CONTRACT(DPInterface, msg_handler_, 4,
                                dpinterface_msg_handler_offset);
        TSS_SDK_OFFSET_CONTRACT(DPInterface, hack_manager_, 8,
                                dpinterface_hack_manager_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(DPInterface) == 12, dpinterface_size_must_be_12);
#endif  // TSS_SDK_LAYOUT_DP_INTERFACE_H_
