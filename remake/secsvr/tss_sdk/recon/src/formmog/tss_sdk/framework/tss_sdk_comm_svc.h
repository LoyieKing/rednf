// Shared reconstructed declaration for libtss_sdk.so.1.0.394.
#ifndef TSS_RECON_FRAMEWORK_TSS_SDK_COMM_SVC_H_
#define TSS_RECON_FRAMEWORK_TSS_SDK_COMM_SVC_H_

#include <cstddef>
#include <string>

#include "src/formmog/tss_sdk/framework/tss_sdk_plugin_mgr.h"

class DPInterface;

namespace tss_sdk_impl {

struct TssSdkChannelStatus {
    unsigned int total_recv_;
    unsigned int succ_recv_;
    unsigned int fail_recv_;
    unsigned int total_send_;
    unsigned int succ_send_;
    unsigned int fail_send_;
};
struct UserInfo;

class TssSdkCommSvc {
public:
    TssSdkCommSvc();
    ~TssSdkCommSvc();

    UserInfo *get_user_info(unsigned int &uin, unsigned int idx);
    UserInfo *get_user_info_by_uin(unsigned int uin);

    int send_buf_to_client(int busi_type, unsigned int uin,
                           unsigned char *buf, short buf_size);
    int recv_buf_from_backend(const char **recv_buf,
                              std::size_t *recv_buf_size);
    int send_buf_to_backend(const char *send_buf, std::size_t send_buf_size,
                            bool use_ipc, int business_id);

    template <class PKG>
    int send_ipc_pkg(PKG const &pkg, unsigned int uin, unsigned int cmd,
                     unsigned short rcv);
    template <class PKG>
    int send_socket_pkg(int business_id, PKG const &pkg);

    bool is_need_trans_uin(unsigned int game_id);
    int get_last_error_code();
    std::string get_last_error();
    void set_errno(int err_no);

    unsigned int channel_id_;                  // +0x00
    TssSdkPluginMgr *plugin_mgr_;              // +0x04
    DPInterface *dp_if_;                       // +0x08
    TssSdkChannelStatus sdk_channel_status_;   // +0x0c
    CThreadErrorInfo *error_info_;             // +0x24

private:
    TssSdkCommSvc(const TssSdkCommSvc &);
    TssSdkCommSvc &operator=(const TssSdkCommSvc &);
};

}  // namespace tss_sdk_impl

typedef char tss_recon_comm_svc_size_must_be_40[
    sizeof(tss_sdk_impl::TssSdkCommSvc) == 40 ? 1 : -1];
typedef char tss_recon_channel_status_size_must_be_24[
    sizeof(tss_sdk_impl::TssSdkChannelStatus) == 24 ? 1 : -1];

#endif  // TSS_RECON_FRAMEWORK_TSS_SDK_COMM_SVC_H_
