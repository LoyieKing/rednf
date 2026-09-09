#pragma once

#include <utility>
#include <vector>

// 8 字节 POD 双 int 视图（P246）：ORIG 4.1 在 C1ERKS_/aSERKS_ 对该段发
// 「dest 缓存 %ecx + 高低半读交错」的整块拷贝（0x83776e5 / 0x8377833），
// 4.4 对两个独立标量成员恒逐条重载 this ⇒ 必须用 union 具名成员整体赋值；
// 同时保留 m_arr 数组视角供 Clear 的 scaled 循环（0x836c5a5 `add $0xc,%edx`）。
union STAssignMonsterInt2
{
    int m_arr[2];
    struct { int m_lo, m_hi; } m_pair;
};

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
    STAssignMonsterInt2 m_pair18;           // +0x18..+0x1f（m_pair.m_lo=m_18, m_pair.m_hi=m_1c）
    int m_20;                               // +0x20
    int m_24;                               // +0x24
    std::vector<int> m_vec28;               // +0x28
    int m_34;                               // +0x34
    STAssignMonsterInt2 m_pair38;           // +0x38..+0x3f（m_arr[0..1]=m_38[0..1]）

    std::vector<std::pair<int, int> > m_vec40; // +0x40
    int m_4c;                               // +0x4c
    int m_50;                               // +0x50
    char m_54;                              // +0x54
    char m_55;                              // +0x55
    int m_58;                               // +0x58
    std::vector<unsigned char> m_vec5c;     // +0x5c
};
