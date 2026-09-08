// CFieldLengthChecker.cpp — ORIG 转写（df_game_r elf32-i386）
// AddField 双分支：静态 fieldLength_ 空 → 解析字段定义（'(' ')' ';' 分隔，填充
// 静态 fieldNames_/fieldLength_）；实例成员空 → 解析 "values(" 数据行，按列
// 更新静态 fieldLength_ 为实测最大长度（'...' 引号值去转义、trim）。
#include <iostream>
#include "CFieldLengthChecker.h"
#include "CEnvironment.h"

std::vector<std::string> CFieldLengthChecker::fieldNames_;
std::vector<unsigned int> CFieldLengthChecker::fieldLength_;

CFieldLengthChecker::CFieldLengthChecker()
{
}

CFieldLengthChecker::~CFieldLengthChecker()
{
}

void CFieldLengthChecker::AddField(char const* query)
{
    if (fieldLength_.empty()) {
        std::string s(query);
        std::string a;
        std::string b;
        std::string c;
        // [b28d-56 判词·P167] ORIG 0x0817? 六处调用 `find(char const*, size_type)`
        // （`_ZNKSs4findEPKcj`），旧码 `find(char)` 选 `find(char, size_type)`
        // 重载 ⇒ p167 MISSING/EXTRA 各 6。改为字符串字面量实参。
        size_t pos = s.find("(");
        a = s.substr(0, pos);
        b = s.substr(pos + 1, std::string::npos);
        pos = b.find(")");
        c = b.substr(0, pos);
        while ((pos = c.find(";")) != std::string::npos) {
            fieldNames_.push_back(c.substr(0, pos));
            c = c.substr(pos + 1, std::string::npos);
            fieldLength_.push_back(0);
        }
        fieldNames_.push_back(c);
        fieldLength_.push_back(0);
    }
    if (m_fieldNames_.empty()) {
        std::string s(query);
        std::string a;
        std::string b;
        std::string c;
        size_t pos = s.find("values(");
        a = s.substr(0, pos);
        b = s.substr(pos + 7, std::string::npos);
        pos = b.find(")");
        c = b.substr(0, pos);
        for (int i = 0; i < fieldLength_.size() && (pos = c.find(";")) != std::string::npos; ++i) {
            std::string val;
            if (c[0] == '\'') {
                pos = 0;
                do {
                    pos = c.find("'", pos + 1);
                    if (pos == std::string::npos)
                        return;
                } while (c[pos - 1] == '\\');
                ++pos;
                val = c.substr(1, pos - 2);
            } else {
                val = "";
            }
            trim(val);
            if (fieldLength_[i] < val.length()) {
                fieldLength_[i] = val.length();
            }
            c = c.substr(pos + 1, std::string::npos);
        }
    }
}
