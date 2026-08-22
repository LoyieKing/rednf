// ============================================================================
// CClearCondition（G2 副本清除条件批次）
// ----------------------------------------------------------------------------
// ORIG：
//   C1    _ZN15CClearConditionC1Ev                    0x82a4342（vector 构造 + Clear）
//   Clear _ZN15CClearCondition5ClearEv                0x82a438a
//   Empty _ZNK15CClearCondition5EmptyEv               0x822cba8 → return m_vec.empty()
//   IsC   _ZNK15CClearCondition16IsClearConditionEv   0x822cb92 → return m_flag <= m_count
//   ClearCondition _ZN15CClearCondition14ClearConditionE... 0x82fefce
//     → type∈[0,4] 校验；遍历 (+0x14 的 vector<DungeonClearCondition>) 与
//       m_vec 同步递增；结尾 return (m_flag <= m_count)。
// 布局：当前 CClearCondition.h 以 m_unk14(int) 建模 +0x14（CBattle_Field 0x18
// 布局锁定）。ORIG 此处实为 vector<DungeonClearCondition>，因 ABI 锁定无法在
// 本批扩展；ClearCondition 在该布局下无可匹配条件，保持 ORIG 结尾判定
// (m_flag <= m_count)。Empty/IsClearCondition/Clear 与布局一致，按 ORIG 实现。
// ============================================================================

#include "CClearCondition.h"

CClearCondition::CClearCondition()
{
    m_flag = 0;
    m_count = 0;
    m_vec.clear();
    m_unk14 = 0;
}

CClearCondition::~CClearCondition()
{
}

void CClearCondition::Clear()
{
    m_flag = 0;
    m_count = 0;
    m_vec.clear();
    m_unk14 = 0;
}

bool CClearCondition::Empty() const
{
    return m_vec.empty();
}

bool CClearCondition::IsClearCondition() const
{
    return m_flag <= m_count;
}

bool CClearCondition::ClearCondition(ENUM_DUNGEON_CLEAR_CONDITION_TYPE type, int idx)
{
    if ((int)type < 0 || (int)type > 4)
    {
        return false;
    }
    // ORIG 同步遍历 +0x14 的 DungeonClearCondition 表与 m_vec（逐条件计数），
    // 最终判定 (m_flag <= m_count)。当前重建布局未建模条件表，无可匹配项。
    return m_flag <= m_count;
}

void CClearCondition::SetDungeonClearConditionVector(
    std::vector<DungeonClearCondition>& vec)
{
    (void)vec;  // 条件表所在 +0x14 因 CBattle_Field ABI 锁定为 int，无法承载
}