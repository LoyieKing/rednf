// Restored from secagent binary (reverse-engineered, behavior matched).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/zenlib/zen_key_value_config.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
//
// 键值配置解析：每行 "key value"（空白切词，恰好 2 词才接受），
// '#' 注释行跳过；词级裁剪后任一为空则跳过该行，否则覆盖写入 map。
// 裁剪逻辑按原版逐指令复刻（含全空白行的尾裁剪保留首字符怪癖）。

#include "src/commlib/zenlib/zen_key_value_config.h"
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

KeyValueConfig::KeyValueConfig() {
}

KeyValueConfig::~KeyValueConfig() {
}

int KeyValueConfig::load_from_file(const char *conf_file_path) {
    values_.clear();

    std::ifstream fin(conf_file_path, std::ios::in);
    if (!fin) {
        return -1;
    }

    std::string line;
    std::vector<std::string> vec;
    while (std::getline(fin, line)) {
        // 行级裁剪：尾裁剪（全空白行保留首字符）+ 头裁剪（正确实现，全删）。
        if (!line.empty()) {
            unsigned int e = (unsigned int)line.size();
            while (e > 0) {
                --e;
                if ((signed char)line[e] < 0 || !isspace((unsigned char)line[e])) {
                    break;
                }
            }
            line.erase(e + 1, line.size() - (e + 1));

            unsigned int p = 0;
            unsigned int len = (unsigned int)line.size();
            while (p < len && (signed char)line[p] >= 0 && isspace((unsigned char)line[p])) {
                ++p;
            }
            line.erase(0, p);
        }
        // 第二遍裁剪：去掉行首/行尾的 '\r'。
        size_t r = line.find_last_not_of("\r", std::string::npos, 1);
        line.erase(r + 1, line.size() - (r + 1));
        size_t f = line.find_first_not_of("\r", 0, 1);
        if (f > line.size()) {
            f = line.size();
        }
        line.erase(0, f);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        vec.clear();
        // 空白切词（高字节字符视作词内字符，不切分）。
        const char *word_start = line.c_str();
        const char *pos = word_start;
        const char *end = pos + line.size();
        while (pos < end) {
            if ((signed char)*pos >= 0 && isspace((unsigned char)*pos)) {
                if (pos > word_start) {
                    vec.push_back(std::string(word_start, pos - word_start));
                }
                ++pos;
                word_start = pos;
                continue;
            }
            ++pos;
        }
        if (pos > word_start) {
            vec.push_back(std::string(word_start, pos - word_start));
        }

        if (vec.size() == 2) {
            // 逐词裁剪（同行级：尾裁剪 bug 保留 + 头裁剪全删）。
            for (unsigned int k = 0; k < 2; ++k) {
                std::string &word = vec[k];
                if (!word.empty()) {
                    unsigned int e = (unsigned int)word.size();
                    while (e > 0) {
                        --e;
                        if ((signed char)word[e] < 0 || !isspace((unsigned char)word[e])) {
                            break;
                        }
                    }
                    word.erase(e + 1, word.size() - (e + 1));

                    unsigned int p = 0;
                    unsigned int len = (unsigned int)word.size();
                    while (p < len && (signed char)word[p] >= 0 && isspace((unsigned char)word[p])) {
                        ++p;
                    }
                    word.erase(0, p);
                }
            }
            if (!vec[0].empty() && !vec[1].empty()) {
                add_item(vec[0], vec[1]);
            }
        } else {
            printf("bad config line in file %s. line=%s\n", conf_file_path, line.c_str());
        }
    }

    return 0;
}

int KeyValueConfig::get_str(const char *config_name, char *value,
                            unsigned int value_len, const char *default_value) {
    if (config_name == NULL || value == NULL || value_len == 0) {
        return -1;
    }
    memset(value, 0, value_len);

    const std::string *item_value = get_item_value(std::string(config_name));
    if (item_value == NULL) {
        if (default_value != NULL) {
            size_t len = strlen(default_value);
            size_t n = (value_len <= len) ? value_len - 1 : len;
            memcpy(value, default_value, n);
            value[n] = 0;
        }
        return 0;
    }

    size_t len = item_value->length();
    if (value_len <= len) {
        return -2;
    }
    memcpy(value, item_value->c_str(), len);
    value[len] = 0;
    return 0;
}

int KeyValueConfig::get_int(const char *config_name, int *value,
                            int default_value, KeyValueConfig::IntValueType type) {
    if (config_name == NULL || value == NULL) {
        return -1;
    }

    const std::string *item_value = get_item_value(std::string(config_name));
    if (item_value == NULL || (type != EVALUE_INT10 && type != EVALUE_INT16)) {
        *value = default_value;
    } else if (type == EVALUE_INT16) {
        sscanf(item_value->c_str(), "%x", value);
    } else {
        sscanf(item_value->c_str(), "%d", value);
    }
    return 0;
}

int KeyValueConfig::get_short(const char *config_name, short int *value,
                              short int default_value, KeyValueConfig::IntValueType type) {
    int item_value = 0;
    int ret = get_int(config_name, &item_value, (int)default_value, type);
    if (ret == 0) {
        *value = (short int)item_value;
    }
    return ret;
}

int KeyValueConfig::get_bool(const char *config_name, bool *value, bool default_value) {
    int item_value = 0;
    int ret = get_int(config_name, &item_value, default_value ? 1 : 0, EVALUE_INT10);
    if (ret == 0) {
        *value = (item_value != 0);
    }
    return ret;
}

void KeyValueConfig::add_item(const std::string &name, const std::string &value) {
    std::map<std::string, std::string>::iterator it = values_.lower_bound(name);
    if (it == values_.end() || name < it->first) {
        values_.insert(it, std::make_pair(name, value));
    } else {
        it->second.assign(value);
    }
}

const std::string *KeyValueConfig::get_item_value(const std::string &name) const {
    std::map<std::string, std::string>::const_iterator it = values_.find(name);
    if (it == values_.end()) {
        return NULL;
    }
    return &it->second;
}
