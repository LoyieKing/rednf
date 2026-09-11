// [b28d-solo-STQuestScript · §6.2 三件套] ORIG 0x08aadaa8（382 条）vs 我方 384
// （gap −2）；p167 调用点差集 57/57 CLEAR（0/0，含 P304 权威型 &g_townScriptFileList
// 直压、TownAreaScript 循环外构造 + 块内 clear、ScanInt(NULL)、bool 直存 setne、
// if/else 正极性守卫、Main 代落 T_Rect 去用户析构）。残余 byte 层 N 封顶：
// ① N104/EH 类：我方 TownAreaScript+两 string 的 cleanup 链相对正常路径的块序与
//    ORIG 相反（内容逐条等价，仅尾块互换）；② P223 单源尾岛合并（`je` 落点）；
// ③ 少量 `mov %eax,A(%esp)` vs `(%esp)` 寻址形态。
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
    // [P2S2STQuest 2026-09-11 负结论] 试形 `while (ScanType(...))` 已回滚：4.4 把
    // while 的条件块放在**循环体之后**（我方 head 处 ScanType 段消失、整体 -1 条），
    // 与 ORIG「条件在 head（08aadb33）+ 各 continue 直跳 head（10 条 `jmp 08aadb33`）」
    // 不符。旧有 for(;;) 形 head 正确，仅多一个 continue 中继块（我方 0x69d1）。
    // [P2S2STQuest 2026-09-11 负结论·残余 1 条] 我方 0x69d1 是 8 条 continue 的公共
    // 中继块（`jmp 0x6577`）；ORIG 全部 continue 直跳 head（10 条 `jmp 08aadb33`，无
    // 中继块）。探针 /tmp/p2/probe4.cpp（同构 for(;;)+`if(!ScanType)break`+10 个
    // `if(...){...;continue;}`）**逐条复现我方形态**（9 条中继 + 末条直跳 .L14），
    // 二分支版 /tmp/p2/probe2.cpp(v1) 才是 ORIG 形态；probe2.cpp(v2) 的 `goto next`、
    // probe3.cpp(v7)/probe5.cpp(v14) 的 else-return 变体均**无法**消除中继 ⇒ 4.4 对
    // 多分支 for(;;) 的 continue-label 合并启发式，源码层不可控（P223 族）。
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
                    // [P2S2STQuest 2026-09-11] ORIG 8aadcec `jmp 08aadd30` = 首轮先跳
                    // 条件检查 ⇒ 原码是 while（do/while 无此 jmp，我方旧形多 -1 条）。
                    // 外层 while (flag) 保证进入时 flag 为真 ⇒ 与 do/while 行为等价。
                    while (i <= 3 && flag)
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
                    }                                    // 8aadd30
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
            // [P2S2STQuest 2026-09-11] ORIG 无末位 else：8aade90 `je 08aade99` 直接把
            // [safe zone against b] 不匹配的路径送到共同后继（无 `jmp`+`movl $0x0,-0x4c`
            // 两条）；未匹配标签时 m_areaType 保持 area.clear() 后的值。
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
            // [P2S2STQuest 2026-09-11] ORIG 8aadf15 `mov %eax,-0x18(%ebp)` = 外层 value
            // 槽（非块内新局部）；块内 `int level` 会另占浅槽并把全部槽位压低 8、
            // 帧 0x60→0x70（§P212 同族）。
            value = ScanInt(&flag);
            // ORIG 8aadf18：`test; je <return false 块（在后）>` ⇒ if/else 正极性
            // （早返形 4.4 恒把 return false 块前置，P59/P222）。
            if (flag)
            {
                script->m_3c = value;
            }
            else
            {
                return false;
            }
            continue;
        }
        if (line == "[need quest]")
        {
            // ORIG 8aadf5b `mov %eax,-0x18(%ebp)` ⇒ 同为外层 value 槽复用。
            value = ScanInt(&flag);
            if (flag)
            {
                script->m_40 = value;
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
}