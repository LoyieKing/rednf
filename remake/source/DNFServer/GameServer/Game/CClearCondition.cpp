// ============================================================================
// CClearCondition（G2 副本清除条件批次）
// ----------------------------------------------------------------------------
// ORIG：
//   C1    _ZN15CClearConditionC1Ev                    0x82a4342（vector 构造 + Clear）
//   Clear _ZN15CClearCondition5ClearEv                0x82a438a
//   Empty _ZN15CClearCondition5EmptyEv               0x822cba8 → return m_vec.empty()
//   IsC   _ZN15CClearCondition16IsClearConditionEv   0x822cb92 → return m_flag <= m_count
//   ClearCondition _ZN15CClearCondition14ClearConditionE... 0x82fefce
//     → type∈[0,4] 校验；遍历 (+0x14 指向的 vector<DungeonClearCondition>) 与
//       m_vec 同步递增；结尾 return (m_flag <= m_count)。
// 布局：当前 CClearCondition.h 以 m_unk14(int) 建模 +0x14（CBattle_Field 0x18
// 布局锁定）。ORIG 此处实为 vector<DungeonClearCondition>* 指针（指向外部表，
// SetDungeonClearConditionVector 写入 vec 地址；Clear 置 0），与 m_unk14 兼容。
// Empty/IsClearCondition/Clear 与布局一致，按 ORIG 实现。
// ============================================================================

#include "CClearCondition.h"

CClearCondition::CClearCondition()
{
    // ORIG 0x82a4342：m_vec(std::vector<int> @+8) 默认构造后 call Clear()（0x82a438a，
    // 出线）清 +0/+4/+0x14 并 vec.clear。此处不得内联 Clear 体——ORIG ctor 是
    // `vector<int>::C1` + `call CClearCondition::Clear`，内联会多出逐字段 mov。
    Clear();
}

CClearCondition::~CClearCondition()
{
}

void CClearCondition::Clear()
{
    m_flag = 0;
    m_count = 0;
    m_vec.clear();
    m_pCondVec = 0;
}

bool CClearCondition::Empty()
{
    return m_vec.empty();
}

bool CClearCondition::IsClearCondition()
{
    return m_flag <= m_count;
}

// [b28d-51] 判词 N（ORIG 0x082fefce，N82 族）：switch 决策树 + 帧槽方向差；
// 语义/调用集齐，byte 层封顶。
bool CClearCondition::ClearCondition(ENUM_DUNGEON_CLEAR_CONDITION_TYPE type, int idx)
{
    if ((int)type < 0 || (int)type > 4)
    {
        return false;
    }
    // ORIG 0x82fefce：+0x14 指向 vector<DungeonClearCondition>（外部条件表），
    // 与 m_vec 同步遍历；条目 m_0/m_4 匹配 type/idx 时，若 m_8 > 当前计数
    // 则计数自增并 m_count++，否则返回 false；结尾 (m_flag <= m_count)。
    std::vector<DungeonClearCondition>::iterator itd = m_pCondVec->begin();
    std::vector<DungeonClearCondition>::iterator endd = m_pCondVec->end();
    std::vector<int>::iterator itv = m_vec.begin();
    std::vector<int>::iterator endv = m_vec.end();
    while (itd != endd && itv != endv)
    {
        DungeonClearCondition* d = &*itd;
        int* v = &*itv;
        if (d->m_0 == (int)type && d->m_4 == idx)
        {
            if (d->m_8 > *v)
            {
                ++(*v);
                ++m_count;
            }
            else
            {
                return false;
            }
        }
        ++itd;
        ++itv;
    }
    if (m_flag > m_count)
    {
        return false;
    }
    return true;
}

// [b28d-51] 判词 N（ORIG 0x082fef12，N82/N86 族）：vector 迭代器/临时槽
// 阶梯方向差（ORIG 帧 0x28 vs 4.4 0x24+ebx）；语义/调用集齐，byte 层封顶。
void CClearCondition::SetDungeonClearConditionVector(
    std::vector<DungeonClearCondition>& vec)
{
    // ORIG 0x82fef12：Clear 后若 vec 非空，+0x14 存 vec 地址，并对每个
    // 条目：m_flag(+0x00) += 条目 m_8 字段，同时向 m_vec push 一个 0。
    Clear();
    if (!vec.empty())
    {
        m_pCondVec = &vec;
        for (std::vector<DungeonClearCondition>::iterator it = vec.begin();
             it != vec.end(); ++it)
        {
            m_flag += it->m_8;
            m_vec.push_back(0);
        }
    }
}
