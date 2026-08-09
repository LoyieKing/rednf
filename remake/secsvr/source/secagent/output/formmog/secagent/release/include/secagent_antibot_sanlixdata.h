// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_antibot_sanlixdata.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_SANLIXDATA_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_SANLIXDATA_H_H_

#include <time.h>

// sizeof = 32744
struct AntibotSanlixDataInfo { // line 13
public:
bool is_read_succ_;
unsigned int data_len_;
unsigned char data_[];
time_t last_modify_time_;
time_t load_time_;
};
// sizeof = 130976
struct AntibotSanlixData { // line 31
private:
AntibotSanlixDataInfo te_data_;
AntibotSanlixDataInfo ts_data_;
AntibotSanlixDataInfo tk_data_;
AntibotSanlixDataInfo tenparty_data_;
public:
AntibotSanlixData(); // line 33
~AntibotSanlixData(); // line 34
int load(); // line 37
const AntibotSanlixDataInfo & get_tedata() const; // line 40
const AntibotSanlixDataInfo & get_tsdata() const; // line 43
const AntibotSanlixDataInfo & get_tkdata() const; // line 46
const AntibotSanlixDataInfo & get_tenpartydata() const; // line 49
private:
int load_sanlix_data(const char *arg0, AntibotSanlixDataInfo *arg1); // line 52
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_ANTIBOT_SANLIXDATA_H_H_
