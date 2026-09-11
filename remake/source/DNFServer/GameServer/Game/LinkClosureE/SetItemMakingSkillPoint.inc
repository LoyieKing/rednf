// ============================================================================
// LinkFinal2 核销 —— CItemMakingSkill::SetItemMakingSkillPoint(int,int,CUser*)
// ORIG T @0x08545d24 (0x24b)，objdump 逐指令转写（GM 设置技能点）：
//   skillIdx>0xa 或等级表空 → 直接返回；
//   point∈(0,0x3e8] 且 point<表尾 first（=表最大经验门槛）才放行（否则返回）；
//   写 m_skillLevels[skillIdx]=(short)point；
//   等级钳制：charac_level==0x46（70）→ 钳到表尾 first；否则从头扫表，
//     user 等级 < vec[i].second 且当前值 > vec[i].first 时钳到 vec[i].first 并停；
//   clamp：值 >0x3e8 → 0x3e8；alter() 通知；Send_ItemMakingSkillList(user)。
// ============================================================================
// [b28d-solo-LinkClosureE] AE 178/178（p167 CLEAR 19/19）。真 bug 修正：放行判定
//   极性反置——旧 `gate=(first<point); if(!gate)return` 在 point<=0 / point>0x3e8 时
//   继续写入技能点（ORIG 该两路 return），且 first>=point 被误拒。现按 ORIG 写
//   `if (!(point>0 && point<=0x3e8 && rbegin()->first>=point)) return;`。
//   配套：删 vec 具名引用与 cur 具名（P0/P183）、clamp 改有符号 (short) 比较。
void CItemMakingSkill::SetItemMakingSkillPoint(int skillIdx, int point,
                                               CUser* user)
{
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    if (skillIdx > 0xa)                                    // ORIG 0x8545d3f
        return;
    if (table->m_vec18.rbegin() == table->m_vec18.rend())  // ORIG 空表 → 返回
        return;

    // ORIG 0x8545d8f..0x8545dd6：point<=0 / point>0x3e8 / first>=point 一律返回
    if (!(point > 0 && point <= 0x3e8
          && table->m_vec18.rbegin()->first >= point))
        return;

    m_skillLevels[skillIdx] = static_cast<unsigned short>(point);  // ORIG mov %cx,0x6(,idx,2)

    if (user->get_charac_level() == 0x46)                  // ORIG 0x8545df7
    {
        if (static_cast<short>(m_skillLevels[skillIdx])
            > table->m_vec18.rbegin()->first)              // ORIG 0x8545e34 setg
            m_skillLevels[skillIdx] =
                static_cast<unsigned short>(table->m_vec18.rbegin()->first);
    }
    else
    {
        for (unsigned int i = 0; i < table->m_vec18.size(); ++i)  // ORIG 0x8545e84 循环
        {
            if (user->get_charac_level() < table->m_vec18[i].second)
            {
                if (static_cast<short>(m_skillLevels[skillIdx])
                    > table->m_vec18[i].first)             // ORIG 0x8545ed9 setg
                {
                    m_skillLevels[skillIdx] =
                        static_cast<unsigned short>(table->m_vec18[i].first);
                    break;                                 // ORIG 0x8545f04
                }
            }
        }
    }

    if (static_cast<short>(m_skillLevels[skillIdx]) > 0x3e8)   // ORIG 0x8545f31 clamp
        m_skillLevels[skillIdx] = 0x3e8;

    alter();                                               // ORIG 0x8545f4a
    Send_ItemMakingSkillList(user);                        // ORIG 0x8545f5c
}