void ComboSkill::convertComboSkillArray(unsigned char& count,
                                        comoboSkillData* data,
                                        ENUM_SKILL_TREE_KIND kind) const
{
    if (data == 0)
    {
        return;
    }
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0x4b, "ComboSkill::convertComboSkillArray kind(%d) error", kind);
    }
    else
    {
        const ComboSkillMap& curMap = getComboSkillMap(kind);
        ComboSkillMap::const_iterator it = curMap.begin();
        unsigned int index = 0;
        for (; it != curMap.end(); ++it)
        {
            data[index].m_comboSkillIndex = it->first;
            for (unsigned int j = 0; j < it->second.size() && j < 6; ++j)
            {
                data[index].m_comboSkillList[j] = (short)it->second[j];
            }
            ++index;
        }
        count = (unsigned char)index;
    }
