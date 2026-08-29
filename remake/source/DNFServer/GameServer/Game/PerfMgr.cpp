// PerfMgr.cpp — 与 df_game_r ORIG 对照实现。
// Evidence: docs/class_func_reports/PerfMgr/{inc,get,clear}.md
//   inc  0x085c3c44: 读 (this+idx*4) → lea +1 → 写回
//   get  0x085c3c5c: 返回 (this+idx*4)
//   clear 0x085c3c6a: for(i=0; i<=3; ++i) (this+i*4) = 0（cmpl $0x3; setle）
#include "PerfMgr.h"

void PerfMgr::inc(TASK_IDX idx)
{
    ++m_count[idx];
}

int PerfMgr::get(TASK_IDX idx)
{
    return m_count[idx];
}

void PerfMgr::clear()
{
    for (int i = TASK_IDX_BEGIN; i < TASK_IDX_END; ++i)
    {
        m_count[i] = 0;
    }
}
