// Restored from secagent binary (reverse-engineered, behavior matched).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/zenlib/zen_digit_list.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
//
// 数字列表配置解析：每行一个数字（支持十进制/十六进制），'#' 注释行跳过。
// 裁剪逻辑按原版逐指令复刻：
//   - 尾裁剪：全空白行会保留 1 个字符；
//   - 头裁剪：全空白行再删除 size-1 个字符（原版 bug-for-bug 行为），
//     最终留下 1 个空格字符，会被 strtoul 解析为 0 并插入集合。

#include "src/commlib/zenlib/zen_digit_list.h"
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>

DigitList::DigitList() {
}

DigitList::~DigitList() {
}

int DigitList::load(const char *conf_file_path, DigitList::EDigitType type) {
    values_.clear();

    std::ifstream fin(conf_file_path, std::ios::in);
    if (!fin) {
        return -1;
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (!line.empty()) {
            // 尾裁剪：跳过行尾 isspace 字符；全空白行时 e 落到 0，
            // erase(1, size-1) 保留首字符。
            unsigned int e = (unsigned int)line.size();
            while (e > 0) {
                --e;
                if ((signed char)line[e] < 0 || !isspace((unsigned char)line[e])) {
                    break;
                }
            }
            line.erase(e + 1, line.size() - (e + 1));

            // 头裁剪：全空白行时原版取 p-1（上一轮起点），保留 1 个字符。
            unsigned int p = 0;
            unsigned int len = (unsigned int)line.size();
            while (p < len && (signed char)line[p] >= 0 && isspace((unsigned char)line[p])) {
                ++p;
            }
            line.erase(0, (p == len) ? p - 1 : p);
        }

        if (line.empty()) {
            continue;
        }
        if (line[0] == '#') {
            continue;
        }
        values_.insert(str_to_digit(line.c_str(), type));
    }

    return 0;
}

bool DigitList::is_exist(unsigned int value) const {
    return values_.find(value) != values_.end();
}

const std::set<unsigned int> &DigitList::get_list() {
    return values_;
}

unsigned int DigitList::str_to_digit(const char *str, DigitList::EDigitType type) {
    if (str == NULL) {
        return (unsigned int)-1;
    }
    if (type == EDT_16) {
        return (unsigned int)strtoul(str, NULL, 16);
    }
    if (type != EDT_10) {
        return (unsigned int)-2;
    }
    return (unsigned int)strtoul(str, NULL, 10);
}
