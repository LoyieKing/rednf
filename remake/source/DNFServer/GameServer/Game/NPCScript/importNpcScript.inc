// ----------------------------------------------------------------------------
// importNpcScript —— ORIG 0x089fc264 主分支逐关键字转写。
// 深水区（长尾 keyword 的次要字段、静态 lookup 表优化）以语义等价形式实现，
// 已在对应分支标注「推断」。
// ----------------------------------------------------------------------------
// [b28e-NPCScript 语义修复批] ORIG 0x089fc264：超大函数（>500，§6.2 三件套）。
//   p167 调用点差集 CLEAR（ORIG 351 = OUR 351，仅 C1/C2、D1/D2 同址别名差，P93）。
//   按 call 锚点分段 + 槽号归一化多重集比对，本批修 6 处：
//   ① [web page] 0x089fc74a：ORIG ScanStr 后直接 push_back（无返回值检查）⇒
//      删我方多余的 `if(!ScanStr(&sub)) break;`（P171 照抄）。
//   ② [click mark] 0x089fc94f：ORIG info 在循环外、m_param 仅在 m_type==0 时由
//      ScanInt(&ok) 更新（type!=0 保留旧值）⇒ 改条件写 + info 移出循环。
//   ③ [favor_duration_reward_buff_dialog] 0x089fcf57：ScanInt 实参 NULL→&ok。
//   ④ [change] 0x089fd2f4：ORIG 循环头 `int value = 0` ⇒ 提前声明。
//   ⑤ 函数头 0x089fc2e3：ORIG 函数级 int 槽（-0x1a4，被 [skill]/[speech]/[dialog]/
//      [int data] 复用）⇒ int v 提升函数级（P320 int 版）。
//   ⑥ [reward item] 0x089fdc81/0x089fdcf0/0x089fddeb：三层 `!ScanType || tok==end`
//      拆两条独立 if（P225 推广），消 4.4 setcc 物化。
//   残余 = 4.1/4.4 结构性差异（N 封顶）：ORIG 9 条 continue 中继岛
//   [round5 复核] p167 复算 ORIG 351 calls = OUR 351 calls，MISSING/EXTRA 全部成对
//   为 C1↔C2 / D1↔D2 **同址别名**（`-24 _ZNSsD2Ev` ↔ `+24 _ZNSsD1Ev`、
//   `-11 _ZNSsC2Ev` ↔ `+11 _ZNSsC1Ev`、stGiftItemInfo 的 C2Im… ↔ C1Im… 等，P93）
//   ⇒ **无真语义缺口**；逐助记符比对得 109 个形态差异块（我方 2532 / ORIG 2593），
//   主体为下述 N 族，不属可译缺口。
//   （0x089fc2f4..0x089fc30e 九对 jmp/nop 同落 +0xab，4.4 就地 jmp）、bool 局部
//   物化（ORIG mov %al,-S; movzbl; xor$1 vs 4.4 直接 test，N122）、状态机路径标志
//   （ORIG %esi/%ebx vs 4.4 栈 bool）、`fav` 引用缓存致成员偏移基数差 0x170
//   （P183，语义等价）、帧 0x67c vs 0x66c（N82）。
bool importNpcScript(NpcScript* npc, char const* path)  // ORIG load 调用侧 xor $1 取反 ⇒ bool（P22）
{
    if (!loadRDARScriptFile(g_npcScriptBaseDirectory, path))
    {
        return IMPORT_FAIL;
    }

    npc->clear();
    // ORIG 0x088bbc80：getDirectoryPath(npc->m_directoryPath(+0x100), "%s%s", base, path)
    getDirectoryPath(npc->m_directoryPath, "%s%s", g_npcScriptBaseDirectory, path);

    std::string token;
    std::string sub;
    // [b28d-solo-NPCScript] P320 解析器函数级单槽 bool：ORIG 全函数只有
    // 一个 ok 槽（-0x1a5，函数头 movb $0 一次），各分支复用；原每分支
    // 独立 `bool ok = false;`（20 个槽）使帧 +0x30、槽序整体偏移。
    bool ok = false;
    // ORIG 0x089fc2e3：函数头 movl $0,-0x1a4 一次；该槽被 [skill]/[speech]/
    // [dialog]/[int data] 四段复用（P320 int 版），分支内不得重新初始化。
    int v = 0;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[name]")
        {
            ScanStr(&npc->m_name);
            continue;
        }
        if (token == "[field name]")
        {
            ScanStr(&npc->m_fieldName);
            continue;
        }
        if (token == "[small face]")
        {
            ScanStr(&npc->m_smallFace);
            npc->m_smallFaceId = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (token == "[big face]")
        {
            ScanStr(&npc->m_bigFace);
            npc->m_bigFaceId = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (token == "[popup face]")
        {
            ScanStr(&npc->m_popupFace);
            npc->m_popupFaceId = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (token == "[field animation]")
        {
            ScanStr(&npc->m_fieldAnimation);
            continue;
        }
        if (token == "[field wav]")
        {
            ScanStr(&npc->m_fieldWav);
            continue;
        }
        if (token == "[popup wav]")
        {
            ScanStr(&npc->m_popupWav[0]);
            ScanStr(&npc->m_popupWav[1]);
            continue;
        }
        if (token == "[look around]")
        {
            npc->m_lookAround = ScanInt(static_cast<bool*>(0)) != 0;
            continue;
        }
        if (token == "[role]")
        {
            while (ScanStr(&sub))
            {
                int role = getNpcRoleByString(sub);
                if (role == -1)
                {
                    ScanInt(static_cast<bool*>(0));  // 丢弃数值
                }
                else
                {
                    npc->m_roleMap[static_cast<ENUM_NPC_ROLE>(role)] =
                        ScanInt(static_cast<bool*>(0));
                }
            }
            continue;
        }
        if (token == "[quest speech]")
        {
            ScanStr(&npc->m_questSpeech);
            continue;
        }
        if (token == "[skill]")
        {
            int slot = 0xb;  // [common]
            ScanStr(&sub);
            if (sub == "[common]")
            {
                slot = 0xb;
            }
            else
            {
                slot = getCharacterJob(sub.c_str());
            }
            // ORIG 0x089fc6dd 循环体内每轮重发 `slot*0xc + 0x68` 全链（P183
            // 地址重物化铁律）：旧译文用 `std::vector<int>& list` 缓存基址，
            // 少 6~7 条/轮。展开为逐次 `npc->m_skills[slot]` 表达式。
            npc->m_skills[slot].clear();
            while ((v = ScanInt(&ok), ok))
            {
                npc->m_skills[slot].push_back(v);
            }
            continue;
        }
        if (token == "[web page]")
        {
            int count = ScanInt(static_cast<bool*>(0));
            for (int i = 0; i < count; ++i)
            {
                // ORIG 0x089fc74a：ScanStr 后直接 push_back，无返回值检查（P171 照抄）。
                ScanStr(&sub);
                npc->m_webPages.push_back(sub);
            }
            continue;
        }
        if (token == "[/web page]")
        {
            continue;
        }
        if (token == "[movie path]")
        {
            while (ScanStr(&sub))
            {
                npc->m_movieData.m_movies.push_back(sub);
            }
            npc->m_movieData.m_movieIdx = 0;
            continue;
        }
        if (token == "[/movie path]")
        {
            continue;
        }
        if (token == "[ingame advertisement movie path]")
        {
            while (ScanStr(&sub))
            {
                // ORIG 同样只消费不存储。
            }
            continue;
        }
        if (token == "[speech]")
        {
            // ORIG 用临时 string 消费全部字段，不写回结构。
            std::string tmp;
            ScanInt(static_cast<bool*>(0));
            ScanInt(static_cast<bool*>(0));
            ScanInt(static_cast<bool*>(0));
            ScanStr(&tmp);
            ScanStr(&tmp);
            while ((v = ScanInt(&ok), ok))
            {
            }
            continue;
        }
        if (token == "[click mark]")
        {
            // ORIG 0x089fc94f：info 对象在循环外（-0x1b8/-0x1b4），type 直接写
            // info.m_type；m_param 仅在 type==0 时更新（type!=0 保留旧值，照抄 ORIG）。
            ClickMarkInfo info;
            while ((info.m_type = ScanInt(&ok), ok))
            {
                if (info.m_type == 0)
                {
                    info.m_param = ScanInt(&ok);
                }
                npc->m_clickMarks.push_back(info);
            }
            continue;
        }
        if (token == "[dialog]")
        {
            InteractiveDialogScript dlg;
            dlg.m_a = ScanInt(static_cast<bool*>(0));
            dlg.m_b = ScanInt(static_cast<bool*>(0));
            dlg.m_c = ScanInt(static_cast<bool*>(0));
            int condCount = ScanInt(static_cast<bool*>(0));
            // ORIG 0x089fca26/0x089fca2f：`cmpl $0xffffffff,-0x88; jne else` +
            // then 块 `movb $0x1,-0x648`（对象 +0x18 = m_flag0）——condCount==-1
            // 时置 1，否则保持 clear() 的 0。旧译文缺该写入（真语义缺口）。
            if (condCount == -1)
            {
                dlg.m_flag0 = 1;
            }
            else
            {
                for (int i = 0; i < condCount * 2; ++i)
                {
                    dlg.m_conditions.push_back(ScanInt(static_cast<bool*>(0)));
                }
            }
            dlg.m_d = ScanInt(static_cast<bool*>(0));
            dlg.m_flag = ScanInt(static_cast<bool*>(0)) != 0;
            ScanStr(&dlg.m_str);
            while ((v = ScanInt(&ok), ok))
            {
                dlg.m_list2.push_back(v);
            }
            npc->m_dialogs.push_back(dlg);
            continue;
        }
        if (token == "[/dialog]")
        {
            continue;
        }
        if (token == "[favor_dialog]")
        {
            // ORIG 0x089fcb8e：PresentResponce 是函数局部静态（__cxa_guard + atexit，
            // 无每次调用析构）。逐字段 ScanInt/ScanStr 写入静态对象后 push_back。
            static PresentResponceScript PresentResponce;
            PresentResponce.m_a = ScanInt(static_cast<bool*>(0));
            PresentResponce.m_b = ScanInt(static_cast<bool*>(0));
            PresentResponce.m_c = ScanInt(static_cast<bool*>(0));
            ScanStr(&PresentResponce.m_str);
            npc->m_presentResponces.push_back(PresentResponce);
            continue;
        }
        if (token == "[/favor_dialog]")
        {
            continue;
        }
        if (token == "[favor_dialog_exception]")
        {
            PresentResponceScriptException ex;
            ex.m_a = ScanInt(static_cast<bool*>(0));
            ScanStr(&ex.m_str);
            npc->m_presentResponceExceptions.push_back(ex);
            continue;
        }
        if (token == "[/favor_dialog_exception]")
        {
            continue;
        }
        if (token == "[event_favor_dialog]")
        {
            PresentResponceScriptEvent ev;
            ev.m_a = ScanInt(static_cast<bool*>(0));
            ScanStr(&ev.m_str);
            npc->m_presentResponceEvents.push_back(ev);
            continue;
        }
        if (token == "[/event_favor_dialog]")
        {
            continue;
        }
        if (token == "[1st favor dialog]")
        {
            // ORIG 0x089fce08：声明局部 string + 一块清零缓冲（rep stos 0x100 dword），
            // 循环 ScanStr 到同一 string，取 c_str→strchr('>')，命中则 p+1 strcpy 进缓冲、
            // 再对该 string 赋 const char*（operator=(char const*)），最后 push_back。
            std::string one;
            char gtbuf[0x400] = { '\0' };
            while (ScanStr(&one))
            {
                char* pc = const_cast<char*>(one.c_str());
                // ORIG 0x089fce66/0x089fce80：`movl $0,-0x7c` 前置 + strchr 结果
                // 覆盖同槽 ⇒ 原码是「声明置 0 + 独立赋值」两段式（P198③/P50）。
                char* pg = 0;
                pg = strchr(pc, '>');
                if (pg != 0)
                {
                    pg = pg + 1;
                    strcpy(gtbuf, pg);
                    one = pg;
                    npc->m_firstFavorDialogs.push_back(one);
                }
            }
            continue;
        }
        if (token == "[/1st favor dialog]")
        {
            continue;
        }
        if (token == "[favor_duration_reward_buff_dialog]")
        {
            ScanStr(&npc->m_favorDurationBuffDialog);
            ScanInt(&ok);   // ORIG 0x089fcf57：传 &ok（非 NULL）
            continue;
        }
        if (token == "[string data]")
        {
            npc->m_stringData.clear();
            while (ScanStr(&sub))
            {
                npc->m_stringData.push_back(sub);
            }
            continue;
        }
        if (token == "[int data]")
        {
            npc->m_intData.clear();
            while ((v = ScanInt(&ok), ok))
            {
                npc->m_intData.push_back(v);
            }
            continue;
        }
        if (token == "[balloon message]")
        {
            npc->m_balloonMessages.clear();
            npc->m_balloonTicks.clear();
            while (ScanStr(&sub))
            {
                npc->m_balloonMessages.push_back(sub);
                // ORIG 0x089fd096：ScanInt 结果先存命名 int，再 push_back(ERKi)。
                int tick = ScanInt(static_cast<bool*>(0));
                npc->m_balloonTicks.push_back(tick);
            }
            continue;
        }

        if (token == "[favorable relationship]")
        {
            stFavorableRelationShip_t& fav = npc->m_favorableRelationShip;
            fav.m_enabled = true;

            for (;;)
            {
                if (!ScanType(token, true))
                {
                    break;
                }
                if (token == "[/favorable relationship]")
                {
                    break;
                }
                if (token == "[default favor]")
                {
                    fav.m_defaultFavor = ScanInt(&ok);
                    if (!ok) { return IMPORT_FAIL; }
                    continue;
                }
                if (token == "[max gift per day]")
                {
                    fav.m_maxGiftPerDay = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { return IMPORT_FAIL; }
                    continue;
                }
                if (token == "[dialog day not continuously]")
                {
                    fav.m_dialogDayGap1 = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { return IMPORT_FAIL; }
                    fav.m_dialogDayGap2 = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { return IMPORT_FAIL; }
                    continue;
                }
                if (token == "[mood]")
                {
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;
                        }
                        if (token == "[/mood]")
                        {
                            break;
                        }
                        if (token == "[change]")
                        {
                            for (;;)
                            {
                                // ORIG make_pair<ENUM_NPC_MOOD&, unsigned int>：首参为
                                // ENUM_NPC_MOOD 具名左值，次参 unsigned int 右值临时。
                                ENUM_NPC_MOOD mood = static_cast<ENUM_NPC_MOOD>(3);
                                int value = 0;   // ORIG 0x089fd2f4：循环头 movl $0
                                if (!ScanStr(&sub)) { break; }
                                mood = static_cast<ENUM_NPC_MOOD>(getNpcMoodFromString(sub.c_str()));
                                if (mood == 3) { return IMPORT_FAIL; }
                                value = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_fav.m_moodChanges.push_back(
                                    std::make_pair(mood,
                                                   static_cast<unsigned int>(value)));
                            }
                        }
                        else if (token == "[key item]")
                        {
                            // ORIG make_pair<unsigned long&, unsigned int&>：两参均具名左值。
                            for (;;)
                            {
                                unsigned long key = 0xffffffff;
                                unsigned int id = 0;
                                key = ScanInt(&ok);
                                if (!ok) { break; }
                                id = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_fav.m_keyItems.push_back(std::make_pair(key, id));
                            }
                        }
                    }
                    continue;
                }
                if (token == "[gift item]")
                {
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;
                        }
                        if (token == "[/gift item]")
                        {
                            break;
                        }
                        if (token == "[favor rate per mood]")
                        {
                            fav.m_favorRate[2] =
                                static_cast<float>(ScanInt(&ok)) / 100.0f;
                            if (!ok) { return IMPORT_FAIL; }
                            fav.m_favorRate[1] =
                                static_cast<float>(ScanInt(&ok)) / 100.0f;
                            if (!ok) { return IMPORT_FAIL; }
                            fav.m_favorRate[0] =
                                static_cast<float>(ScanInt(&ok)) / 100.0f;
                            if (!ok) { return IMPORT_FAIL; }
                            continue;
                        }
                        // ORIG 四个独立 gift 块（各自 make_pair<unsigned long&, GiftInfo&>
                        // + map::insert，键 = info.m_a 首字），勿合并（合并=少一半调用符号）。
                        // 组名块按 P48 各自调 getItemGroupNameTable()（find/end 各一次，不缓存）。
                        if (token == "[prefer item]")
                        {
                            int slot = 0;
                            while ((slot = ScanInt(&ok), ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
                                info.m_a = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_b = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_preferItems[slot].insert(
                                    std::make_pair(info.m_a, info));
                            }
                            continue;
                        }
                        if (token == "[prefer item group]")
                        {
                            int slot = 0;
                            while ((slot = ScanInt(&ok), ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                if (!ScanStr(&sub)) { return IMPORT_FAIL; }
                                std::map<std::string, int>::const_iterator it =
                                    getItemGroupNameTable().find(sub);
                                if (it == getItemGroupNameTable().end()) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
                                info.m_a = static_cast<unsigned long>(it->second);
                                info.m_b = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                fav.m_preferItemGroups[slot].insert(
                                    std::make_pair(info.m_a, info));
                            }
                            continue;
                        }
                        if (token == "[unprefer item]")
                        {
                            int slot = 0;
                            while ((slot = ScanInt(&ok), ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
                                info.m_a = ScanInt(&ok);
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_b = 1;
                                info.m_c = -info.m_c;
                                fav.m_unpreferItems[slot].insert(
                                    std::make_pair(info.m_a, info));
                            }
                            continue;
                        }
                        if (token == "[unprefer item group]")
                        {
                            int slot = 0;
                            while ((slot = ScanInt(&ok), ok))
                            {
                                if (slot > 4) { return IMPORT_FAIL; }
                                if (!ScanStr(&sub)) { return IMPORT_FAIL; }
                                std::map<std::string, int>::const_iterator it =
                                    getItemGroupNameTable().find(sub);
                                if (it == getItemGroupNameTable().end()) { return IMPORT_FAIL; }
                                stFavorableRelationShip_t::stGiftItemInfo_t info;
                                info.m_a = static_cast<unsigned long>(it->second);
                                info.m_c = static_cast<short>(ScanInt(&ok));
                                if (!ok) { return IMPORT_FAIL; }
                                info.m_b = 1;
                                info.m_c = -info.m_c;
                                fav.m_unpreferItemGroups[slot].insert(
                                    std::make_pair(info.m_a, info));
                            }
                            continue;
                        }
                        break;  // 推断：未知子 keyword 结束本段处理
                    }
                    continue;
                }
                if (token == "[reward item]")
                {
                    // ORIG 三层嵌套 ScanType 循环（0x89fdc81 外层 [reward item]、
                    // 0x89fdcf0 中层 [item info]、0x89fddeb 内层 [mood]），逐层退出。
                    for (;;)                       // 外层
                    {
                        // ORIG 两条独立检查（xor$1/test/je + test/je 同落出口），
                        // 写 `||` 会被 4.4 折成 setcc 物化（P225 推广：拆嵌套 if）。
                        if (!ScanType(token, true))
                        {
                            break;                  // → 回 favorable 子块
                        }
                        if (token == "[/reward item]")
                        {
                            break;
                        }
                        if (token == "[item info]")
                        {
                            int level = 0;
                            int mood = 3;
                            for (;;)                 // 中层 [item info]
                            {
                                if (!ScanType(token, true))
                                {
                                    break;          // → 回外层
                                }
                                if (token == "[/item info]")
                                {
                                    break;
                                }
                                if (token == "[favor level]")
                                {
                                    level = ScanInt(&ok);
                                    if (!ok) { return IMPORT_FAIL; }
                                }
                                if (token == "[mood]")
                                {
                                    if (!ScanStr(&sub)) { return IMPORT_FAIL; }
                                    mood = getNpcMoodFromString(sub.c_str());
                                    if (mood == 3) { return IMPORT_FAIL; }
                                    if (level == 0) { return IMPORT_FAIL; }
                                    bool stop = false;
                                    while (!stop)   // 内层 [mood]
                                    {
                                        bool haveFavor = false;
                                        for (;;)
                                        {
                                            if (!ScanType(token, true))
                                            {
                                                stop = true;   // → 回中层
                                                break;
                                            }
                                            if (token == "[/mood]")
                                            {
                                                stop = true;   // → 回中层
                                                break;
                                            }
                                            if (token == "[favor]")
                                            {
                                                haveFavor = true;
                                                break;
                                            }
                                        }
                                        if (stop || !haveFavor) { break; }
                                        if (!ScanStr(&sub)) { break; }
                                        int dir = 3;
                                        if (sub == "up")        { dir = 1; }
                                        else if (sub == "down") { dir = 2; }
                                        else if (sub == "keep") { dir = 0; }
                                        if (dir == 3) { return IMPORT_FAIL; }
                                        int a = 0;
                                        while ((a = ScanInt(&ok), ok))
                                        {
                                            stFavorableRelationShip_t::stRewardItemElement_t elem;
                                            elem.m_a = a;
                                            elem.m_b = ScanInt(&ok);
                                            if (!ok) { return IMPORT_FAIL; }
                                            int w = ScanInt(&ok);
                                            if (!ok) { return IMPORT_FAIL; }
                                            // ORIG 0x89fdf4a/0x89fdf86/0x89fe02d：三处各自重算
                                            // m_slots[level][mood] 基址（无引用缓存）；w 为独立
                                            // 栈槽（-0x48），elem.m_w 与写回各读一次 weight
                                            // （0x89fdf79 / 0x89fe021）。
                                            elem.m_w =
                                                w + fav.m_rewardItems.m_slots[level][mood].m_weight[dir];
                                            fav.m_rewardItems.m_slots[level][mood].m_byDir[dir].push_back(elem);
                                            fav.m_rewardItems.m_slots[level][mood].m_weight[dir] += w;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    continue;
                }
                if (token == "[favor level reward mail]")
                {
                    for (;;)
                    {
                        stFavorableRelationShip_t::stLevelRewardInfo info;
                        info.m_level = static_cast<unsigned short>(ScanInt(&ok));
                        if (!ok) { break; }
                        info.m_flag0 = static_cast<unsigned int>(ScanInt(&ok));
                        if (!ok) { break; }
                        info.m_flag1 = static_cast<unsigned int>(ScanInt(&ok));
                        if (!ok) { break; }
                        std::string name;
                        std::string desc;
                        if (!ScanStr(&name)) { break; }
                        if (!ScanStr(&desc)) { break; }
                        info.m_name = name;
                        info.m_desc = desc;
                        fav.m_levelRewards.push_back(info);
                    }
                    continue;
                }
                if (token == "[favor_duration_reward]")
                {
                    // ORIG 0x089fe277：周期奖励直接写入内嵌 stPeriodRewardInfo
                    // （param+0x66c..0x680）：m_type(ushort)/m_name/m_a(int)/m_b(int)/
                    // m_desc/m_param。三临时 string（C1Ev×3，含异常清理 D1Ev），
                    // 成功后 aSERKSs×3 拷入。任一 scan 失败 → 退出 favorable 子块回主循环。
                    unsigned short type = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { break; }
                    std::string name;
                    if (!ScanStr(&name)) { break; }
                    unsigned int a = ScanInt(&ok);
                    if (!ok) { break; }
                    unsigned int b = ScanInt(&ok);
                    if (!ok) { break; }
                    std::string desc;
                    std::string param;
                    if (!ScanStr(&desc)) { break; }
                    if (!ScanStr(&param)) { break; }
                    fav.m_durationReward.m_type = type;
                    fav.m_durationReward.m_name = name;
                    fav.m_durationReward.m_a = a;
                    fav.m_durationReward.m_b = b;
                    fav.m_durationReward.m_desc = desc;
                    fav.m_durationReward.m_param = param;
                    continue;
                }
                if (token == "[favor_duration_reward_buff]")
                {
                    unsigned short a = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { continue; }  // ORIG: 失败回到主循环
                    stFavorableRelationShip_t::stBuffRewardInfo info;
                    info.m_a = a;
                    info.m_b = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok) { continue; }
                    info.m_c = static_cast<unsigned int>(ScanInt(&ok));
                    if (!ok) { continue; }
                    fav.m_buffRewards.push_back(info);
                    continue;
                }
                break;  // 推断：未知子段结束 favorable relationship 块
            }
            continue;
        }

        if (token == "[visible on dungeon clear]")
        {
            npc->m_visibleOnDungeonClear =
                ScanInt(static_cast<bool*>(0)) == 1;
            continue;
        }
        if (token == "[move town limit str]")
        {
            ScanStr(&npc->m_moveTownLimitStr);   // +0xf8
            continue;
        }
        if (token == "[move town confirm str]")
        {
            ScanStr(&npc->m_moveTownConfirmStr); // +0xfc
            continue;
        }
        if (token == "[random message balloon]")
        {
            // ORIG 0x089fe66f：子 keyword 分发用函数局部静态查找表 __lookup
            // (map<char const*,int,less_TCHAR>) + SpinLock __init_lock + bool __inited
            // 双检锁惰性注册（三键 0x6ac/0x6b7/0x6c2）。注册链一次性插入全部三键后置位，
            // 再重查同一 token 走真实解析。
            typedef std::map<char const*, int, less_TCHAR> LookupMap;
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            enum
            {
                TAG_ANIMATION   = 0x6ac,
                TAG_PROBABILITY = 0x6b7,
                TAG_LIST        = 0x6c2
            };

            randomMessageBalloonInfo info;
            std::string sub;
            bool ok = true;
            while (ScanStr(&sub))
            {
                int id;
            recheck:
                if (!__inited)
                {
                    __init_lock.enter();
                    if (__inited)
                    {
                        __init_lock.leave();
                        goto recheck;
                    }
                    id = 0;   // 未就绪 → 走注册链
                }
                else
                {
                    LookupMap::const_iterator it = __lookup.find(sub.c_str());
                    if (it != __lookup.end())
                        id = it->second;
                    else
                        id = -1;
                }

                switch (id)
                {
                case 0:
                case TAG_ANIMATION:
                    if (__inited)
                    {
                        if (!ScanStr(&info.m_randomMessageAnimation)) { ok = false; break; }
                        if (!info.m_randomMessageAnimation.empty())
                            npc->m_balloonInfo.m_randomMessageAnimation =
                                info.m_randomMessageAnimation;
                    }
                    else
                    {
                        __lookup.insert(
                            LookupMap::value_type("[random message animation]",
                                                  static_cast<int>(TAG_ANIMATION)));
                        __lookup.insert(
                            LookupMap::value_type("[random message probability]",
                                                  static_cast<int>(TAG_PROBABILITY)));
                        __lookup.insert(
                            LookupMap::value_type("[random message list]",
                                                  static_cast<int>(TAG_LIST)));
                        __init_lock.leave();
                        __inited = true;
                        goto recheck;
                    }
                    break;
                case TAG_PROBABILITY:
                    if (!ScanInt(&info.m_randomMessageProbability)) { ok = false; break; }
                    if (info.m_randomMessageProbability != 0)
                        npc->m_balloonInfo.m_randomMessageProbability =
                            info.m_randomMessageProbability;
                    break;
                case TAG_LIST:
                {
                    if (!ScanInt(&id)) { ok = false; break; }   // id 复用为 count
                    info.m_randomMessageList.reserve(id);
                    std::string msg;
                    bool bad = false;
                    for (int i = 0; i < id; ++i)
                    {
                        if (!ScanStr(&msg)) { bad = true; break; }
                        info.m_randomMessageList.push_back(msg);
                    }
                    if (bad) { ok = false; break; }
                    if (!info.m_randomMessageList.empty())
                        npc->m_balloonInfo.m_randomMessageList = info.m_randomMessageList;
                    break;
                }
                default:
                    break;
                }
            }
            if (!ok) { return IMPORT_FAIL; }
            continue;
        }
        if (token == "[hide]")
        {
            npc->m_hide = ScanInt(static_cast<bool*>(0)) == 1;
            continue;
        }
        // 推断：未知 keyword 直接跳过（ORIG 无 default 动作）。
    }
    // [b28d-solo-NPCScript] EOF：回写脚本索引并成功返回。ORIG 的
    // FindIndexByFullScan 在循环外（p167 序位 345/351），原
    // `if(!ScanType){…return;}` 把它提前到第 6 位（P164 EOF 落点）。
    npc->m_scriptFileIndex = g_npcScriptFileList.FindIndexByFullScan(path);
    return IMPORT_OK;
}