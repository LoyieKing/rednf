// Restored from binary (gunnersvr) + DWARF type info.
// Original path: src/commlib/zenlib/zen_config_ini_implement.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_INI_IMPLEMENT_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_INI_IMPLEMENT_H_H_

#include "src/commlib/zenlib/zen_config_property_tree.h"
#include <stddef.h>
#include <stdint.h>
#include <string>

// sizeof = 48
struct Zen_INI_PropertyTree : public Zen_Conf_PropertyTree {
public:
    int get_string_value(const std::string &sectionname, const std::string &keyname, std::string &value) const;
    int get_string_value(const std::string &sectionname, const std::string &keyname, char *buf, size_t size_buf) const;
    int get_int32_value(const std::string &sectionname, const std::string &keyname, int32_t &value) const;
    int get_uint32_value(const std::string &sectionname, const std::string &keyname, uint32_t &value) const;
    int get_bool_value(const std::string &sectionname, const std::string &keyname, bool &value) const;
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_INI_IMPLEMENT_H_H_
