bool importLevelUpRewardScript(LevelUpRewardScript* script, char const* path)
{
    // b28d-solo-EventScriptsCompleteB2 复扫（2026-09-09）：原判 DIFF/N82+N93。
    // 本轮实测：11 处 `if(!ScanType) { next=false; goto levelup_section_end; }`
    // 可对齐 xor（我方 36→47=ORIG 47、ORIG-only xor 归零），但每分支多 1 条
    // next 槽写 + goto 岛 ⇒ gap -5→-15（register next 亦 -15），按 §6.2 gap
    // 优先回退正向形。槽序镜像（ORIG token -0x140 先声明深、帧 0x170；4.4
    // token -0x68 先声明浅、帧 0x1a0）、11 分支 cond/item/entry 块内对象 4.4
    // 独立分槽而 4.1 交错复用、next 在 ORIG 为 %ebx 寄存器（4.4 -O0 恒槽化）
    // ⇒ N82/N93 封顶；语义逐分支与 ORIG 汇编一致。
    // 2026-09-10 复验（判据① jcc/jmp 目标下标逐位比对）：残留 34 处分支目标
    // 偏移**全部同向（+1 平移）且方向一致**，无回边/退出错位；报告 ## 汇编
    // +0x151 `mov $0x0,%ebx` + 节尾 +0x11ec `test %ebx,%ebx` 证实 ORIG 的
    // next 走寄存器，4.4 -O0 恒把具名 bool 槽化（P0/N96 边界）⇒ 不可由合法
    // C++ 复现。
    if (!loadRDARScriptFile("Event/", path))
        return false;

    std::string token;
    bool result = false;
    script->m_dbTableName = "";
    script->m_flag08 = 0;

    // for(;;)+头侧 break 形态：令首 ScanType 调用停在循环体前（ORIG 主循环
    // test 位于 +0x67 循环头；while(cond) 形式会被 GCC 4.4 旋转至环尾）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[recv check db table]")
        {
            ScanStr(&script->m_dbTableName);
            script->m_flag08 = 1;
        }
        else if (token == "[level up]")
        {
            bool next = true;
            while (next)
            {
                if (!ScanType(token, true))
                    break;                    // ORIG 节顶 EOF：回主循环头

                Condition_LevelUp cond;
                Condition_LevelUp::Reward_Item item;

                if (token == "[swordman]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 0;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;         // ORIG ebx=0：退出本节回主循环
                }
                else if (token == "[fighter]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 1;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[gunner]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 2;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[mage]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 3;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[priest]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 4;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[at gunner]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 5;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[thief]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 6;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[at fighter]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 7;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[at mage]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 8;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[demonic swordman]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 9;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[creator mage]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 0xa;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[/level up]")
                {
                    next = false;
                }
                // 其余未知 token：next 保持真 → 销毁局部并继续本节（ORIG 同）
            }
        }
        else if (token == "[send mail]")
        {
            importActionMailScript(&script->m_mail);
        }
        else if (token == "[level up reward end]")
        {
            result = true;
            break;
        }
    }
    return result;
