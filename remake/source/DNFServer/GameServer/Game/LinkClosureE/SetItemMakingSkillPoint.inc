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
