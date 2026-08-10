// Auto-generated header stub from DWARF info
// Original path: src/commsvr/gunnersvr/gunner_reporter.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_REPORTER_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_REPORTER_H_H_

#include "output/protocol/common/release/include/comm_conf_gunnersvr.h"
#include "src/commsvr/gunnersvr/gunner_channel_mgr.h"
#include "src/commsvr/gunnersvr/gunner_file_scanner.h"
#include "src/commsvr/gunnersvr/gunner_statinfo_fetcher.h"
#include <time.h>
#include <vector>

// sizeof = 7836
struct GunnerInfoReporter { // line 28
private:
static GunnerInfoReporter *instance_;
GunnerChannelMgr channel_mgr_;
GunnerFileScanner file_scanner_;
GunnerChannelMgr::LPCONFIG config_;
GunnerStatinfoFetcher statinfo_fetcher_;
public:
int init(GunnerChannelMgr::LPCONFIG config); // line 31
int reload(GunnerChannelMgr::LPCONFIG config); // line 33
int report(time_t report_time); // line 36
int report_statinfo_to_monitor(const GunnerStatInfo &stat_info); // line 38
static GunnerInfoReporter * instance(); // line 41
void clean_instance(); // line 43
private:
GunnerInfoReporter(); // line 45
~GunnerInfoReporter(); // line 47
int pack_by_appid(const GunnerStatInfo &stat_info, unsigned int appid_num, const unsigned int *appid, std::vector<sec_proto::MonitorSvrReport> &report_pkgs); // line 52
};

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_REPORTER_H_H_
