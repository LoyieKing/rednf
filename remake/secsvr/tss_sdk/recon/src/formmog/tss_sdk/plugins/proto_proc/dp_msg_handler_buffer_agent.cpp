// Reconstructed from libtss_sdk.so.1.0.394 (ELF32 i386, GNU C++ 4.1.0/4.1.2
// generation, built -O2 -fPIC).
//
//   tss_dpsdk::DPMsgHandler::recv_buffer_for_encrypt(
//       UserInfo*, unsigned char*, unsigned int, unsigned char*, unsigned int&)
//       0x000ae1b0 - 0x000ae200  (DWARF low_pc/high_pc)
//   tss_dpsdk::DPMsgHandler::recv_buffer_for_decrypt(
//       UserInfo*, unsigned char*, unsigned int, unsigned char*, unsigned int&,
//       int, int)
//       0x000ae200 - 0x000ae289  (next symbol 0x000ae290; padding after 0xae289)
//   tss_dpsdk::DPMsgHandler::OnRecvPkgFromClient(
//       UserInfo*, unsigned char*, unsigned int)
//       0x000b0fc0 - 0x000b1470  (DWARF low_pc/high_pc; next symbol
//       __static_initialization_and_destruction_0 at 0x000b1470)
//
// The only symbols this file is meant to provide are the three DPMsgHandler
// methods above.  Every helper type below is a local, minimal ABI declaration:
// the generated headers in build/tmp/tss_sdk_headers are empty for
// dp_msg_handler.h / dp_vmbase_engine.h / dp_bill_adapter.h / dp_msg_packer.h,
// and the real class layout/method set is not available.  The declarations
// reproduce only what is needed to emit the exact mangled calls; they are NOT
// a reconstruction of the full SDK classes (INTEGRATION BOUNDARY).
//
// DWARF evidence used (libtss_sdk.so.1.0.394):
//   DPMsgHandler          sizeof 4, private DPVMBaseEngine* vm_engine_ @+0x00
//   DPInterData           sizeof 0x631 (1585), head @+0x00, body @+0x05
//   DPInterDataHead       sizeof 5, packed: packet_size_ u16 @+0, protocol_type_
//                         u16 @+2, data_type_ u8 @+4
//   DPInterDataBody       union size 0x62c (1580); cs_test_data_/sc_test_data_
//                         are uint8_t[10]
//   DPProtocolRequestCode sizeof 0x0c, DPClientInitDataPtc 0x24,
//   DPClientStatPtc       sizeof 0x62c, DPClientDecryptInfo 0x08,
//   DPClientLogPtc        sizeof 0x86
//
// Recovered dependencies (nm -C of the .so; all external here):
//   tss_dpsdk::DPVMBaseEngine::encrypt(UserInfo*, unsigned char*, unsigned int,
//       unsigned char*, unsigned int&, bool)                0x000be820
//   tss_dpsdk::DPVMBaseEngine::decrypt(UserInfo*, unsigned char*, unsigned int,
//       unsigned char*, unsigned int&, int, int)            0x000be270
//   tss_dpsdk::DPMsgPacker::send_error_report_msg(UserInfo*, int, int)
//                                                            0x000b2920
//   tss_dpsdk::DPBillAdapter::dump(char const*, unsigned int,
//       unsigned char const*, unsigned int)                  0x000b? (nm symbol)
//   tss_dpsdk::DPBillAdapter::DPBillAdapter(char const*, unsigned int,
//       UserInfo*)
//   tss_dpsdk::DPBillAdapter::~DPBillAdapter()
//   tss_dpsdk::DPBillAdapter::bill_ (DPBillRecorder, derives CommBillRecord)
//   CommBillRecord::operator<<(long long)
//   sec_proto::DPInterData::unpack(char const*, unsigned int, unsigned int*,
//       unsigned int)                                        0x001619e0
//   ZEN_Trace_LogMsg::debug_output(ZEN_LOG_PRIORITY, char const*, ...)
//
// No shared header, README, CMake, test or other agent's file was modified.

#include "src/origsrc/formmog_proto_tss_sdk_client.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/user_info_recon.h"

#include <stddef.h>
#include <string.h>

#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/formmog/tss_sdk/layout/dp_bill.h"
#include "src/formmog/tss_sdk/layout/dp_msg_handler.h"
#include "src/formmog/tss_sdk/layout/dp_vmbase_engine.h"

namespace tss_dpsdk {

// No canonical layout header exists for DPMsgPacker: only this out-of-line
// entry point is consumed by this TU (dp_msg_packer_core_agent.cpp owns it).
class DPMsgPacker {
public:
    static int send_error_report_msg(tss_sdk_impl::UserInfo *user, int error_id,
                                     int flag);
};

}  // namespace tss_dpsdk

namespace tss_dpsdk {

// ---------------------------------------------------------------------------
// 0x000ae1b0
// ---------------------------------------------------------------------------

int DPMsgHandler::recv_buffer_for_encrypt(tss_sdk_impl::UserInfo *user,
                                          unsigned char *buf,
                                          unsigned int buf_len,
                                          unsigned char *out,
                                          unsigned int &out_len) {
    return vm_engine_->encrypt(user, buf, buf_len, out, out_len, false);
}

// ---------------------------------------------------------------------------
// 0x000ae200
// ---------------------------------------------------------------------------

int DPMsgHandler::recv_buffer_for_decrypt(tss_sdk_impl::UserInfo *user,
                                          unsigned char *buf,
                                          unsigned int buf_len,
                                          unsigned char *out,
                                          unsigned int &out_len, int pkg_type,
                                          int is_internal_pkg) {
    int ret = vm_engine_->decrypt(user, buf, buf_len, out, out_len, pkg_type,
                                  is_internal_pkg);

    if (ret != 0 && user != 0)
        DPMsgPacker::send_error_report_msg(user, 0x192, 1);

    return ret;
}

// ---------------------------------------------------------------------------
// 0x000b0fc0
// ---------------------------------------------------------------------------
//
// Original source lines (DWARF .debug_line):
//   770 entry
//   772 flags&1 bill-dump gate
//   774 DPBillAdapter::dump("OnRecvPkgFromClient", ...)
//   777 size_t used = 0
//   778 sec_proto::DPInterData pkg
//   779 int ret = pkg.unpack(...)
//   780 unpack failure gate
//   782 DPBillAdapter bill("dp pkg decode failed", 33, user)
//   783 bill << buf_len
//   784 for (i = 0; i < buf_len && i < 12; ++i)
//   786   bill << buf[i]
//   788 send_error_report_msg(user, 0x1a3, 1)
//   789 return -1
//   792 success log
//   795 switch (pkg.head.protocol_type_)
//   799 case 9  -> DPProtocolRequestCode
//   805 case 10 -> DPClientInitDataPtc
//   811 case 13 -> DPClientLogPtc
//   817 case 11 -> DPClientStatPtc
//   823 case 12 -> DPClientDecryptInfo
//   829 case 14 -> compare 10-byte test data
//   832 DPBillAdapter bill("check test failed", 26, user)
//   836 bill << buf_len << protocol_type_ << data_type_ << packet_size_
//   837/839 10-byte loop over pkg.body.cs_test_data_
//   842 send_error_report_msg(user, 0x19e, 1)
//   850 default log
//   853 send_error_report_msg(user, 0x1a4, 1)
//   858 return ret
//
// Note: the entry bill gate dereferences user without a null check, matching
// the original (0x0b0fdd test BYTE PTR [user+0x20],1).  Later uses are
// null-tolerant exactly as in the original.

int DPMsgHandler::OnRecvPkgFromClient(tss_sdk_impl::UserInfo *user,
                                      unsigned char *buf, unsigned int buf_len) {
    if ((user->base_info_.flags_ & 1) != 0)
        DPBillAdapter::dump("OnRecvPkgFromClient", user->base_info_.uin_, buf,
                            buf_len);

    size_t used = 0;
    sec_proto::DPInterData pkg;
    int ret = pkg.unpack(reinterpret_cast<const char *>(buf), buf_len, &used, 0);

    if (ret != 0) {
        DPBillAdapter bill("dp pkg decode failed", 33u, user);
        bill << buf_len;
        for (unsigned int i = 0; i < buf_len && i < 12; ++i)
            bill << buf[i];
        DPMsgPacker::send_error_report_msg(user, 0x1a3, 1);
        return -1;
    }

    ZEN_Trace_LogMsg::debug_output(
        (user != 0 && (user->base_info_.flags_ & 1) != 0) ? RS_INFO : RS_DEBUG,
        "[%u][%s], buf_len=%u, used=%u, protocol_type_=%u, data_type_=%u, "
        "packet_size_=%u",
        user != 0 ? user->base_info_.uin_ : 0u, __func__, buf_len, used,
        static_cast<unsigned int>(pkg.head.protocol_type_),
        static_cast<unsigned int>(pkg.head.data_type_),
        static_cast<unsigned int>(pkg.head.packet_size_));

    switch (pkg.head.protocol_type_) {
    case 9:
        ret = OnRecvMsgFromClient(user, pkg.body.request_code_);
        break;
    case 10:
        ret = OnRecvMsgFromClient(user, pkg.body.init_data_ptc_);
        break;
    case 13:
        ret = OnRecvMsgFromClient(user, pkg.body.client_log_ptc_);
        break;
    case 11:
        ret = OnRecvMsgFromClient(user, pkg.body.client_stat_ptc_);
        break;
    case 12:
        ret = OnRecvMsgFromClient(user, pkg.body.decrypt_info_);
        break;
    case 14:
        ret = 0;
        if (memcmp(pkg.body.cs_test_data_, "0123456789", 10) != 0) {
            DPBillAdapter bill("check test failed", 26u, user);
            bill << buf_len;
            bill << pkg.head.protocol_type_;
            bill << pkg.head.data_type_;
            bill << pkg.head.packet_size_;
            for (unsigned int i = 0; i < 10; ++i)
                bill << pkg.body.cs_test_data_[i];
            DPMsgPacker::send_error_report_msg(user, 0x19e, 1);
            return -1;
        }
        break;
    default:
        ZEN_Trace_LogMsg::debug_output(
            RS_ERROR,
            "[%u][%s], unknown interanl msg,buf_len=%u, used=%u, "
            "protocol_type_=%u, data_type_=%u, packet_size_=%u",
            user != 0 ? user->base_info_.uin_ : 0u, __func__, buf_len, used,
            static_cast<unsigned int>(pkg.head.protocol_type_),
            static_cast<unsigned int>(pkg.head.data_type_),
            static_cast<unsigned int>(pkg.head.packet_size_));
        DPMsgPacker::send_error_report_msg(user, 0x1a4, 1);
        return -1;
    }

    return ret;
}

}  // namespace tss_dpsdk
