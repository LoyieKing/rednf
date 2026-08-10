// Restored from gunnersvr binary (reverse-engineered, behavior matched).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_config_ini_implement.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_config_ini_implement.h"
#include "src/commlib/zenlib/zen_config_property_tree.h"
#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include <cstring>
#include <fstream>
#include <string>

namespace ZEN_OS {
char *strtrim(char *str);
}

struct ZEN_INI_Implemention : public ZEN_NON_Copyable {
    ZEN_INI_Implemention();
    ~ZEN_INI_Implemention();
    int read(const char *file_name, Zen_Conf_PropertyTree &propertytree);
    int write(const char *file_name, Zen_Conf_PropertyTree &propertytree);
};

ZEN_INI_Implemention::ZEN_INI_Implemention()
{
}

ZEN_INI_Implemention::~ZEN_INI_Implemention()
{
}

int ZEN_INI_Implemention::read(const char *file_name, Zen_Conf_PropertyTree &propertytree)
{
    const size_t LINE_BUFFER_LEN = 0x2000;
    char one_line[LINE_BUFFER_LEN];
    char str_key[LINE_BUFFER_LEN];
    char str_value[LINE_BUFFER_LEN];
    std::string cur_section;
    std::ifstream cfgfile(file_name);

    if (cfgfile.rdstate() & (std::ios::failbit | std::ios::badbit))
        return -1;

    while (!(cfgfile.rdstate() & (std::ios::failbit | std::ios::badbit))) {
        cfgfile.getline(one_line, LINE_BUFFER_LEN, cfgfile.widen('\n'));
        ZEN_OS::strtrim(one_line);

        char c = one_line[0];
        if (c == ';' || c == '#')
            continue;

        if (c == '[') {
            size_t len = strlen(one_line) - 1;
            if (one_line[len] == ']') {
                memmove(one_line, one_line + 1, len);
                one_line[strlen(one_line) - 2] = '\0';
                ZEN_OS::strtrim(one_line);
                cur_section.assign(one_line, strlen(one_line));
                std::string section_name(one_line);
                std::string root_path("");
                propertytree.new_child(root_path, section_name);
            }
        }

        char *eq = strchr(one_line, '=');
        if (eq == NULL)
            continue;

        *eq = '\0';
        strncpy(str_key, one_line, LINE_BUFFER_LEN);
        strncpy(str_value, eq + 1, LINE_BUFFER_LEN);
        ZEN_OS::strtrim(str_key);
        ZEN_OS::strtrim(str_value);

        std::string key(str_key);
        std::string value(str_value);
        propertytree.put_leaf_str(cur_section, key, value);
    }
    return 0;
}

int Zen_INI_PropertyTree::get_string_value(const std::string &sectionname, const std::string &keyname, std::string &value) const
{
    return get_leaf_str(sectionname, keyname, value);
}

int Zen_INI_PropertyTree::get_string_value(const std::string &sectionname, const std::string &keyname, char *buf, size_t size_buf) const
{
    return get_leaf_str(sectionname, keyname, buf, size_buf);
}

int Zen_INI_PropertyTree::get_int32_value(const std::string &sectionname, const std::string &keyname, int32_t &value) const
{
    return get_leaf_int(sectionname, keyname, value);
}

int Zen_INI_PropertyTree::get_uint32_value(const std::string &sectionname, const std::string &keyname, uint32_t &value) const
{
    return get_leaf_uint(sectionname, keyname, value);
}

int Zen_INI_PropertyTree::get_bool_value(const std::string &sectionname, const std::string &keyname, bool &value) const
{
    return get_leaf_bool(sectionname, keyname, value);
}
