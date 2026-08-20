#pragma once

#include <utility>
#include <vector>

// 地图脚本中的怪物配置记录。
// 字段名称保留原始偏移命名，便于与反汇编访问及 CMap 转换逻辑对应。
struct STAssignMonster
{
    STAssignMonster();
    STAssignMonster(const STAssignMonster& other);
    ~STAssignMonster();
    STAssignMonster& operator=(const STAssignMonster& other);
    void Clear();

    int m_0;                                // +0x00
    char m_4;                               // +0x04
    int m_8;                                // +0x08
    int m_c;                                // +0x0c
    int m_10;                               // +0x10
    int m_14;                               // +0x14
    int m_18;                               // +0x18
    char m_1c;                              // +0x1c
    int m_20;                               // +0x20
    int m_24;                               // +0x24
    std::vector<int> m_vec28;               // +0x28
    int m_34;                               // +0x34
    int m_38;                               // +0x38
    int m_3c;                               // +0x3c
    std::vector<std::pair<int, int> > m_vec40; // +0x40
    int m_4c;                               // +0x4c
    int m_50;                               // +0x50
    char m_54;                              // +0x54
    char m_55;                              // +0x55
    int m_58;                               // +0x58
    std::vector<unsigned char> m_vec5c;     // +0x5c
};
