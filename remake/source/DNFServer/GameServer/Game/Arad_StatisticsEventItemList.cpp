// df_game_r Game/ ARAD::Arad_StatisticsEventItemList 实现。
//
// 依据 docs/class_func_reports/ARAD__Arad_StatisticsEventItemList/*（ORIG elf32 反汇编
// + Ghidra C）。loadScriptData 为固定 30 个物品索引的 push_back 序列，逐项忠实转写。
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
    // 声明序与赋值序分离：槽位布局（idx29→-0xc … idx0→-0x80）与执行流均忠实 ORIG。
    unsigned long idx29, idx28, idx27, idx26, idx25, idx24, idx23, idx22, idx21, idx20;
    unsigned long idx19, idx18, idx17, idx16, idx15, idx14, idx13, idx12, idx11, idx10;
    unsigned long idx9, idx8, idx7, idx6, idx5, idx4, idx3, idx2, idx1, idx0;

    idx0 = 0x28d3aa;  m_statisticsItemList.push_back(idx0);
    idx1 = 0x28d3a4;  m_statisticsItemList.push_back(idx1);
    idx2 = 0x6856;    m_statisticsItemList.push_back(idx2);
    idx3 = 0x6857;    m_statisticsItemList.push_back(idx3);
    idx4 = 0x6858;    m_statisticsItemList.push_back(idx4);
    idx5 = 0x6859;    m_statisticsItemList.push_back(idx5);
    idx6 = 0x685a;    m_statisticsItemList.push_back(idx6);
    idx7 = 0x685b;    m_statisticsItemList.push_back(idx7);
    idx8 = 0x685c;    m_statisticsItemList.push_back(idx8);
    idx9 = 0x1d98;    m_statisticsItemList.push_back(idx9);
    idx10 = 0x28d435; m_statisticsItemList.push_back(idx10);
    idx11 = 0x28d436; m_statisticsItemList.push_back(idx11);
    idx12 = 0x28d437; m_statisticsItemList.push_back(idx12);
    idx13 = 0x28d438; m_statisticsItemList.push_back(idx13);
    idx14 = 0x28d439; m_statisticsItemList.push_back(idx14);
    idx15 = 0x1d95;   m_statisticsItemList.push_back(idx15);
    idx16 = 0x28d14d; m_statisticsItemList.push_back(idx16);
    idx17 = 0x28cdad; m_statisticsItemList.push_back(idx17);
    idx18 = 0x28cdac; m_statisticsItemList.push_back(idx18);
    idx19 = 0x28d487; m_statisticsItemList.push_back(idx19);
    idx20 = 0x28d4bf; m_statisticsItemList.push_back(idx20);
    idx21 = 0x2a445c; m_statisticsItemList.push_back(idx21);
    idx22 = 0x2a4a9c; m_statisticsItemList.push_back(idx22);
    idx23 = 0x2a7026; m_statisticsItemList.push_back(idx23);
    idx24 = 0x2a639c; m_statisticsItemList.push_back(idx24);
    idx25 = 0x2a50dc; m_statisticsItemList.push_back(idx25);
    idx26 = 0x2a571c; m_statisticsItemList.push_back(idx26);
    idx27 = 0x2a7660; m_statisticsItemList.push_back(idx27);
    idx28 = 0x2a5d63; m_statisticsItemList.push_back(idx28);
    idx29 = 0x2a69dd; m_statisticsItemList.push_back(idx29);
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
