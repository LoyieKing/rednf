// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + CUserPremium.cpp 局部定义
// SPremiumReturnItemInfo：premium 返回物品组信息。
// 定义源：Game/CUserPremium.cpp:57（含 sizeof 0x2c 断言）。
#ifndef DNF_GAME_SPREMIUMRETURNITEMINFO_H_
#define DNF_GAME_SPREMIUMRETURNITEMINFO_H_

struct SPremiumReturnItemInfo
{
    int m_count;        // +0x00 组内 int 数
    int m_items[10];    // +0x04..+0x2b
};  // memset 0x2c

typedef char spremiumreturniteminfo_size_0x2c
    [(sizeof(SPremiumReturnItemInfo) == 0x2c) ? 1 : -1];

// ORIG 另有析构 W @0x08939e04（_ZN23SPremiumReturnItemInfoD1Ev）；
// 平凡成员场景由隐式析构提供，无需显式声明。

#endif  // DNF_GAME_SPREMIUMRETURNITEMINFO_H_
