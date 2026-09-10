// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CReliablePerson/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15CReliablePerson13InitTradeGoldEjjj`（ORIG 0x86454a8）。
#include <iostream>

// CReliablePerson — 交易金币可信度统计（ORIG 0x86453b8..0x86454a8；
// 权威参照 docs/class_func_reports/CReliablePerson.md）
// 布局（+0x00..+0x0f）：
//   +0x00 total（累计交易金；GetTradeGoldTotal；>1e9 时溢出到 billion）
//   +0x04 billion（累计溢出的十亿位；GetTradeGoldTotalBillion）
//   +0x08 daily（日交易金；GetTradeGoldDaily / SetTradeGoldDaily）
//   +0x0c schoolPoint（CUserCharacInfo.h 侧声明）
// 原始工程中 3 个 Get* + SetTradeGoldDaily 为头内联（ORIG W），
// 此处统一落地为本 TU 定义（符号体逐字节一致）。

class CReliablePerson
{
public:
    void InitTradeGold(unsigned int daily, unsigned int billion,
                       unsigned int total);
    unsigned int GetTradeGoldDaily() const;
    unsigned int GetTradeGoldTotal() const;
    unsigned int GetTradeGoldTotalBillion() const;
    void SetTradeGoldDaily(unsigned int gold);
    void IncrementTradeGold(unsigned int gold);

    unsigned int m_totalTradeGold;   // +0x00
    unsigned int m_billionTradeGold; // +0x04
    unsigned int m_dailyTradeGold;   // +0x08
    int m_schoolPoint;              // +0x0c
};

#include "CReliablePerson/InitTradeGold.inc"
                                    unsigned int daily)
{
    m_totalTradeGold = total;
    m_billionTradeGold = billion;
    m_dailyTradeGold = daily;
}

#include "CReliablePerson/GetTradeGoldDaily.inc"
{
    return m_dailyTradeGold;
}

#include "CReliablePerson/GetTradeGoldTotal.inc"
{
    return m_totalTradeGold;
}

#include "CReliablePerson/GetTradeGoldTotalBillion.inc"
{
    return m_billionTradeGold;
}

#include "CReliablePerson/SetTradeGoldDaily.inc"
{
    m_dailyTradeGold = gold;
}

#include "CReliablePerson/IncrementTradeGold.inc"
{
    m_dailyTradeGold += gold;
    m_totalTradeGold += gold;
    if (m_totalTradeGold > 999999999)
    {
        unsigned int total = m_totalTradeGold;
        // 累计交易额超过 10 亿：十亿位进位到 billion，total 保留余数
        m_totalTradeGold = total - total / 1000000000 * 1000000000;
        m_billionTradeGold += total / 1000000000;
    }
}
