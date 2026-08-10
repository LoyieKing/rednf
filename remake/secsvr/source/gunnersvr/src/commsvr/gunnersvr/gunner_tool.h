// Reconstructed from gunnersvr DWARF + disassembly.
// sizeof = 180：file_name_[129] + 填充(3) + statinfo_fetcher_(24) + 5 个标量 = 180。
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TOOL_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TOOL_H_H_

#include "src/commsvr/gunnersvr/gunner_statinfo_fetcher.h"

#include <stdint.h>

// sizeof = 180
struct GunnerTool { // line 25
private:
    static GunnerTool *instance_;               // line 35
    static const int MAX_SHM_FILE_NAME_LEN = 128; // line 65
    static const int MAX_TIME_STR_LEN = 64;     // line 67
    char file_name_[MAX_SHM_FILE_NAME_LEN + 1]; // +0 (129)
    GunnerStatinfoFetcher statinfo_fetcher_;    // +132 (24)
    int server_type_;                           // +156
    int server_id_;                             // +160
    unsigned int app_id_;                       // +164
    uint32_t feature_id_;                       // +168
    uint64_t feature_vaule_;                    // +172
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

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_TOOL_H_H_
