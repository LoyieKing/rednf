// df_game_r Game/ ARAD::Arad_StatisticsEventItemList 实现。
//
// 依据 docs/class_func_reports/ARAD__Arad_StatisticsEventItemList/*（ORIG elf32 反汇编
// + Ghidra C）。loadScriptData 为固定 30 个物品索引的 push_back 序列，逐项忠实转写。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN4ARAD28Arad_StatisticsEventItemListC2Ev`（ORIG 0x819d16f）；ctor 已是首强符号。
#include <iostream>

#include "Arad_StatisticsEventItemList.h"

#include <algorithm>
#include <utility>

namespace ARAD
{

Arad_StatisticsEventItemList::Arad_StatisticsEventItemList()
    : Arad_InterfaceGameScript("Arad_StatisticsEventItemList")
{
    // ORIG 0x819cbe8：基类构造后 vector<unsigned long>(this+0x08) 默认构造。
}

Arad_StatisticsEventItemList::~Arad_StatisticsEventItemList()
{
    // ORIG D1 0x819ccac：置 vptr 后析构 +0x08 vector，再析构基类。
    // ORIG D0 0x819cd0e：转发 D1 后 operator delete。
}

bool Arad_StatisticsEventItemList::loadScriptData()
{
    // b28d-solo-Arad_StatisticsEventItemList: AE（.o 阶段上限；call 目标已对齐）。
    // ORIG 0x0819cd2c：30 个字面量右值绑定 push_back(unsigned long&&)（EOm 右值重载）；
    // 每个右值物化到独立栈槽（-0x80 … -0xc）后取址传参（P0 限定①：匿名临时槽）。
    // 旧形为 30 个具名局部左值 ⇒ 选 push_back(const unsigned long&)（ERKm），
    // 除 30 条 call 目标外逐条全等（ndiff=30）；改右值后 TU diff=0、p167 30/30 CLEAR。
    // 反例记录：static_cast<unsigned long&&>(具名局部) 会被 4.4 多物化一份临时槽
    // （帧 0x98→0x108、槽 -0xf8… 全漂），不可用；std::move 会引入 ORIG 无的 call（P234）。
    m_statisticsItemList.push_back(0x28d3aa);
    m_statisticsItemList.push_back(0x28d3a4);
    m_statisticsItemList.push_back(0x6856);
    m_statisticsItemList.push_back(0x6857);
    m_statisticsItemList.push_back(0x6858);
    m_statisticsItemList.push_back(0x6859);
    m_statisticsItemList.push_back(0x685a);
    m_statisticsItemList.push_back(0x685b);
    m_statisticsItemList.push_back(0x685c);
    m_statisticsItemList.push_back(0x1d98);
    m_statisticsItemList.push_back(0x28d435);
    m_statisticsItemList.push_back(0x28d436);
    m_statisticsItemList.push_back(0x28d437);
    m_statisticsItemList.push_back(0x28d438);
    m_statisticsItemList.push_back(0x28d439);
    m_statisticsItemList.push_back(0x1d95);
    m_statisticsItemList.push_back(0x28d14d);
    m_statisticsItemList.push_back(0x28cdad);
    m_statisticsItemList.push_back(0x28cdac);
    m_statisticsItemList.push_back(0x28d487);
    m_statisticsItemList.push_back(0x28d4bf);
    m_statisticsItemList.push_back(0x2a445c);
    m_statisticsItemList.push_back(0x2a4a9c);
    m_statisticsItemList.push_back(0x2a7026);
    m_statisticsItemList.push_back(0x2a639c);
    m_statisticsItemList.push_back(0x2a50dc);
    m_statisticsItemList.push_back(0x2a571c);
    m_statisticsItemList.push_back(0x2a7660);
    m_statisticsItemList.push_back(0x2a5d63);
    m_statisticsItemList.push_back(0x2a69dd);
    return true;
}

void Arad_StatisticsEventItemList::clearScriptData()
{
    // ORIG 0x0819d084：vector<unsigned long>::clear(this+0x08)。
    m_statisticsItemList.clear();
}

bool Arad_StatisticsEventItemList::needStatistics(unsigned long itemIndex)
{
    // ORIG 0x0819d09a：std::find(begin, end, itemIndex) != end()；
    // ORIG 尾部为显式 0/1 归一（test/je/mov 形态），以 if 分支返回匹配。
    if (std::find(m_statisticsItemList.begin(), m_statisticsItemList.end(),
                  itemIndex) != m_statisticsItemList.end())
        return true;
    return false;
}

}  // namespace ARAD
