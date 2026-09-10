bool CCreatureScriptMgr::LoadCreatureScriptFile()
{
    CreatureScript native;
    char buf170[0x100] = {0};   // ORIG rep stos 0x100 清零
    char buf270[0x100] = {0};
    char buf370[0x100] = {0};
    AttackInfo attackInfo;

    if (!sync_script::GetInstanceSyncScript()->truncate_creature_info_table())
    {
        puts("failed to truncate creature info table");
        return false;
    }

    // round1 R19（p167 差集定位）：ORIG 走**非 const** `_Rb_tree_iterator`
    // （`ptEv`/`ne` 均为其 const 成员方法），我方 const_iterator 多发 2 条
    // `const_iterator::C1ERK iterator` 转换构造（P96/P270）⇒ 改非 const。
    for (std::map<int, std::string>::iterator it = g_creatureScriptFileList.m_fileMap.begin();
         it != g_creatureScriptFileList.m_fileMap.end(); it++)
    {
        const char* path = it->second.c_str();
        if (path == 0)
        {
            return true;   // ORIG cmpl $0x0 → return 1
        }

        if (!importCreatureScript(&native, path))
        {
            LogManager::logFormat(1, "Creature.cpp",
                "bool user_creature::CCreatureScriptMgr::LoadCreatureScriptFile()",
                0xf6b, "importCreatureScript fail file - %s", path);
            return false;
        }

        if (sync_script::GetInstanceSyncScript()->IsEnable())
        {
            // P197④：ORIG 用 `size()`（无 empty 调用）；P175：ORIG 走 `at()` 索引
            // （`vector<string>::at` ×4）；`[0][0]` 的 string 下标在 ORIG 是
            // `c_str()[0]`（p167：MISSING c_str ×2 / EXTRA string::operator[] ×2）。
            if (native.m_attackInfo.size() != 0 && native.m_attackInfo.at(0).c_str()[0] != 0)
            {
                sprintf(buf170, "%s%s", native.m_directory.c_str(),
                        native.m_attackInfo.at(0).c_str());
                if (ImportAttackInfoScript(&attackInfo, buf170, 0))
                {
                    sprintf(buf270, "%d,%d,%f,%d",
                            native.m_skillType, native.m_skillMP,
                            (double)attackInfo.m_damage, attackInfo.m_attackType);
                    if (attackInfo.m_elements.size() == 0)
                    {
                        sprintf(buf270 + strlen(buf270), ",%d", 4);
                    }
                    else
                    {
                        for (std::set<ENUM_ELEMENT>::const_iterator e =
                                 attackInfo.m_elements.begin();
                             e != attackInfo.m_elements.end(); ++e)
                        {
                            sprintf(buf270 + strlen(buf270), ",%d", *e);
                        }
                    }
                }
                else
                {
                    sprintf(buf270, "%d,%d", native.m_skillType, native.m_skillMP);
                }
            }
            if (native.m_attackInfo.size() > 1 && native.m_attackInfo.at(1).c_str()[0] != 0)
            {
                sprintf(buf170, "%s%s", native.m_directory.c_str(),
                        native.m_attackInfo.at(1).c_str());
                if (ImportAttackInfoScript(&attackInfo, buf170, 0))
                {
                    // ORIG 0x0833d018：第二块（attackInfo[1]）用 over 组字段
                    // (-0x64c=+0x6c m_overSkillType, -0x654=+0x64 m_overSkillMP)
                    sprintf(buf370, "%d,%d,%f,%d",
                            native.m_overSkillType, native.m_overSkillMP,
                            (double)attackInfo.m_damage, attackInfo.m_attackType);
                    if (attackInfo.m_elements.size() == 0)
                    {
                        sprintf(buf370 + strlen(buf370), ",%d", 4);
                    }
                    else
                    {
                        for (std::set<ENUM_ELEMENT>::const_iterator e =
                                 attackInfo.m_elements.begin();
                             e != attackInfo.m_elements.end(); ++e)
                        {
                            sprintf(buf370 + strlen(buf370), ",%d", *e);
                        }
                    }
                }
                else
                {
                    // ORIG 0x0833d11b：同为 over 组字段
                    sprintf(buf370, "%d,%d", native.m_overSkillType,
                            native.m_overSkillMP);
                }
            }
        }

        CCreatureScript* script = new CCreatureScript();
        script->Set(&native);
        int itemId = m_creatureIdToItemId[native.m_id];
        script->SetItemId(itemId);

        if (!sync_script::GetInstanceSyncScript()->insert_creature_info_to_db(
                itemId, &native, buf270, buf370))
        {
            printf("failed to insert id number < %d > creature info to db\n",
                   native.m_id);
            return false;
        }

        if (m_scripts.find(native.m_id) != m_scripts.end())
        {
            // P221：具名 trace 在 4.4 恒占深槽（ORIG 为浅槽）⇒ 构造即调用临时链。
            cMyTrace("bool user_creature::CCreatureScriptMgr::LoadCreatureScriptFile()",
                     0xfbe, 5)("[%s][%d]",
                               "bool user_creature::CCreatureScriptMgr::LoadCreatureScriptFile()",
                               0xfbe);
            return false;
        }
        m_scripts.insert(std::make_pair(native.m_id, script));
    }
    return true;
