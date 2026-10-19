// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_stat_monitor.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_STAT_MONITOR_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_STAT_MONITOR_H_H_

#include <stddef.h>

// sizeof = 140
struct Comm_Stat_Monitor : public ZEN_Server_Status { // line 9
private:
static const size_t STAT_MMAP_FILENAME_LEN;
protected:
char stat_mmap_filename_[];
static Comm_Stat_Monitor *instance_;
public:
Comm_Stat_Monitor(const Comm_Stat_Monitor &arg0);
protected:
Comm_Stat_Monitor(); // line 70
virtual ~Comm_Stat_Monitor(); // line 75
void create_stat_fname(bool if_zerg, const SERVICES_ID &service_info); // line 51
public:
int initialize(bool if_zerg, const SERVICES_ID &service_info, size_t num_stat_item, const ZEN_STATUS_ITEM_WITHNAME *item_ary, bool mutli_thread); // line 33
static int get_svr_info_from_stat_fname(const char *stat_file_name, SERVICES_ID *service_info, bool *if_zerg); // line 81
static Comm_Stat_Monitor * instance(); // line 11
static void clean_instance(); // line 21
};
extern const size_t STAT_MMAP_FILENAME_LEN; // line 12

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_STAT_MONITOR_H_H_
