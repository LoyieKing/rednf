// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_config_ini_implement.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_INI_IMPLEMENT_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_INI_IMPLEMENT_H_H_

#include "output/commlib/zenlib/release/include/zen_config_property_tree.h"
#include <stddef.h>

// sizeof = 48
struct Zen_INI_PropertyTree : public Zen_Conf_PropertyTree { // line 36
public:
int get_string_value(const string &arg0, const string &arg1, string &arg2) const; // line 41
int get_string_value(const string &arg0, const string &arg1, char *arg2, size_t arg3) const; // line 47
int get_int32_value(const string &arg0, const string &arg1, int32_t &arg2) const; // line 51
int get_uint32_value(const string &arg0, const string &arg1, uint32_t &arg2) const; // line 55
int get_bool_value(const string &arg0, const string &arg1, bool &arg2) const; // line 60
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_INI_IMPLEMENT_H_H_
