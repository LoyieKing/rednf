// ORIG T _ZNK16CItemMakingSkill13GetSkillLevelE21ENUM_ITEM_MAKING_TYPEP5CUser
// @0x08545c0c。报告 GetSkillLevel.md：idxByExp = 第一个 vec[i].first > 当前点数
// 的下标；idxByLevel = 第一个 user 等级 < vec[i].second 的下标；取 min。
// [b28d-solo-LinkClosureE] AE 76/76（p167 CLEAR 7/7）：删 cur 具名（P0）；
//   两循环改 `for(init; i<size;) { if (cur>=first){++i} else break; }` 复现 ORIG 的
//   setge/setl + je 物化与「cond1 底部、cond2 在 body 顶、++ 在 cond2 后」布局；
//   尾部三目 `(idxByExp<=idxByLevel)?…` 对齐。idxByLevel 方向修正为 `level>=second`
//   （ORIG setge；旧写法 setl 方向相反）。
int CItemMakingSkill::GetSkillLevel(ENUM_ITEM_MAKING_TYPE type, CUser* user) const
{
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    int idxByExp = 0;
    int idxByLevel = 0;
    for (idxByExp = 0;
         static_cast<int>(idxByExp) < static_cast<int>(table->m_vec18.size());)
    {
        if (static_cast<int>(static_cast<short>(m_skillLevels[type]))
            >= static_cast<int>(table->m_vec18[idxByExp].first))
        {
            ++idxByExp;
        }
        else
        {
            break;
        }
    }
    for (idxByLevel = 0;
         static_cast<int>(idxByLevel) < static_cast<int>(table->m_vec18.size());)
    {
        if (user->get_charac_level()
            >= static_cast<int>(table->m_vec18[idxByLevel].second))
        {
            ++idxByLevel;
        }
        else
        {
            break;
        }
    }
    return (idxByExp <= idxByLevel) ? idxByExp : idxByLevel;
}