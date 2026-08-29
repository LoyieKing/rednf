// ============================================================================
// ItemFarmingData.h — 农田升级权重条目（独占头）
//
// ORIG 弱符号 W（df_game_r 实证）：
//   _ZN15ItemFarmingDataC1Ev      0x08a8cdb0（C2 同址）默认构造
//   _ZN15ItemFarmingDataC1ERKS_   0x0853ac12（C2 同址）拷贝构造
//   _ZN15ItemFarmingDataD1Ev      0x083cd0ba（D2 同址）析构
// 仅此三个函数（nm 别名计 6 条），无 operator= / 无其它方法 / 无虚函数。
//
// 布局：{ std::vector<std::pair<int,int>> +0x00; int +0x0c }，sizeof 0x10。
// C1 反汇编：vector<pair<int,int>>::C1 → movl $0,0xc → vector::clear
//   （源码含 clear() 调用，故带 unwind 清理路径）；C1ERKS：vector 拷贝构造
//   + 0xc int 拷贝；D1 仅 vector<pair<int,int>>::D1。
// 使用点：GenerateRandomUpgrade::generateUpgrade（0x08533676）以拷贝构造
//   local_50 ← map 元素 pair+4，随后对 +0x00 调 vector<pair<int,int>>::operator[]。
// ============================================================================

#ifndef DNF_GAME_ITEMFARMINGDATA_H
#define DNF_GAME_ITEMFARMINGDATA_H

#include <utility>
#include <vector>

struct ItemFarmingData
{
    ItemFarmingData() : m_vecWeight(), m_active(0) { m_vecWeight.clear(); }
    ItemFarmingData(const ItemFarmingData& o)
        : m_vecWeight(o.m_vecWeight), m_active(o.m_active) {}
    ~ItemFarmingData() {}

    std::vector<std::pair<int, int> > m_vecWeight;  // +0x00（升级条目 {itemId, weight}）
    int m_active;                                   // +0x0c（==0 视为未启用）
};

#endif  // DNF_GAME_ITEMFARMINGDATA_H
