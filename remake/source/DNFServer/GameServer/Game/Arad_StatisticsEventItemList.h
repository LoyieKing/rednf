// df_game_r Game/ ARAD::Arad_StatisticsEventItemList。
//
// ORIG（df_game_r, elf32-i386）：
//   class ARAD::Arad_StatisticsEventItemList（namespace ARAD）
//   类名 "Arad_StatisticsEventItemList" = 28 字符（mangled _ZN4ARAD28...）。
//   vtable @0x8ba5a78、typeinfo @0x8ba5a90。
//
// ORIG 符号：
//   C1/C2  _ZN4ARAD28Arad_StatisticsEventItemListC1Ev @0x0819cbe8
//   D1     _ZN4ARAD28Arad_StatisticsEventItemListD1Ev @0x0819ccac
//   D0     _ZN4ARAD28Arad_StatisticsEventItemListD0Ev @0x0819cd0e
//   loadScriptData    _ZN4ARAD28Arad_StatisticsEventItemList14loadScriptDataEv @0x0819cd2c
//   clearScriptData   _ZN4ARAD28Arad_StatisticsEventItemList15clearScriptDataEv @0x0819d084
//   needStatistics    _ZN4ARAD28Arad_StatisticsEventItemList14needStatisticsEm @0x0819d09a
//
// ORIG 布局（size = 0x14，initAradData 中 operator new(0x14) 实证）：
//   +0x00  vptr
//   +0x04  std::string m_scriptDataName（基类 Arad_InterfaceGameScript 成员，ctor 传入
//          "Arad_StatisticsEventItemList"）
//   +0x08  std::vector<unsigned long> m_statisticsItemList
//
// vtable 槽位（继承 Arad_InterfaceGameScript）：槽0=D1、槽1=D0、槽2=loadScriptData、
// 槽3=clearScriptData。needStatistics 为非虚普通成员函数。

#ifndef DNF_ARAD_STATISTICSEVENTITEMLIST_H
#define DNF_ARAD_STATISTICSEVENTITEMLIST_H

#include <vector>

#include "Arad_DataManager.h"   // ARAD::Arad_InterfaceGameScript

namespace ARAD
{

class Arad_StatisticsEventItemList : public Arad_InterfaceGameScript
{
public:
    Arad_StatisticsEventItemList();            // ORIG C1/C2 0x0819cbe8
    virtual ~Arad_StatisticsEventItemList();   // ORIG D1 0x0819ccac / D0 0x0819cd0e

    // ORIG 0x0819cd2c（vtable 槽2）：向 m_statisticsItemList 依次 push_back 固定的
    // 30 个统计物品索引后返回 true（ORIG 无前置清空）。
    virtual bool loadScriptData();
    // ORIG 0x0819d084（vtable 槽3）：清空 m_statisticsItemList。
    virtual void clearScriptData();

    // ORIG 0x0819d09a：itemIndex 在 m_statisticsItemList 中则返回 true（std::find）。
    bool needStatistics(unsigned long itemIndex);

private:
    std::vector<unsigned long> m_statisticsItemList;   // +0x08
};

}  // namespace ARAD

#endif  // DNF_ARAD_STATISTICSEVENTITEMLIST_H
