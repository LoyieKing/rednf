// ORIG T _ZN16CItemMakingSkill12UpSkillLevelE21ENUM_ITEM_MAKING_TYPEiP5CUser
// @0x0854571c。报告 UpSkillLevel.md：type>0xa 或表空守卫 → 直接返回；
// 仅当 point∈(0,m_bonusExp] 且 point<=vec.back().first 时加值；随后按等级
// （0x46 级钳 vec.back().first / 否则逐档钳制）+ 1000 上限 clamp；alter() 通知。
// [b28d-solo-LinkClosureE] DIFF 235/233（p167 CLEAR 24/24）：唯一残差 = 技能点加法
//   的指令选择——ORIG `movzwl→%eax; mov %eax,%ecx; mov point,%eax;
//   lea (%ecx,%eax,1),%eax; mov %eax,%ecx` vs 4.4 `movzwl→%ecx; add %eax,%ecx`
//   （差 2 条中转）⇒ N86「同一算术多 codegen」封顶（`+=` / 显式 int 提升 / 去 cast
//   三形实测同读数）。其余全等：守卫 `!(point>0 && point<=m_bonusExp && first>=point)`、
//   trace 临时链（P227 令 vararg 在 ctor 前落 %esi/%ebx）、rbegin()->first 族。
// [D08 2026-09-10] 复测加固（探针 /tmp/usk_probe.cpp f0–f7）：`+=`、显式赋值的
//   ushort/int/uint 提升、交换操作数、双层 cast、int 回 cast 八形读数**全同** f0
//   （`movzwl→%ecx; add %eax,%ecx`）；唯一异形 f3（交换+混型）产 `push %ebx` 且 point
//   升格 %ebx，与 ORIG 无 ebx 保存相反 ⇒ 反劣。ORIG 两段中转（0x085457e7 / 0x085457ef
//   各一条 `mov %eax,%ecx`）4.4 不可造 ⇒ N18a+N1 封顶。
void CItemMakingSkill::UpSkillLevel(ENUM_ITEM_MAKING_TYPE type, int point, CUser* user)
{
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    if (10 < type)
        return;
    if (table->m_vec18.rbegin() == table->m_vec18.rend())   // ORIG rbegin==rend 空判定
        return;
    // ORIG 0x085457a3..0x085457d3：point<=0 / point>m_bonusExp / first<point 一律返回
    if (!(point > 0 && point <= table->m_bonusExp
          && table->m_vec18.rbegin()->first >= point))
        return;

    // [N1D 2026-09-11 复测·n1_v 口径] 真残差唯一段 = 技能点加法：
    //   ORIG 0x085457e2..0x085457f4 `movzwl 0x6(%eax,%ecx,2),%eax;
    //   mov %eax,%ecx; mov 0x10(%ebp),%eax; lea (%ecx,%eax,1),%eax;
    //   mov %eax,%ecx; mov %cx,...`（5 条）vs 4.4 `movzwl ...,%ecx;
    //   mov 0x10(%ebp),%eax; add %eax,%ecx; mov %cx,...`（3 条）。
    //   本波试形：`m_skillLevels[type] = (unsigned short)(m_skillLevels[type] + point)`
    //   （显式加法+截断，原为 `+= (unsigned short)point`）实测读数与形态**全同**
    //   （235/234，同段同形）⇒ 已回滚。4.1 的两条 %eax→%ecx 值中转 + lea 加法
    //   在 4.4 不可造（N18a/N1 族，与 D08 探针 f0–f7 八形结论一致）。
    m_skillLevels[type] += static_cast<unsigned short>(point);
    if (user->get_charac_level() == 0x46)
    {
        if (static_cast<short>(m_skillLevels[type])
            > table->m_vec18.rbegin()->first)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xb5, 0)(
                "CHARACTER_LEVEL_CURRENT_MAX :item_making_skill_[%d] = %d",
                static_cast<int>(type),
                static_cast<int>(static_cast<short>(m_skillLevels[type])));
            m_skillLevels[type] =
                static_cast<unsigned short>(table->m_vec18.rbegin()->first);
        }
    }
    else
    {
        for (std::vector<std::pair<int, int> >::size_type i = 0;
             i < table->m_vec18.size(); ++i)
        {
            if (user->get_charac_level() < table->m_vec18[i].second)
            {
                if (static_cast<short>(m_skillLevels[type])
                    > table->m_vec18[i].first)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xc5, 0)(
                        "NO CHARACTER_LEVEL_CURRENT_MAX : item_making_skill_[%d] = %d",
                        static_cast<int>(type),
                        static_cast<int>(static_cast<short>(m_skillLevels[type])));
                    m_skillLevels[type] =
                        static_cast<unsigned short>(table->m_vec18[i].first);
                }
            }
        }
    }
    if (static_cast<short>(m_skillLevels[type]) > 0x3e8)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xce, 0)(
            "ITEM_MAKING_SKILL_LEVEL_LIMIT : item_making_skill_[%d] = %d",
            static_cast<int>(type),
            static_cast<int>(static_cast<short>(m_skillLevels[type])));
        m_skillLevels[type] = 1000;
    }
    alter();
}