// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_config_ini_implement.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_CONFIG_INI_IMPLEMENT_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_CONFIG_INI_IMPLEMENT_H_H_

#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_config_property_tree.h"
#include <stddef.h>

// sizeof = 48
struct ZenINIPropertyTree : public ZenConfPropertyTree { // line 36
public:
int get_string_value(const string &arg0, const string &arg1, string &arg2) const; // line 41
int get_string_value(const string &arg0, const string &arg1, char *arg2, size_t arg3) const; // line 47
int get_int32_value(const string &arg0, const string &arg1, int32_t &arg2) const; // line 51
int get_uint32_value(const string &arg0, const string &arg1, uint32_t &arg2) const; // line 55
int get_bool_value(const string &arg0, const string &arg1, bool &arg2) const; // line 60
ZenINIPropertyTree();
~ZenINIPropertyTree();
};
// sizeof = 1
struct ZEN_INI_Implemention : public ZEN_NON_Copyable { // line 12
public:
ZEN_INI_Implemention(); // line 14
~ZEN_INI_Implemention(); // line 15
int read(const char *arg0, ZenConfPropertyTree &arg1); // line 18
int write(const char *arg0, ZenConfPropertyTree &arg1); // line 21
};

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_CONFIG_INI_IMPLEMENT_H_H_
