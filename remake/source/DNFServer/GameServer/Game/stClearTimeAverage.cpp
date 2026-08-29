// ============================================================================
// stClearTimeAverage / stClearTimeAverageMapKey（排行系统清时平均）
// ----------------------------------------------------------------------------
// ORIG（df_game_r）：
//   stClearTimeAverage
//     ctor  _ZN18stClearTimeAverageC1Ev / C2Ev  0x0891b6a8（sub $0x18 + call clear）
//     clear _ZN18stClearTimeAverage5clearEv     0x0891b6bc（+0/+4/+8/+0xc/+0x10 五 int 置 0）
//   stClearTimeAverageMapKey
//     ctor  _ZN24stClearTimeAverageMapKeyC1Ev / C2Ev  0x08370d1c（call clear）
//     clear _ZN24stClearTimeAverageMapKey5clearEv     0x08370d30（+0/+4 两 int 置 0）
//     operator< _ZNK24stClearTimeAverageMapKeyltERKS_ 0x08370d48（W 头内联）
// 布局：stClearTimeAverage { int m_v[5]; }（0x14）；
//       stClearTimeAverageMapKey { int m_a; int m_b; }（8）。
// ============================================================================

#include "stClearTimeAverage.h"

stClearTimeAverage::stClearTimeAverage()
{
    clear();
}

void stClearTimeAverage::clear()
{
    m_v[0] = 0;
    m_v[1] = 0;
    m_v[2] = 0;
    m_v[3] = 0;
    m_v[4] = 0;
}

stClearTimeAverageMapKey::stClearTimeAverageMapKey()
{
    clear();
}

void stClearTimeAverageMapKey::clear()
{
    m_a = 0;
    m_b = 0;
}

bool stClearTimeAverageMapKey::operator<(const stClearTimeAverageMapKey& o) const
{
    if (m_a < o.m_a)
        return true;
    if (m_a == o.m_a && m_b < o.m_b)
        return true;
    return false;
}
