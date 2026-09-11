// ---------------------------------------------------------------------------
// upgradeAbilityByMonsterParameterCategory — ORIG 0x88b7f36。
// 先扫描是否存在 id==13（hasCat13）；再逐条目：解析目标字段后——
//   hasCat13&&isChampion&&id==10 跳过；hasCat13&&!isChampion&&id==13 跳过；
//   m_field4!=0 → 目标 = trunc(target_f * getValueFloat())；
//   否则 → 目标 += getValueInt()（即 m_field8/100）。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=AE ORIG 0x088b7f36 orig/ours=163/163。
// 修复：第一循环引用局部（P157 反面：消 sete 物化 + 恢复 -0x18 槽）、switch case
// 书写序 = ORIG 跳转表叶块排放序（P46：0..5,10,11,12,6..9,13）、删 default、
// 三守卫合并为单 || goto 尾标签（消 3 个 continue 中继岛）。case→字段映射与
// Ghidra 跳转表一致（0..13 全核，无字段错位）。
void upgradeAbilityByMonsterParameterCategory(
    const std::vector<STMonsterParameterCategory>* categories,
    ActiveStaticInfo* info, bool isChampion)
{
    bool hasCat13 = false;
    for (std::vector<STMonsterParameterCategory>::size_type i = 0;
         i < categories->size(); ++i)
    {
        // ORIG 有引用局部槽 -0x18（P157 反面：引用局部保留 operator[] 一次）；
        // 直接内联 .m_field0 会使 4.4 物化 sete（含 call 条件）。
        const STMonsterParameterCategory& cat = (*categories)[i];
        if (cat.m_field0 == 13)
        {
            hasCat13 = true;
            break;
        }
    }

    for (std::vector<STMonsterParameterCategory>::size_type j = 0;
         j < categories->size(); ++j)
    {
        const STMonsterParameterCategory& cat = (*categories)[j];
        // P167：ORIG 为内联 switch（无需独立 helper，§4.1），字段映射直接内联。
        // case 书写序 = ORIG 跳转表叶块排放序（P46）：0..5,10,11,12,6..9,13。
        int* target = 0;
        switch (cat.m_field0)
        {
        case 0:  target = &info->m_statBase4;       break; // +0x120
        case 1:  target = &info->m_statBase3;       break; // +0x118
        case 2:  target = &info->m_attack;          break; // +0x18
        case 3:  target = &info->m_magicAttack;     break; // +0x20
        case 4:  target = &info->m_defense;         break; // +0x28
        case 5:  target = &info->m_magicDefense;    break; // +0x30
        case 10: target = &info->m_hpBase;          break; // +0x04
        case 11: target = &info->m_statBase6;       break; // +0x130
        case 12: target = &info->m_attr17Base[13];  break; // +0xac
        case 6:  target = &info->m_statBase11;      break; // +0x158
        case 7:  target = &info->m_statBase12;      break; // +0x160
        case 8:  target = &info->m_statBase13;      break; // +0x168
        case 9:  target = &info->m_statBase14;      break; // +0x170
        case 13: target = &info->m_hpBase;          break; // +0x04
        }
        if (target == 0 ||
            (hasCat13 && isChampion && cat.m_field0 == 10) ||
            (hasCat13 && !isChampion && cat.m_field0 == 13))
            goto NEXT;

        if (cat.m_field4 != 0)
        {
            *target = static_cast<int>(static_cast<float>(*target) *
                                       cat.getValueFloat());
            goto NEXT;
        }
        *target += cat.getValueInt();
    NEXT: ;
    }
}