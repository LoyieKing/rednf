// 还原实现：comm_stat_monitor.cpp（语义对照 gunnersvr oracle 0x0807cd80-0x0807d540）
#include "src/commlib/framework/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/commlib/zenlib/release/include/zen_server_status.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_string.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_stat_define.h"

Comm_Stat_Monitor *Comm_Stat_Monitor::instance_ = 0;

// line 70
Comm_Stat_Monitor::Comm_Stat_Monitor()
    : ZEN_Server_Status() {
}

// line 75
Comm_Stat_Monitor::~Comm_Stat_Monitor() {
}

// line 21
void Comm_Stat_Monitor::clean_instance() {
    if (instance_ != 0) {
        delete instance_;
        instance_ = 0;
    }
}

// line 81（错误码 0x142e 为 oracle 常量，宏名不在 DWARF 中，保留字面值）
int Comm_Stat_Monitor::get_svr_info_from_stat_fname(const char *stat_file_name,
                                                    SERVICES_ID *service_info,
                                                    bool *if_zerg) {
    bool tmp_if_zerg = true;
    const char *scan_format = "ZERG_%d_%d.STAT";
    SERVICES_ID tmp_service_info(0, 0);
    if (strncmp(stat_file_name, "ZERG", 4) != 0) {
        if (strncmp(stat_file_name, "TSS", 3) != 0) {
            return 0x142e;
        }
        tmp_if_zerg = false;
        scan_format = "TSS_%d_%d.STAT";
    }
    int ret = sscanf(stat_file_name, scan_format,
                     &tmp_service_info.services_type_,
                     &tmp_service_info.services_id_);
    if (ret != 2) {
        return 0x142e;
    }
    if (if_zerg) {
        *if_zerg = tmp_if_zerg;
    }
    *service_info = tmp_service_info;
    return 0;
}

// line 51
void Comm_Stat_Monitor::create_stat_fname(bool if_zerg,
                                          const SERVICES_ID &service_info) {
    snprintf(stat_mmap_filename_, STAT_MMAP_FILENAME_LEN,
             if_zerg ? "ZERG_%d_%d.STAT" : "TSS_%d_%d.STAT",
             service_info.services_type_, service_info.services_id_);
    stat_mmap_filename_[STAT_MMAP_FILENAME_LEN] = 0;
}

// line 33
int Comm_Stat_Monitor::initialize(bool if_zerg, const SERVICES_ID &service_info,
                                  size_t num_stat_item,
                                  const ZEN_STATUS_ITEM_WITHNAME *item_ary,
                                  bool mutli_thread) {
    create_stat_fname(if_zerg, service_info);
    ZEN_OS::strupr(stat_mmap_filename_);
    return ZEN_Server_Status::initialize(stat_mmap_filename_, num_stat_item,
                                         item_ary, mutli_thread);
}

// line 11
Comm_Stat_Monitor *Comm_Stat_Monitor::instance() {
    if (instance_ == 0) {
        instance_ = new Comm_Stat_Monitor;
    }
    return instance_;
}
