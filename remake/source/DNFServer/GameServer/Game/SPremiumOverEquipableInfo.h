// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + CUserPremium.cpp
// SPremiumOverEquipableInfo：ORIG 弱内联 D1 W @0x08939b4e（隐式析构）。
// has_def=1：完整复制定义自 CUserPremium.cpp:49（POD：int 组内标签数 + 26 槽 int 标签
// 分类值，仅前 22 槽写入；sizeof 0x6c 断言随定义迁入）。
#ifndef DNF_GAME_SPREMIUMOVEREQUIPABLEINFO_H_
#define DNF_GAME_SPREMIUMOVEREQUIPABLEINFO_H_

struct SPremiumOverEquipableInfo
{
    int m_count;        // +0x00 组内标签数（ScanInt 失败前已写入）
    int m_slots[26];    // +0x04 标签分类值（[hat avatar]=0 .. [magic stone]=0x15）
};  // memset 0x6c；仅前 22 槽被写入
typedef char spremiumoverequipableinfo_size_0x6c
    [(sizeof(SPremiumOverEquipableInfo) == 0x6c) ? 1 : -1];

#endif  // DNF_GAME_SPREMIUMOVEREQUIPABLEINFO_H_
