bool importTownScript(TownScript* script, const char* path)
{
    // ORIG T 0x8aada90（382 条）：ScanType 失败=文件读完并成功收尾；各标签
    // 的 ScanStr/ScanInt 按报告保留其原始检查边界。
    if (!loadRDARScriptFile(g_townScriptBaseDirectory, path))
    {
        return false;
    }
    script->clear();

    // ORIG：getDirectoryPath(script->m_str38, "%s%s", g_townScriptBaseDirectory, path)
    getDirectoryPath(script->m_str38, "%s%s", g_townScriptBaseDirectory, path);

    std::string line;
    std::string sub;
    int value = 0;
    bool flag = false;
    // ORIG 8aadb2b：TownAreaScript 在循环**外**构造（-0x54），[area] 段内只
    // `call clear`（8aadc82）后复用同一对象。
    TownAreaScript area;
    for (;;)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[name]")
        {
            ScanStr(&script->m_str4);
            continue;
        }
        if (line == "[entering title]")
        {
            ScanStr(&script->m_str8);
            continue;
        }
        if (line == "[entering title next]")
        {
            ScanStr(&script->m_str10);
            continue;
        }
        if (line == "[cutscene image]")
        {
            ScanStr(&script->m_strc);
            script->m_14 = ScanInt((bool*)0);   // ORIG：ScanInt 返回直接存（无检查）
            continue;
        }
        if (line == "[dungeon what must be cleared]")
        {
            script->m_18 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[only server parsing dungeon what must be cleared]")
        {
            script->m_1c = ScanInt((bool*)0);
            continue;
        }
        if (line == "[area]")
        {
            area.clear();
            // ORIG 8aadc87 `movl $0x0,(%esp)` ⇒ 传 NULL，无检查；flag 全程用外层
            // -0x19 槽（块内再声明会另占槽，P212）。
            area.m_areaIndex = ScanInt((bool*)0);
            ScanStr(&area.m_mapName);               // ORIG 8aadc9f 无检查
            ScanStr(&sub);                          // ORIG 8aadca4 ScanStr（非 ScanType）
            if (sub == "[minimap rect]")
            {
                // ORIG 8aadcca..8aadd75：T_Rect 复用 + 多个 rect；内层每次
                // ScanInt 得到下一项，四项后的值直接作为下一矩形首项复用。
                T_Rect r;
                value = ScanInt(&flag);                 // 8aadcd5
                while (flag)
                {
                    int i = 0;
                    do
                    {
                        if (i == 0)
                            r.m_00 = value;
                        else if (i == 1)
                            r.m_08 = value;
                        else if (i == 2)
                            r.m_04 = value;
                        else
                            r.m_0c = value;
                        value = ScanInt(&flag);         // 8aadd1e
                        ++i;
                    } while (i <= 3 && flag);           // 8aadd30
                    area.m_rects.push_back(r);           // 8aadd4e
                }
                ScanStr(&sub);                      // ORIG 8aadd6f ScanStr
            }
            if (sub == "[use private store]")
            {
                value = ScanInt(&flag);             // ORIG 8aadd97
                area.m_field24 = (value != 0);      // ORIG 8aadda2 setne（bool 直存）
                ScanStr(&sub);                      // ORIG 8aaddb0 ScanStr
            }
            // 区域类型：normal/gate/fishing place/pub/dungeon gate/
            //          safe zone against a / safe zone against b
            if (sub == "[normal]")
                area.m_areaType = 0;
            else if (sub == "[gate]")
                area.m_areaType = 1;
            else if (sub == "[fishing place]")
                area.m_areaType = 2;
            else if (sub == "[pub]")
                area.m_areaType = 3;
            else if (sub == "[dungeon gate]")
                area.m_areaType = 4;
            else if (sub == "[safe zone against a]")
                area.m_areaType = 5;
            else if (sub == "[safe zone against b]")
                area.m_areaType = 6;
            else
                area.m_areaType = 0;
            // ORIG 8aade99..8aadec7：循环 ScanInt 直到 flag 失败，全部 push
            for (;;)
            {
                value = ScanInt(&flag);
                if (!flag)
                    break;
                area.m_field18.push_back(value);
            }
            script->m_areas[area.m_areaIndex] = area;
            continue;
        }
        if (line == "[limit level]")
        {
            int level = ScanInt(&flag);
            // ORIG 8aadf18：`test; je <return false 块（在后）>` ⇒ if/else 正极性
            // （早返形 4.4 恒把 return false 块前置，P59/P222）。
            if (flag)
            {
                script->m_3c = level;
            }
            else
            {
                return false;
            }
            continue;
        }
        if (line == "[need quest]")
        {
            int quest = ScanInt(&flag);
            if (flag)
            {
                script->m_40 = quest;
            }
            else
            {
                return false;
            }
            continue;
        }
        // 未知键：忽略（回到循环读下一行）
    }
    // ORIG 尾部：script->m_field0 = g_townScriptFileList.FindIndexByFullScan(path)
    script->m_field0 = g_townScriptFileList.FindIndexByFullScan(path);
    return true;
