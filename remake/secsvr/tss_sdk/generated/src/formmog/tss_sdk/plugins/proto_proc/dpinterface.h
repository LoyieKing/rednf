// Auto-generated header stub from DWARF info
// Original path: src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DPINTERFACE_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DPINTERFACE_H_H_

#include <time.h>

// sizeof = 12
struct DPInterface { // line 34
private:
unsigned int is_inited_;
DPMsgHandler *msg_handler_;
DPHackMgr *hack_manager_;
public:
static DPInterface * instance(); // line 36
int init(const char *arg0); // line 38
int add_user(tss_sdk_impl::UserInfo *arg0); // line 41
int del_user(tss_sdk_impl::UserInfo *arg0); // line 42
int check_timer(time_t arg0); // line 44
int check_timer(tss_sdk_impl::UserInfo *arg0, time_t arg1); // line 45
int recv_buffer_for_encrypt(tss_sdk_impl::UserInfo *arg0, unsigned char *arg1, unsigned int arg2, unsigned char *arg3, unsigned int &arg4); // line 49
int recv_buffer_for_encrypt(tss_sdk_impl::UserInfo *arg0, unsigned int arg1, unsigned char *arg2, unsigned int arg3, unsigned char *arg4, unsigned int &arg5); // line 51
int recv_buffer_for_decrypt(tss_sdk_impl::UserInfo *arg0, unsigned char *arg1, unsigned int arg2, unsigned char *arg3, unsigned int &arg4); // line 53
bool is_need_decrypt(tss_sdk_impl::UserInfo *arg0, int arg1); // line 55
bool is_need_encrypt(tss_sdk_impl::UserInfo *arg0, int arg1); // line 56
int anticheck_pkg(tss_sdk_impl::UserInfo *arg0, int arg1, unsigned char *arg2, unsigned int arg3); // line 57
int on_recv_msg(tss_sdk_impl::UserInfo *arg0, void *arg1); // line 60
int write_bill(unsigned int arg0, char *arg1); // line 62
private:
int check_and_reload_config(); // line 65
DPInterface(); // line 74
~DPInterface(); // line 75
DPInterface(const DPInterface &arg0); // line 76
DPInterface & operator=(const DPInterface &arg0); // line 77
};

#endif // SECSVR_SRC_FORMMOG_TSS_SDK_PLUGINS_PROTO_PROC_DPINTERFACE_H_H_
