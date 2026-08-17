// 本 TU 由 GCC 4.1.2 编译（ORIG .comment 中 4.1.2 x2 之二；A/B 实测 4.1.2
// 对 register bool 比较生成 ORIG 的 setle %al + test %al,%al 形态，
// 4.4.x 各版本均为 setle %bl / jg，无法复现）。
// 其余函数留在 MemoryCashManager.cpp（c6444r 编译，保持已 identical 不回退）。
#include "MemoryCashManager.h"
#include "DNFFileLog.h"

// [DNF-NONIDENTICAL] DNF-MON-NEAR-0009 | monitor | 与ORIG差异=NEAR | CMemoryCashManager::ProcessCashDataPrint | 详见 function_reports/monitor/_ZN18CMemoryCashManager20ProcessCashDataPrintEv.md
void CMemoryCashManager::ProcessCashDataPrint()
{
    m_cashCnt34--;
    register bool b = (m_cashCnt34 <= 0);
    if (b)
    {
        DNF_LOG_SCOPE_LINE(0x4e, "./log/cashmem",
            "buddy(%d/%d)  member(%d/%d)  black(%d/%d)",
            m_buddyCashHitCnt, m_buddyCashCnt, m_memberCashHitCnt, m_memberCashCnt, m_blackListCashHitCnt,
            m_blackListCashCnt);
        resetCashCnt();
    }
}
