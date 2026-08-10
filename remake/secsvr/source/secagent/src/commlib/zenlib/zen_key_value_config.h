// Restored from secagent binary (reverse-engineered, behavior matched).
// Original path: src/commlib/zenlib/zen_key_value_config.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_KEY_VALUE_CONFIG_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_KEY_VALUE_CONFIG_H_H_

#include <map>
#include <string>

// sizeof = 28 (vptr 4 + std::map 24)
struct KeyValueConfig {
    enum IntValueType {
        EVALUE_INT10 = 0,
        EVALUE_INT16 = 1,
    };
public:
    KeyValueConfig();
    virtual ~KeyValueConfig();
    int load_from_file(const char *conf_file_path);
    int get_str(const char *config_name, char *value, unsigned int value_len, const char *default_value);
    int get_int(const char *config_name, int *value, int default_value, KeyValueConfig::IntValueType type);
    int get_short(const char *config_name, short int *value, short int default_value, KeyValueConfig::IntValueType type);
    int get_bool(const char *config_name, bool *value, bool default_value);
private:
    void add_item(const std::string &name, const std::string &value);
    const std::string *get_item_value(const std::string &name) const;
    std::map<std::string, std::string> values_;
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_KEY_VALUE_CONFIG_H_H_
