void ComboSkill::setComboSkill(ENUM_SKILL_TREE_KIND kind, ComboSkillMap& map)
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0xb5, "ComboSkill::setComboSkill kind(%d) error", kind);
    }
    else
    {
        if (map.empty())
        {
            return;
        }
        else
        {
            ComboSkillMap& curMap = getComboSkillMap(kind);
            ComboSkillMap::iterator findIt = curMap.end();
            ComboSkillMap::iterator it = map.begin();
            resetTree(kind);
            while (it != map.end())
            {
                findIt = curMap.find(it->first);
                if (findIt == curMap.end())
                {
                    continue;
                }
                else
                {
                    for (unsigned int i = 0; i < it->second.size() && i < 6; ++i)
                    {
                        findIt->second[i] = it->second[i];
                    }
                }
                ++it;
            }
        }
    }
