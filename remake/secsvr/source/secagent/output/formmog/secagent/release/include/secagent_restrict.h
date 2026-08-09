// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_restrict.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_RESTRICT_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_RESTRICT_H_H_

#include "output/commlib/zenlib/release/include/zen_digit_list.h"

// sizeof = 28
struct SecagentRestrict { // line 13
private:
bool is_use_restrict_;
DigitList allow_login_list_;
public:
SecagentRestrict(); // line 15
~SecagentRestrict(); // line 16
int init(const char *arg0); // line 28
int reload(const char *arg0); // line 30
bool is_allow_login(unsigned int arg0); // line 33
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_RESTRICT_H_H_
