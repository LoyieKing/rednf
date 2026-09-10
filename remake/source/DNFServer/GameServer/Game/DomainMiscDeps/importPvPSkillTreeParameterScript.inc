bool PvPSkillTreeParameterScript::importPvPSkillTreeParameterScript(
    const char* path)
{
    if (!loadRDARScriptFile(g_pvpSkillTreeScriptBaseDirectory, path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    PvPSkillTreeKey key;
    int curLevel = 0;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                                 // ORIG 08a5db1a：跳循环外 return 1
        }
        if (token == "[level]")
        {
            curLevel = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[job index]")
        {
            key.m_a = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[grow type index]")
        {
            key.m_b = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[awakening type]")
        {
            key.m_c = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[static basic skill]")
        {
            std::map<PvPSkillTreeKey,
                     std::map<int, PvPSkillTreeValue> >::iterator treeIt =
                m_map0.find(key);
            // ORIG 08a5d76c：operator!= 正极性 + else{return false}（P220/P47）。
            if (treeIt != m_map0.end())
            {
                std::map<int, PvPSkillTreeValue>::iterator gradeIt =
                    treeIt->second.find(curLevel);
                // ORIG 08a5d8??：local_10 = &gradeIt->second.m_vec0c 指针缓存。
                std::vector<std::pair<int, int> >* vec;
                if (gradeIt != treeIt->second.end())
                {
                    vec = &gradeIt->second.m_vec0c;
                }
                else
                {
                    PvPSkillTreeValue fresh;
                    std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                              bool> ins = treeIt->second.insert(
                        std::make_pair(curLevel, fresh));
                    gradeIt = ins.first;
                    vec = &gradeIt->second.m_vec0c;
                }
                while (true)
                {
                    // ORIG 08a5d95f：循环内 pair<int,int> 默认构造 + 成员赋值 +
                    // push_back(const&)（P135 左值形）。
                    std::pair<int, int> entry;
                    entry.first = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    entry.second = ScanInt(&ok);
                    if (!ok)
                    {
                        break;                     // ORIG 08a5da22 双失败出口
                    }
                    vec->push_back(entry);
                    std::map<PvPSkillTreeKey,
                             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator
                        perIt = m_map30.find(key);
                    if (perIt != m_map30.end())
                    {
                        // ORIG 三次独立 perIt->second.m_addSkills 访问。
                        std::map<int, int>::iterator slot =
                            perIt->second.m_addSkills.find(entry.first);
                        if (slot != perIt->second.m_addSkills.end())
                        {
                            slot->second = entry.second;
                        }
                        else
                        {
                            perIt->second.m_addSkills.insert(
                                std::make_pair(entry.first, entry.second));
                        }
                    }
                    else
                    {
                        return false;              // ORIG 08a5d9c8 路径
                    }
                }
            }
            else
            {
                return false;
            }
        }
        else if (token == "[skill]")
        {
            PvPSkillTreeValue value;
            while (true)
            {
                std::pair<int, int> entry;
                entry.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                entry.second = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                value.m_vec00.push_back(entry);
                std::map<PvPSkillTreeKey,
                         FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator
                    perIt = m_map30.find(key);
                if (perIt != m_map30.end())
                {
                    std::map<int, int>::iterator slot =
                        perIt->second.m_addSkills.find(entry.first);
                    if (slot != perIt->second.m_addSkills.end())
                    {
                        slot->second = entry.second;
                    }
                    else
                    {
                        perIt->second.m_addSkills.insert(
                            std::make_pair(entry.first, entry.second));
                    }
                }
                else
                {
                    FAIR_PVP_SKILL_TREE_SKILL_PER_POINT fresh;
                    fresh.m_addSkills.insert(
                        std::make_pair(entry.first, entry.second));
                    m_map30.insert(
                        std::make_pair(key, fresh));
                }
            }
            std::map<PvPSkillTreeKey,
                     std::map<int, PvPSkillTreeValue> >::iterator treeIt =
                m_map0.find(key);
            if (treeIt != m_map0.end())
            {
                std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                          bool> ins = treeIt->second.insert(
                    std::make_pair(curLevel, value));
                if (!ins.second)
                {
                    return false;                  // 重复 grade
                }
            }
            else
            {
                std::map<int, PvPSkillTreeValue> inner;
                std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                          bool> ins = inner.insert(
                    std::make_pair(curLevel, value));
                if (!ins.second)
                {
                    return false;                  // ORIG local_f0!=1 路径
                }
                m_map0.insert(
                    std::make_pair(key, inner));
            }
        }
    }
    return true;
