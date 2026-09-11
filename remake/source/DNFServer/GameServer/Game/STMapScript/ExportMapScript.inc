// [b28d-solo-STMapScript] _Z15ExportMapScriptP11STMapScriptPKc：p167 CLEAR 378/378
//（§6.2 三件套②归零）；超大函数 verdict DIFF 按 §6.2 口径收：残差＝N82 帧/槽号 + N89
// libstdc++ 全内联（ORIG 大量 out-of-line 模板实例），无真语义缺口。
bool ExportMapScript(STMapScript* script, const char* path)
{
    static char szSaveFileName[0x100];                 // 函数级静态 szSaveFileName
    getScriptFileFullPath(szSaveFileName, 0x100,
                          g_szMapScriptBaseDirectory, path);
    // ORIG 槽序实证（-0x94 file / -0x8c tileIdx / -0x85..-0x88 bool4）：
    // 声明位置在 file 初始化之前（实测最优形；仅声明、不初始化，不发指令）。
    bool chL, chR, chT, chB;                           // -0x88..-0x85（screen edge）
    unsigned int tileIdx;                              // -0x8c：tile/exttile 共用循环变量
    FILE* file = fopen(szSaveFileName, "wb");
    if (!file)
        return false;

    STMapScript defaults;                       // auStack_48c：缺省值对照

    if (script->m_str1b0.length() != 0)         // +0x1b0 非空才输出 [map name]
        fprintf(file, "[map name] `%s`\n", script->m_str1b0.c_str());
    fprintf(file, "[player number] %d %d\n",
            script->m_playerCountMin, script->m_playerCountMax);
    fwrite("[pvp start area]\n", 1, 17, file);
    for (int i = 0; i < 3; ++i)                 // words 0x6f..0x72 步进 7
        fprintf(file, "%d %d %d %d\n",
                script->m_pvpStartArea[i].m_x,
                script->m_pvpStartArea[i].m_y,
                script->m_pvpStartArea[i].m_w,
                script->m_pvpStartArea[i].m_h);
    {                                           // practice 区（3/4）任一非零才输出
        bool any = false;
        for (int i = 3; i < 5; ++i)
            if (script->m_pvpStartArea[i].m_x || script->m_pvpStartArea[i].m_y ||
                script->m_pvpStartArea[i].m_w || script->m_pvpStartArea[i].m_h)
                any = true;
        if (any)
        {
            fwrite("[pvp practice start area]\n", 1, 26, file);
            for (int i = 3; i < 5; ++i)
                fprintf(file, "%d %d %d %d\n",
                        script->m_pvpStartArea[i].m_x,
                        script->m_pvpStartArea[i].m_y,
                        script->m_pvpStartArea[i].m_w,
                        script->m_pvpStartArea[i].m_h);
        }
    }
    // ORIG 0x89e3fc0 展开序：cmp $1 je; cmp $2 je; test(=case0); jmp end
    // ⇒ switch、case 书写序 1,2,0（case0 体空）。
    switch (script->m_24c)
    {
    case 1:
        fwrite("[shadow type] `[mirror]`\n", 1, 25, file);
        break;
    case 2:
        fwrite("[shadow type] `[more darker]`\n", 1, 30, file);
        break;
    case 0:
        break;
    }
    if (!script->m_list4.empty())
    {
        fwrite("[dungeon]\n", 1, 10, file);
        for (std::list<int>::iterator it = script->m_list4.begin();
             it != script->m_list4.end(); it++)
            fprintf(file, "%d\n", *it);
        fwrite("[/dungeon]\n", 1, 11, file);
    }
    if (script->m_c == 0)
        fwrite("[type] `[normal]`\n", 1, 18, file);
    else if (script->m_c == 1)
        fwrite("[type] `[boss]`\n", 1, 16, file);
    else if (script->m_c == 2)
        fwrite("[type] `[dummy]`\n", 1, 17, file);
    if (!script->m_recoveryObjects.empty())     // +0x10 [greed]
    {
        std::string grid;
        for (unsigned int i = 0; i < script->m_recoveryObjects.size(); ++i)
        {
            grid += g_gridSymbols[script->m_recoveryObjects[i].first];
            grid += " ";
            grid += g_gridSymbols[script->m_recoveryObjects[i].second];
            if (script->m_recoveryObjects.size() - 1 != i)
                grid += "\n ";
        }
        fprintf(file, "[greed]\n`%s`\n", grid.c_str());
    }
    if (!script->m_vecTile.empty())             // +0x70 [tile]
    {
        fwrite("[tile]\n", 1, 7, file);
        for (tileIdx = 0; tileIdx < script->m_vecTile.size(); ++tileIdx)
            fprintf(file, "`%s`\n", script->m_vecTile[tileIdx].c_str());
        fwrite("[/tile]\n", 1, 8, file);
    }
    if (!script->m_vecExtendedTile.empty())     // +0x7c [extended tile]
    {
        fwrite("[extended tile]\n", 1, 16, file);
        for (tileIdx = 0; tileIdx < script->m_vecExtendedTile.size(); ++tileIdx)
            fprintf(file, "`%s`\n", script->m_vecExtendedTile[tileIdx].c_str());
        fwrite("[/extended tile]\n", 1, 17, file);
    }
    if (script->m_element != 4)                 // +0x6c != Clear 值
        fprintf(file, "[elemental property] `%s`\n",
                getElementName(static_cast<ENUM_ELEMENT>(script->m_element)));
    if (script->m_sightScroll[0] != defaults.m_sightScroll[0])
        fprintf(file, "[far sight scroll] %d\n", script->m_sightScroll[0]);
    if (script->m_sightScroll[1] != defaults.m_sightScroll[1])
        fprintf(file, "[middle sight scroll] %d\n", script->m_sightScroll[1]);
    if (script->m_sightScroll[2] != defaults.m_sightScroll[2])
        fprintf(file, "[near sight scroll] %d\n", script->m_sightScroll[2]);
    if (!script->m_vecBackAni.empty())          // +0xa0 [background animation]
    {
        fwrite("[background animation]\n", 1, 23, file);
        for (unsigned int i = 0; i < script->m_vecBackAni.size(); ++i)
        {
            const char* ord = script->m_vecBackAni[i].m_orderUpper
                                  ? "[upper]" : "[below]";
            fprintf(file, "`%s` `%s` `%s`\n",
                    script->m_vecBackAni[i].m_filename.c_str(),
                    getDrawLayerName(
                        static_cast<ENUM_DRAWLAYER>(script->m_vecBackAni[i].m_layer)),
                    ord);
        }
        fwrite("[/background animation]\n", 1, 24, file);
    }
    if (!script->m_vecTownMovableArea.empty())  // +0xdc [town movable area]
    {
        fwrite("[town movable area]\n", 1, 20, file);
        for (unsigned int i = 0; i < script->m_vecTownMovableArea.size(); ++i)
            fprintf(file, "%d %d %d %d %d %d\n",
                    script->m_vecTownMovableArea[i].m_x,
                    script->m_vecTownMovableArea[i].m_y,
                    script->m_vecTownMovableArea[i].m_w,
                    script->m_vecTownMovableArea[i].m_h,
                    script->m_vecTownMovableArea[i].m_field1c,
                    script->m_vecTownMovableArea[i].m_field20);
        fwrite("[/town movable area]\n", 1, 21, file);
    }
    if (script->m_vecVirtualMovableArea.empty())    // +0xe8：空 → [pathgate pos]
    {
        fwrite("[pathgate pos]\n", 1, 15, file);
        for (int i = 0; i < 4; ++i)                 // words 0x20+i*8 / 0x104+i*8
            fprintf(file, "%d %d\n",
                    script->m_pathgatePos[i * 2],
                    script->m_pathgatePos[i * 2 + 1]);
    }
    else                                        // 非空 → [virtual movable area]
    {
        fwrite("[virtual movable area]\n", 1, 23, file);
        for (unsigned int i = 0; i < script->m_vecVirtualMovableArea.size(); ++i)
            fprintf(file, "%d %d %d %d\n",
                    script->m_vecVirtualMovableArea[i].m_x,
                    script->m_vecVirtualMovableArea[i].m_y,
                    script->m_vecVirtualMovableArea[i].m_w,
                    script->m_vecVirtualMovableArea[i].m_h);
        fwrite("[/virtual movable area]\n", 1, 24, file);
    }
    if (!script->m_vecGuildAgitEntrance.empty())    // +0xf4
    {
        fwrite("[guild agit entrance info]\n", 1, 27, file);
        for (unsigned int i = 0; i < script->m_vecGuildAgitEntrance.size(); ++i)
            fprintf(file, "%d %d %d %d %d %d\n",
                    script->m_vecGuildAgitEntrance[i].m_x,
                    script->m_vecGuildAgitEntrance[i].m_y,
                    script->m_vecGuildAgitEntrance[i].m_w,
                    script->m_vecGuildAgitEntrance[i].m_h,
                    script->m_vecGuildAgitEntrance[i].m_field1c,
                    script->m_vecGuildAgitEntrance[i].m_field20);
        fwrite("[/guild agit entrance info]\n", 1, 28, file);
    }
    {                                           // MapArea@0x140 四字段任一非零
        if (script->m_dungeonStartArea.m_x || script->m_dungeonStartArea.m_y ||
            script->m_dungeonStartArea.m_w || script->m_dungeonStartArea.m_h)
            fprintf(file, "[dungeon start area] %d %d %d %d\n",
                    script->m_dungeonStartArea.m_x,
                    script->m_dungeonStartArea.m_y,
                    script->m_dungeonStartArea.m_w,
                    script->m_dungeonStartArea.m_h);
        if (script->m_summonStartArea.m_x || script->m_summonStartArea.m_y ||
            script->m_summonStartArea.m_w || script->m_summonStartArea.m_h)
            fprintf(file, "[summon start area] %d %d %d %d\n",
                    script->m_summonStartArea.m_x,
                    script->m_summonStartArea.m_y,
                    script->m_summonStartArea.m_w,
                    script->m_summonStartArea.m_h);
    }
    if (!script->m_vecSound.empty())            // +0x194 [sound]
    {
        fwrite("[sound]\n", 1, 8, file);
        for (unsigned int i = 0; i < script->m_vecSound.size(); ++i)
            fprintf(file, "`%s`\n", script->m_vecSound[i].c_str());
        fwrite("[/sound]\n", 1, 9, file);
    }
    if (!script->m_listAnimation.empty())       // +0x1c [animation]
    {
        fwrite("[animation]\n", 1, 12, file);
        std::string layerName;
        for (std::list<STAssignAnimation>::iterator it =
                 script->m_listAnimation.begin();
             it != script->m_listAnimation.end(); it++)
        {
            layerName = getDrawLayerName(
                static_cast<ENUM_DRAWLAYER>((*it).m_layer));
            fprintf(file, "`%s` `%s` %d %d %d\n",
                    (*it).m_name.c_str(), layerName.c_str(),
                    (*it).m_field08, (*it).m_field0c, (*it).m_field10);
        }
        fwrite("[/animation]\n", 1, 13, file);
    }
    if (!script->m_listQuestAnimation.empty())  // +0x24 [quest animation]
    {
        fwrite("[quest animation]\n", 1, 18, file);
        std::string layerName;
        for (std::list<STQuestAnimation>::iterator it =
                 script->m_listQuestAnimation.begin();
             it != script->m_listQuestAnimation.end(); it++)
        {
            layerName = getDrawLayerName(
                static_cast<ENUM_DRAWLAYER>((*it).m_layer));
            fprintf(file, "`%s` `%s` %d %d %d %d\n",
                    (*it).m_name.c_str(), layerName.c_str(),
                    (*it).m_field08, (*it).m_field0c, (*it).m_field10,
                    (*it).m_field14);
        }
        fwrite("[/quest animation]\n", 1, 19, file);
    }
    if (!script->m_listPassiveObject.empty())   // +0x2c [passive object]
    {
        fwrite("[passive object]\n", 1, 17, file);
        for (std::list<STAssignPassiveObject>::iterator it =
                 script->m_listPassiveObject.begin();
             it != script->m_listPassiveObject.end(); it++)
            fprintf(file, "%d %d %d %d\n",
                    (*it).m_0, (*it).m_4, (*it).m_8, (*it).m_c);
        fwrite("[/passive object]\n", 1, 18, file);
    }
    if (!script->m_vecMonster.empty())          // +0x34 [monster]
    {
        fwrite("[monster]\n", 1, 10, file);
        bool isChampion = false;                // cStack_5d
        for (std::vector<STAssignMonster>::iterator it =
                 script->m_vecMonster.begin();
             it != script->m_vecMonster.end(); it++)
        {
            isChampion = false;
            std::string typeTok;                // local_fc（每轮 ctor/dtor）
            std::string roleTok;                // local_100
            if ((*it).m_20 == 0)      typeTok = "`[fixed]`";
            else if ((*it).m_20 == 1) typeTok = "`[half]`";
            else if ((*it).m_20 == 2) typeTok = "`[ratio]`";
            if ((*it).m_24 == 0)      roleTok = "`[normal]`";
            else if ((*it).m_24 == 1) { roleTok = "`[champion]`"; isChampion = true; }
            else if ((*it).m_24 == 3) roleTok = "`[boss]`";
            fprintf(file,
                    "%d %d %d %d %d %d %d %d %s %s%c",
                    (*it).m_0, static_cast<int>((*it).m_4 != 0),
                    (*it).m_8, (*it).m_c, (*it).m_10, (*it).m_14,
                    (*it).m_pair18.m_pair.m_lo,
                    (*it).m_pair18.m_pair.m_hi,
                    typeTok.c_str(), roleTok.c_str(),
                    isChampion ? ' ' : '\n');
            if (isChampion)
                WriteChampionFeatures(file, *it);
        }
        fwrite("[/monster]\n", 1, 11, file);
    }
    if (script->m_vecMonster.size() != 0)       // [monster specific AI]（ORIG 089e565b..）
    {
        fwrite("[monster specific AI]\n", 1, 0x16, file);
        for (unsigned int i = 0; i < script->m_vecMonster.size(); ++i)
        {
            if (script->m_vecMonster[i].m_34 == 0)
                fwrite("`[normal]`\n", 1, 0xb, file);
            else if (script->m_vecMonster[i].m_34 == 1)
                fprintf(file, "%s %d %d\n", "`[zone defence]`",
                        script->m_vecMonster[i].m_pair38.m_arr[0],
                        script->m_vecMonster[i].m_pair38.m_arr[1]);
            else if (script->m_vecMonster[i].m_34 == 2)
            {
                int wayCount = static_cast<int>(
                    script->m_vecMonster[i].m_vec40.size());
                fwrite("`[roaming]`", 1, 0xb, file);
                fprintf(file, " %d", wayCount);
                // [轴 B 反向已试·按 gap 协议回滚] 改 `unsigned k + 直读 size()` 后
                // setcc 计数与 ORIG 全等（18 seta），但 our 2835→**2851**（gap 5→21，
                // 每循环多 1 次 size() 调用 ⇒ +8 条/2 循环）⇒ 违反「gap 严格改善」，
                // 按 Main 协议回滚；ORIG 0x089e5898 的 seta 须靠别的手法（非本旋钮）。
                for (int k = 0; k < wayCount; ++k)
                    fprintf(file, " %d", script->m_vecMonster[i].m_vec40[k].first);
                for (int k = 0; k < wayCount; ++k)
                    fprintf(file, " %d", script->m_vecMonster[i].m_vec40[k].second);
                fputc(10, file);
            }
        }
        fwrite("[/monster specific AI]\n", 1, 0x17, file);
    }
    if (script->m_vecMonster.size() != 0)       // m_4c 任一非默认(100) → team 表
    {
        bool anyTeam = false;
        for (unsigned int i = 0; i < script->m_vecMonster.size(); ++i)
            if (script->m_vecMonster[i].m_4c != 100)
                anyTeam = true;
        if (anyTeam)
        {
            fwrite("[monster team]\n", 1, 15, file);
            for (unsigned int i = 0; i < script->m_vecMonster.size(); ++i)
                fprintf(file, "%d\n", script->m_vecMonster[i].m_4c);
            fwrite("[/monster team]\n", 1, 16, file);
        }
    }
    if (script->m_championRate != defaults.m_championRate)      // +0x4c
        fprintf(file, "[champion] %d\n", script->m_championRate);
    if (!script->m_listItem.empty())            // +0x50 [item]
    {
        fwrite("[item]\n", 1, 7, file);
        for (std::list<STAssignItem>::iterator it =
                 script->m_listItem.begin();
             it != script->m_listItem.end(); it++)
        {
            fprintf(file, "%d %d %d %d %d %d %d %d %d %d",
                    (*it).m_field00, (*it).m_pair04.a[0], (*it).m_pair04.a[1],
                    (*it).m_field0c, (*it).m_field10, (*it).m_field14,
                    (*it).m_field18, (*it).m_field1c, (*it).m_field20,
                    static_cast<int>((*it).m_generateInfos.size()));
            if ((*it).m_generateInfos.size() != 0)  // ORIG：仅非空才 fputc(10)
                fputc(10, file);
            for (unsigned int k = 0; k < (*it).m_generateInfos.size(); ++k)
                fprintf(file, "%d %d\n",
                        (*it).m_generateInfos[k].m_field00,
                        (*it).m_generateInfos[k].m_field04);
            fputc(10, file);
        }
        fwrite("[/item]\n", 1, 8, file);
    }
    if (!script->m_listSpecialPassive.empty())  // +0x58 [special passive object]
    {
        fwrite("[special passive object]\n", 1, 25, file);
        for (std::list<STAssignSpecialPassiveObj>::iterator it =
                 script->m_listSpecialPassive.begin();
             it != script->m_listSpecialPassive.end(); it++)
        {
            STAssignSpecialPassiveObj* p = &*it;            // 单次 de，后续全用指针
            if (p->m_vec10.size() == 0)
                fprintf(file, "%d %d %d %d %d\n",
                        p->m_0, p->m_4, p->m_8, p->m_c,
                        static_cast<int>(p->m_vec10.size()));
            else
            {
                fprintf(file, "%d %d %d %d\n",
                        p->m_0, p->m_4, p->m_8, p->m_c);
                fprintf(file, "%d\n", static_cast<int>(p->m_vec10.size()));
            }
            for (unsigned int k = 0; k < p->m_vec10.size(); ++k)
            {
                int* act = reinterpret_cast<int*>(&p->m_vec10[k]);
                for (int j = 0; j < 6; ++j)
                {
                    if (j == 0)
                    {
                        if (*act == 0)
                            fwrite("`[monster]` ", 1, 0xc, file);
                        else if (*act == 1)
                            fwrite("`[trap]` ", 1, 9, file);
                        else if (*act == 2)
                            fwrite("`[item]` ", 1, 9, file);
                        else if (*act == 3)
                            fwrite("`[quest]` ", 1, 10, file);
                    }
                    else
                        fprintf(file, "%d ", act[j]);
                }
                fputc(10, file);
            }
        }
        fwrite("[/special passive object]\n", 1, 26, file);
    }
    if (!script->m_vecNpc.empty())              // +0x60 [NPC]
    {
        fwrite("[NPC]\n", 1, 6, file);
        for (std::vector<AssignNpc>::iterator it = script->m_vecNpc.begin();
             it != script->m_vecNpc.end(); it++)
        {
            std::string dirTok;                             // local_110（每轮 ctor/dtor）
            if ((*it).m_dir == 0)
                dirTok = "`[left]`";
            else
                dirTok = "`[right]`";
            fprintf(file, "%d %s %d %d %d\n",
                    (*it).m_npcIdx, dirTok.c_str(),
                    (*it).m_x, (*it).m_y, (*it).m_z);
        }
        fwrite("[/NPC]\n", 1, 7, file);
    }
    if (script->m_vecMonsterSpawnPos.size() != 0) // +0x250 [monster spawn pos]
    {                                           // 块序=ORIG 源序（0x89e624x spawn 先、
        fwrite("[monster spawn pos]\n", 1, 20, file);   //  0x89e637a ai 后；loop var 槽
        fprintf(file, "%d\n", static_cast<int>(script->m_vecMonsterSpawnPos.size()));
        for (unsigned int i = 0; i < script->m_vecMonsterSpawnPos.size(); ++i)
            fprintf(file, "%d %d %d\n",
                    script->m_vecMonsterSpawnPos[i].m_field00,
                    script->m_vecMonsterSpawnPos[i].m_field04,
                    script->m_vecMonsterSpawnPos[i].m_field08);
        fwrite("[/monster spawn pos]\n", 1, 21, file);
    }
    else if (script->m_vecAICharacter.size() != 0)  // +0x25c [ai character]
    {
        fwrite("[ai character]\n", 1, 15, file);
        for (unsigned int i = 0; i < script->m_vecAICharacter.size(); ++i)
        {
            fprintf(file, "%d %d %d %d ",
                    script->m_vecAICharacter[i].m_type,
                    script->m_vecAICharacter[i].m_field04,
                    script->m_vecAICharacter[i].m_field08,
                    script->m_vecAICharacter[i].m_field0c);
            if (script->m_vecAICharacter[i].m_camp == 0)
                fwrite("`[character]`", 1, 13, file);
            else if (script->m_vecAICharacter[i].m_camp == 100)
                fwrite("`[monster]`", 1, 11, file);
            else if (script->m_vecAICharacter[i].m_camp == 200)
                fwrite("`[neutral]`", 1, 11, file);
            fputc(' ', file);
            if (script->m_vecAICharacter[i].m_roleType == 5)
                fwrite("`[normal]`", 1, 10, file);
            else if (script->m_vecAICharacter[i].m_roleType == 6)
                fwrite("`[champion]`", 1, 12, file);
            else if (script->m_vecAICharacter[i].m_roleType == 7)
                fwrite("`[super champion]`", 1, 18, file);
            else if (script->m_vecAICharacter[i].m_roleType == 8)
                fwrite("`[boss]`", 1, 8, file);
            else
                fwrite("`[normal]`", 1, 10, file);
            fputc(' ', file);
            fprintf(file, "%d %d",
                    static_cast<int>(script->m_vecAICharacter[i].m_posX),
                    static_cast<int>(script->m_vecAICharacter[i].m_posY));
            fputc(10, file);
        }
        fwrite("[/ai character]\n", 1, 16, file);
    }
    if (!script->m_mapLoadingImagePath.empty()) // +0x26c（ORIG：fprintf 无 \n，089e67b4）
        fprintf(file, "[map loading image path] %s",
                script->m_mapLoadingImagePath.c_str());
    // ORIG 0x89e67d5 `call empty; xor $1; test; je`：非空分支为 fall-through
    // （[event monster position] 就地），空分支 je 至远端（[move layered map]）。
    // 源形状必须是 `if (!empty()) {monster} else {layered}` 才能复现块序。
    if (!script->m_vecEventMonsterPos.empty())  // +0x278 非空 → [event monster position]
    {
        fwrite("[event monster position]\n", 1, 25, file);
        for (unsigned int i = 0; i < script->m_vecEventMonsterPos.size(); ++i)
        {
            STRivalMapData* p = &script->m_vecEventMonsterPos[i];  // 单次 ix
            fprintf(file, "%d %d %d\n", p->m_field00, p->m_field04, p->m_field08);
        }
        fwrite("[/event monster position]\n", 1, 26, file);
    }
    else                                        // 空 → [move layered map]
    {
        if (script->m_moveLayeredType != 4)
        {
            fwrite("[move layered map]\n", 1, 19, file);
            if (script->m_allDeadCasePassable)
                fwrite("\t[all dead case passable]\n", 1, 26, file);
            if (script->m_moveLayeredType == 1)
                fprintf(file, "\t[zpos under] %d\n", script->m_zposUnder);
            fwrite("[/move layered map]\n", 1, 20, file);
        }
    }
    if (!script->m_vecStaticStartPos.empty())   // +0x2e8
    {
        fwrite("[static player start pos]\n", 1, 26, file);
        for (int i = 0; i < 4; ++i)
        {
            if (static_cast<unsigned int>(i) < script->m_vecStaticStartPos.size())
                fprintf(file, "\t[player %d] %d %d %d\n",
                        script->m_vecStaticStartPos[i].m_playerIdx + 1,
                        script->m_vecStaticStartPos[i].m_x,
                        script->m_vecStaticStartPos[i].m_y,
                        script->m_vecStaticStartPos[i].m_z);
        }
        fwrite("[/static player start pos]\n", 1, 27, file);
    }
    {                                           // screen edge（对照 defaults）
        chL = script->m_screenEdgeLeft != defaults.m_screenEdgeLeft;
        chR = script->m_screenEdgeRight != defaults.m_screenEdgeRight;
        chT = script->m_screenEdgeTop != defaults.m_screenEdgeTop;
        chB = script->m_screenEdgeBottom != defaults.m_screenEdgeBottom;
        if (chL || chR || chT || chB)
        {
            fwrite("[customized screen edge]\n", 1, 25, file);
            if (chL) fprintf(file, "[left] %d\n", script->m_screenEdgeLeft);
            if (chR) fprintf(file, "[right] %d\n", script->m_screenEdgeRight);
            if (chT) fprintf(file, "[top] %d\n", script->m_screenEdgeTop);
            if (chB) fprintf(file, "[bottom] %d\n", script->m_screenEdgeBottom);
            fwrite("[/customized screen edge]\n", 1, 26, file);
        }
    }
    if (script->m_blockUseActiveSkill)
        fwrite("[block use active skill]\n", 1, 25, file);
    if (script->m_blockUseStackableItem)
        fwrite("[block use stackable item]\n", 1, 27, file);

    fclose(file);                               // func_0x0807dea0
    convertBinaryToText(szSaveFileName);
    return true;
}