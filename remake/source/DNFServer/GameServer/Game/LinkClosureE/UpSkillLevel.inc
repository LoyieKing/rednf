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
