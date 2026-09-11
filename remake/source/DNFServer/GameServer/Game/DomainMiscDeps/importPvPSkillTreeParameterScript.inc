// ORIG 0x08a5cf16（0xc43 字节）逐分支转写：
//   [level]/[job index]/[grow type index]/[awakening type] 维护当前键与等级；
//   [skill] 段：明细对入新 PvPSkillTreeValue.m_vec00 且同步写 m_map30[key]
//   的加/减技能表（缺项建 FAIR 默认值）；段尾把 value 插 m_map0[key][level]
//   （重复 grade/键 → 失败）。[static basic skill] 段要求键已存在，明细对追加
//   到既有 value.m_vec0c 并同样更新 m_map30。
// b28d-solo-DomainMiscDeps: DIFF 残余（p167 CLEAR 116/116；N104 EH 尾块序）
// [L3Free25to36 已落 · 复核 2026-09-11] clr_l 两处 ORIG 清零已补：
//   ①0x08a5d0e8 `movl $0x0,-0x10(%ebp)` → `int unusedSlot = 0;`（[skill] 分支入口，
//     value C1Ev(0x08a5d0e3) 之后、pair C1Ev(0x08a5d0f8) 之前；P232/P171 死局部照抄）。
//   ②0x08a5d7d2 `movl $0x0,-0xc(%ebp)` → `std::vector<...>* vec = 0;`（指针槽入口清零，
//     0x08a5d856/0x08a5d920 两次覆写、0x08a5d9b4 读）。
//   实落读数：777 vs 777（gap 0，改前 777 vs 774），difflib ratio 0.236→0.518。
// [L3Free25to36 残余封顶] 帧整体平移 0x10（我方全部槽浅 0x10：token -0x1f0 vs ORIG
//   -0x200），且浅区槽集不同（ORIG 独有 -0x18/-0x20/-0x2c/-0x30/-0x6c/-0x70/-0x8c/
//   -0xa8/-0xb0/-0xe8/-0xec/-0xf0/-0x10c，我方独有 -0x44/-0x84/-0xa0/-0xbc/-0xc0/
//   -0x104）⇒ 属局部变量集/物化点差异（含 map/pair 临时），非单旋钮可复原，记 N104。
//   `int curLevel = 0;` 初始化为我方独有 1 条（ORIG 无；去掉则 776=gap −1），保留以
//   维持 gap 0 且避免未初始化读。
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
                // ORIG 0x08a5d7d2 `movl $0x0,-0xc(%ebp)`：指针槽入口清零（该槽即
                // 下面 vec 的指针缓存，0x08a5d856/0x08a5d920 两次覆写、0x08a5d9b4 读）。
                std::vector<std::pair<int, int> >* vec = 0;
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
            // ORIG 0x08a5d0e8 `movl $0x0,-0x10(%ebp)`：value C1Ev(0x08a5d0e3) 之后、
            // pair C1Ev(0x08a5d0f8) 之前；全函数该槽**仅此一次写、零读**（P232/P171
            // 死局部照抄，槽序 -0x10 与 ORIG 同深）。
            int unusedSlot = 0;
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
}