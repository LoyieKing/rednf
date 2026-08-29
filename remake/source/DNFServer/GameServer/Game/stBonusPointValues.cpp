// ============================================================================
// stBonusPointValues（排行系统清时奖励点区间元素）
// ----------------------------------------------------------------------------
// ORIG：
//   ctor  _ZN18stBonusPointValuesC1Ev / C2Ev  0x08370d8c（调用 clear）
//   clear _ZN18stBonusPointValues5clearEv     0x08370da0（+0/+4 两 int 置 0）
// 布局：{ int m_a; int m_b; }（8 字节），声明见 stBonusPointValues.h。
// ============================================================================

#include "stBonusPointValues.h"

stBonusPointValues::stBonusPointValues()
{
    clear();
}

void stBonusPointValues::clear()
{
    m_a = 0;
    m_b = 0;
}
