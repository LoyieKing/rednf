// Auto-generated header stub from DWARF info
// Original path: src/commsvr/gunnersvr/gunner_tool.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TOOL_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TOOL_H_H_

#include "src/commsvr/gunnersvr/gunner_statinfo_fetcher.h"
#include <stdint.h>

// sizeof = 180
struct GunnerTool { // line 25
private:
static GunnerTool *instance_;
static const int MAX_SHM_FILE_NAME_LEN;
static const int MAX_TIME_STR_LEN;
char file_name_[];
GunnerStatinfoFetcher statinfo_fetcher_;
int server_type_;
int server_id_;
unsigned int app_id_;
uint32_t feature_id_;
uint64_t feature_vaule_;
public:
static GunnerTool * instance(); // line 27
int proc_cmd(int argc, const char **argv); // line 30
void clean_instance(); // line 32
private:
GunnerTool(); // line 38
~GunnerTool(); // line 40
void init(); // line 53
int analyze_cmd(int argc, const char **argv); // line 55
void usage(); // line 57
int print_monitor_info(); // line 59
void print_update_time(); // line 61
int report_gunner_info(); // line 63
};
extern const int MAX_SHM_FILE_NAME_LEN; // line 65
extern const int MAX_TIME_STR_LEN; // line 67

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TOOL_H_H_
