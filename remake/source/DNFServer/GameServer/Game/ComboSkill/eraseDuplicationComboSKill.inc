void ComboSkill::eraseDuplicationComboSKill(ENUM_SKILL_TREE_KIND kind, int slot)
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0xd4, "ComboSkill::eraseDuplicationComboSKill kind(%d) error", kind);
    }
    else
    {
        ComboSkillMap& curMap = getComboSkillMap(kind);
        for (ComboSkillMap::iterator it = curMap.begin(); it != curMap.end();
             ++it)
        {
            for (std::vector<int>::iterator vit = it->second.begin();
                 vit != it->second.end(); ++vit)
            {
                if (*vit == slot)
                {
                    it->second.erase(vit);
                    it->second.push_back(0);
                }
            }
        }
    }
}