// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_zerg_mmappipe.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_ZERG_MMAPPIPE_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_ZERG_MMAPPIPE_H_H_

#include "output/commlib/zenlib/release/include/zen_bus_two_way.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "src/commlib/framework/comm_service_info.h"
#include <stddef.h>

// sizeof = 10280
struct Zerg_MMAP_BusPipe : public ZEN_BusPipe_TwoWay { // line 15
protected:
static const char ZERG_PIPENAME[][];
bool if_check_pthrad_;
SERVICES_ID zerg_svr_info_;
static char send_buffer_[];
Comm_Stat_Monitor *monitor_;
static Zerg_MMAP_BusPipe *zerg_bus_instance_;
public:
Zerg_MMAP_BusPipe(); // line 38
~Zerg_MMAP_BusPipe(); // line 40
int initialize(SERVICES_ID &arg0, size_t arg1, size_t arg2, size_t arg3, bool arg4, bool arg5); // line 50
int getpara_from_zergcfg(const ZenINIPropertyTree &arg0); // line 53
int getpara_from_zergcfg(const Zerg_MMAP_BusPipe::ZERG_CONFIG &arg0); // line 54
int init_after_getcfg(size_t arg0, bool arg1); // line 60
void get_mmapfile_name(char *arg0, size_t arg1); // line 63
int pop_front_recvpipe(Comm_App_Frame *&arg0); // line 67
int push_back_sendpipe(const Comm_App_Frame *arg0); // line 69
int pipe_sendbuf_to_service(unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, const SERVICES_ID &arg4, const SERVICES_ID &arg5, const SERVICES_ID &arg6, const unsigned char *arg7, size_t arg8, unsigned int arg9, unsigned int arg10); // line 124
static void instance(Zerg_MMAP_BusPipe *arg0); // line 131
static Zerg_MMAP_BusPipe * instance(); // line 133
static void clean_instance(); // line 135
int pipe_sendmsg_to_service<sec_proto::LogSvrCommBillRecord>(unsigned int cmd, unsigned int qquin, unsigned int transaction_id, unsigned int backfill_trans_id, const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc, const SERVICES_ID &sendsvc, const /*anon struct*/ int &info, unsigned int app_id, unsigned int option); // line 322
};
extern const char ZERG_PIPENAME[][]; // line 19
extern char send_buffer_[]; // line 28

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_ZERG_MMAPPIPE_H_H_
