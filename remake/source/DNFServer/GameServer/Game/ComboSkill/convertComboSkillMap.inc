void ComboSkill::convertComboSkillMap(unsigned char count,
                                      const comoboSkillData* data,
                                      ENUM_SKILL_TREE_KIND kind)
{
    if (data == 0)
    {
        return;
    }
    if (count > 6)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0x25, "ComboSkill::setComboSkill count value(%d) error",
            (int)count);
    }
    else
    {
        if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
        {
            LogManager::logFormat(
                1, "ComboSkill.cpp",
                __PRETTY_FUNCTION__,
                0x2b, "ComboSkill::setComboSkill kind(%d) error", kind);
        }
        else
        {
            ComboSkillMap& curMap = getComboSkillMap(kind);
            ComboSkillMap::iterator findIt = curMap.end();
            std::vector<int> vecSkill;
            for (int i = 0; i < (int)count && i < 6; ++i)
            {
                findIt = curMap.find(data[i].m_comboSkillIndex);
                if (findIt == curMap.end())
                {
                    continue;
                }
                else
                {
                    vecSkill.clear();
                    for (int j = 0; j < 6; ++j)
                    {
                        vecSkill.push_back((int)data[i].m_comboSkillList[j]);
                    }
                    curMap[data[i].m_comboSkillIndex] = vecSkill;
                }
            }
        }
    }
}