// 还原头文件：Comm_Stat_Monitor
// 继承 ZEN_Server_Status（vptr+74 字节基类），stat_mmap_filename_[65] 位于 0x4a，
// sizeof = 140（实例分配 0x8c）。虚析构，vtable = [0, typeinfo, D1, D0]。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_STAT_MONITOR_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_STAT_MONITOR_H_H_

#include "output/commlib/zenlib/release/include/zen_server_status.h"
#include "src/commlib/framework/comm_service_info.h"

class Comm_Stat_Monitor : public ZEN_Server_Status { // line 8, sizeof = 140
private:
    static const size_t STAT_MMAP_FILENAME_LEN = 64; // line 12
public:
    Comm_Stat_Monitor(); // line 70
    int initialize(bool if_zerg, const SERVICES_ID &service_info, size_t num_stat_item,
                   const ZEN_STATUS_ITEM_WITHNAME *item_ary, bool mutli_thread); // line 33
    static int get_svr_info_from_stat_fname(const char *stat_file_name,
                                            SERVICES_ID *service_info,
                                            bool *if_zerg); // line 81
    static Comm_Stat_Monitor *instance(); // line 11
    static void clean_instance(); // line 21
protected:
    virtual ~Comm_Stat_Monitor(); // line 75
    void create_stat_fname(bool if_zerg, const SERVICES_ID &service_info); // line 51
    char stat_mmap_filename_[STAT_MMAP_FILENAME_LEN + 1]; // line 46 (0x4a)
    static Comm_Stat_Monitor *instance_; // line 9
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_STAT_MONITOR_H_H_
