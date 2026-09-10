void ComboSkill::resetTree(ENUM_SKILL_TREE_KIND kind)
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp", __PRETTY_FUNCTION__,
            0xd, "ComboSkill::resetTree kind(%d) error", kind);
    }
    else
    {
        ComboSkillMap& curMap = getComboSkillMap(kind);
        for (ComboSkillMap::iterator it = curMap.begin(); it != curMap.end();
             ++it)
        {
            for (unsigned int i = 0; i < it->second.size() && i < 6; ++i)
            {
                it->second[i] = 0;
            }
        }
    }
