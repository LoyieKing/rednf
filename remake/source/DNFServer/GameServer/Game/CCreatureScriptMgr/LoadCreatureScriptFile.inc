//   3) sync IsEnable 时按 native.m_attackInfo[0..1] 拼 Attack 路径并
//      ImportAttackInfoScript，序列化 "%d,%d,%f,%d"（元素集追加 ",%d"，
//      空集追加 ",4"；失败则 "%d,%d"）入 buf270/buf370；
//   4) 构造 CCreatureScript，Set(native)，SetItemId(注册表 itemId)，
//      insert_creature_info_to_db，查重后注册进 m_scripts。
// [b28d-30] 复核 DIFF：大件残差（N82/N89 族），ORIG 见 class_func_reports/user_creature__CCreatureScriptMgr/LoadCreatureScriptFile.md。
bool CCreatureScriptMgr::LoadCreatureScriptFile()
{
    // [N5Free211to230.N5D] ORIG 0x0833cbb2 首条 `movl $0x0,-0x60(%ebp)`：script 指针
    // 具名预清（P198① 两句式）⇒ 声明提到函数头（native 之前）。
    CCreatureScript* script = 0;
    CreatureScript native;
    // [N5D] ORIG 0x0833cbc7（native 构造之后、buf rep stos 之前）`movl $0x0,-0x20(%ebp)`
    // = path 具名预清 ⇒ 同样两句式：头部声明 + 循环内赋值。
    const char* path = 0;
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
        path = it->second.c_str();
        if (path == 0)
        {
            // [N5D] ORIG 0x0833d352 = 尾 nop 岛（其后 0x0833d353 mov $1,%ebx）：
            // 4.4 的 `goto 尾标签` 形精确复现（探针 /tmp/r5/n5/p_ret.cpp q2）。
            goto L_true;
        }

        if (!importCreatureScript(&native, path))
        {
            LogManager::logFormat(1, "Creature.cpp",
                __PRETTY_FUNCTION__,
                0xf6b, "importCreatureScript fail file - %s", path);
            return false;
        }

        if (sync_script::GetInstanceSyncScript()->IsEnable())
        {
            // P197④：ORIG 用 `size()`（无 empty 调用）；P175：ORIG 走 `at()` 索引
            // （`vector<string>::at` ×4）；`[0][0]` 的 string 下标在 ORIG 是
            // `c_str()[0]`（p167：MISSING c_str ×2 / EXTRA string::operator[] ×2）。
            // [N5D] P225 推广（P226 条件向）：ORIG 0x0833cd09/0x0833cd3c 每个 && 操作数
            // 各自 `setne %al; test %al,%al; je` 物化，4.4 对 && 链恒折直跳 ⇒ 拆成两条
            // 相邻 if（无内层花括号，缩进零变化）复现（探针 p_creature.cpp f2 vs f1）。
            if (native.m_attackInfo.size() != 0)
            if (native.m_attackInfo.at(0).c_str()[0] != 0)
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
            // [N5D] 同上：ORIG 0x0833cf32 `cmp $0x1,%eax; seta %al; test; je`。
            if (native.m_attackInfo.size() > 1)
            if (native.m_attackInfo.at(1).c_str()[0] != 0)
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

        script = new CCreatureScript();
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

        // [N5D] P293：内联表达式 `find(k) != end()` 在 4.4 恒「先 end 后 find」，
        // ORIG 0x0833d1f6 find → 0x0833d22a end ⇒ 改具名 iterator（探针
        // /tmp/r5/n5/p_creature2.cpp m1 vs m2）。
        std::map<int, CCreatureScript*>::iterator found = m_scripts.find(native.m_id);
        if (found != m_scripts.end())
        {
            // P221：具名 trace 在 4.4 恒占深槽（ORIG 为浅槽）⇒ 构造即调用临时链。
            // [N5D] P139：三处同文串改 __PRETTY_FUNCTION__（ORIG 0x08c2ebc0 size 0x41）。
            cMyTrace(__PRETTY_FUNCTION__,
                     0xfbe, 5)("[%s][%d]",
                               __PRETTY_FUNCTION__,
                               0xfbe);
            return false;
        }
        m_scripts.insert(std::make_pair(native.m_id, script));
    }
L_true:
    return true;
}