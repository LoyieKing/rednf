bool CDataManager::initFairPVPSkillPoint()
{
    bool ok = true;

    if (!m_pvpSkillTree.m_map0.empty())
    {
        for (std::map<PvPSkillTreeKey,
                      std::map<int, PvPSkillTreeValue> >::iterator it =
                 m_pvpSkillTree.m_map0.begin();
             it != m_pvpSkillTree.m_map0.end(); ++it)
        {
            for (std::map<int, PvPSkillTreeValue>::iterator lv =
                     it->second.begin();
                 lv != it->second.end(); ++lv)
            {
                if (m_pvpSkillTree.m_field48 < lv->first)
                {
                    m_pvpSkillTree.m_field48 = lv->first;
                }
            }
        }
    }

    if (!m_pvpSkillTree.m_map18.empty())
    {
        for (std::map<int, std::map<int, int> >::iterator it =
                 m_pvpSkillTree.m_map18.begin();
             it != m_pvpSkillTree.m_map18.end(); ++it)
        {
            if (m_pvpSkillTree.m_field4c < it->first)
            {
                m_pvpSkillTree.m_field4c = it->first;
            }
        }
    }

    if (!m_pvpSkillTree.m_map30.empty())
    {
        for (std::map<PvPSkillTreeKey,
                      FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator perIt =
                 m_pvpSkillTree.m_map30.begin();
             perIt != m_pvpSkillTree.m_map30.end(); ++perIt)
        {
            std::map<int, int> totalSp;
            int sum = 0;
            for (std::map<int, int>::iterator add =
                     perIt->second.m_addSkills.begin();
                 add != perIt->second.m_addSkills.end(); ++add)
            {
                CSkill* skill =
                    G_CDataManager()->find_skill(
                        perIt->first.m_a, add->first);
                if (skill != 0)
                {
                    int sp = skill->get_need_sp_for_level(0, add->second);
                    sum += sp;
                    totalSp.insert(std::make_pair(add->first, sp));
                }
            }

            int running = 0;
            for (std::map<int, int>::iterator rem =
                     perIt->second.m_removeSkills.begin();
                 rem != perIt->second.m_removeSkills.end(); ++rem)
            {
                CSkill* skill =
                    G_CDataManager()->find_skill(
                        perIt->first.m_a, rem->first);
                if (skill != 0) {
                int sp = skill->get_need_sp_for_level(0, rem->second);
                sum -= sp;

                std::map<int, int>::iterator found = totalSp.find(rem->first);
                if (found != totalSp.end())
                {
                    found->second -= sp;
                    if (found->second < 0)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x2379, 0)
                            ("skill point (-) error");
                        ok = false;
                    }
                }

                perIt->second.m_giveSkills.push_back(
                    std::pair<int, int>(rem->first, rem->second));

                int current = 0;
                for (std::map<int, int>::iterator it = totalSp.begin();
                     it != totalSp.end(); ++it)
                {
                    current += it->second;
                }
                if (current + running != sum)
                {
                    running += sum - current;
                    cMyTrace(__PRETTY_FUNCTION__, 0x2390, 0)
                        ("skill point diff job(%d), skill(%d)",
                         (int)perIt->first.m_a, rem->first);
                    ok = false;
                }
                }
            }

            int grand = 0;
            for (std::map<int, int>::iterator it = totalSp.begin();
                 it != totalSp.end(); ++it)
            {
                grand += it->second;
            }
            if (grand != sum)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x239f, 0)
                    ("skill point diff");
                ok = false;
            }

            perIt->second.m_pointCost = sum;
        }
    }

    return ok ? 1 : 0;
