// Reconstructed from gunnersvr DWARF + disassembly.
// GunnerStatInfo: SERVICES_ID(6) + copy_time_(4) + vector(12) = 22 -> sizeof 24
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_STATINFO_FETCHER_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_STATINFO_FETCHER_H_H_

#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/commlib/zenlib/release/include/zen_server_status.h"

#include <time.h>
#include <vector>

// sizeof = 24
struct GunnerStatInfo { // line 22
public:
    SERVICES_ID svr_info_;                        // +0 (6)
    time_t copy_time_;                            // +8
    std::vector<ZEN_STATUS_ITEM_WITHNAME> statinfo_array_; // +12 (12)

    // 二进制中 GunnerInfoReporter 构造器内联出 SERVICES_ID(0,0) + 空 vector 的初始化
    // （DWARF 中为 artificial 默认构造器，需显式初始化 svr_info_ 才能编译）。
    GunnerStatInfo() : svr_info_(0, 0) {}
};

// sizeof = 24
struct GunnerStatinfoFetcher { // line 33
private:
    GunnerStatInfo stat_info_;                    // +0
public:
    int get_info(const char *file_name, GunnerStatInfo &stat_info); // line 36
    int dump(const char *file_name); // line 39
    unsigned int dump_timeinfo(const char *file_name); // line 42
private:
    int fetch_svrinfo_from_filename(const char *file_name, GunnerStatInfo &stat_info); // line 44
};

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_STATINFO_FETCHER_H_H_
