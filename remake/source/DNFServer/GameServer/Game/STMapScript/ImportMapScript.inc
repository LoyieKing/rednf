// [b28d-solo-STMapScript] _Z15ImportMapScriptP11STMapScriptPKc：p167 残 1/851——
// vector<pair<int,int>>::push_back 选 const& 而 ORIG 选 &&（同 ScanChampionFeatures 的
// 重载选择族，int 对拷贝/移动等价 ⇒ N）。
// 已闭合：SPOBJ_ACTION_DATAS::D1Ev ×2 经 P317 删用户空析构（Main 代落 SPOBJ_ACTION_DATAS.h/
// .cpp）后消失（ORIG 0x089e0a66 循环内 C1Ev 后无 D1 call）。
// 超大函数 verdict DIFF 按 §6.2 口径收：残差＝N82/N89 + P163 决策树密度。
// 修法：m_vec180 嵌套转换临时形、[hellparty] make_pair 键 int 化等（与 proc 同族）。
// [N3Free171to190.S1STMapScript 收尾判词] ①**串审计（§6.5/§6.5.1）**：strall 报的 12 个
//   O-only 串**全为假阳性**——0x8ae5848 = `__dso_handle`（`__cxa_atexit` 第 3 实参）、
//   0x94fe4xx = 函数局部静态 guard 变量（`__cxa_guard_acquire/release` 操作数）⇒ 无语义缺口。
//   ②**__uninitialized_copy 族（本 TU 9 个 DIFF 实例）判 N**（libstdc++ 头集族，源码不可控）：
//   ORIG `...IPK9AssignNpc...` 65 条含 `__cxa_begin_catch`/`__cxa_rethrow`/`__cxa_end_catch`/
//   `_Unwind_Resume` 四个 EH 回滚调用，我方同实例 48 条 **0 个 cxa**；对偶
//   `...IPK17TournamentMonster...` ORIG 39 条（主循环直落 ret，无 cleanup 段）vs 我方 56 条
//   （idx41..55 = 15 条 cleanup）⇒ ±17 的对称差即 EH cleanup 归属/生成差异（N89/N130）。
bool ImportMapScript(STMapScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_szMapScriptBaseDirectory, path))
        return false;
    script->Clear();
    getDirectoryPath(script->m_str1a0, "%s%s", g_szMapScriptBaseDirectory, path);

    // 声明序 = ORIG prologue 构造/初始化序（槽位锚点，报告 0x0071..0x0126）：
    std::string tok;                                // -0x42c 主循环 token
    std::string sub;                                // -0x430 分支内子 token
    std::string tok3;                               // -0x434（[NPC] ScanStrOrInt 目标）
    int okInt = 0;                                  // -0x438（ScanInt 返回值暂存）
    bool ok = false;                                // -0x439（共享 ok flag）
    // ORIG 函数头构造、跨轮复用的解析对象（清理路径仅 dtor 其中 3 个非平凡者）。
    STAssignAnimation aniObj;                       // -0x450
    STAssignPassiveObject passiveObj;               // -0x460
    STAssignMonster monObj;                         // -0x76c
    STAssignSpecialPassiveObj spoObj;               // -0x67c
    AssignNpc npcObj;                               // -0x47c
    int normalMonsterCount = 0;                     // -0x13c（[monster] 计数）
    STAssignBloodSystem bloodSys;                   // -0x6a4
    STAssignBloodPhaseTime bloodPt;                 // -0x498
    STAssignDefenseSystem defSys;                   // -0x6cc
    STAssignDefensePhaseTime defPt;                 // -0x4b0
    bool pvpPracticeRead = false;                   // -0x135
    // ORIG 0x89dd081..0x89dd0aa：prologue 末尾至循环顶之间有 13 对
    // `jmp short <loop head>` + `nop` 填充（39 字节，不可达，ORIG 编译产物
    // 形态；nop 被 AE 口径剥离，13 条 jmp 参与计数）。以 .byte 序列原样补齐，
    // rel8 递减 3 与 ORIG 一致。
    __asm__ __volatile__(
        ".byte 0xeb,0x28,0x90, 0xeb,0x25,0x90, 0xeb,0x22,0x90\n\t"
        ".byte 0xeb,0x1f,0x90, 0xeb,0x1c,0x90, 0xeb,0x19,0x90\n\t"
        ".byte 0xeb,0x16,0x90, 0xeb,0x13,0x90, 0xeb,0x10,0x90\n\t"
        ".byte 0xeb,0x0d,0x90, 0xeb,0x0a,0x90, 0xeb,0x07,0x90\n\t"
        ".byte 0xeb,0x04,0x90");
    for (;;)
    {
        if (!ScanType(tok, true))
            break;

        if (tok == "[map name]")                            // 0x089dd0c1
        {
            ScanStr(&script->m_str1b0);
            continue;
        }
        if (tok == "[player number]")                       // 0x089dd10e
        {
            script->m_playerCountMin = ScanInt(static_cast<bool*>(0));
            script->m_playerCountMax = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[show dust]")                           // 0x089dd157
        {
            ScanStr(&sub);
            script->m_str1a4.append(sub);                   // Ss::append(+0x69)
            ScanStr(&sub);
            script->m_str1a8.append(sub);                   // Ss::append(+0x6a)
            continue;
        }
        if (tok == "[sort bottom]")                         // 0x089dd1c8
        {
            script->m_sortBottom = ScanInt(static_cast<bool*>(0)) == 1;
            continue;
        }
        if (tok == "[pvp start area]")                      // 0x089dd202
        {
            for (int i = 0; i < 3; ++i)
            {
                script->m_pvpStartArea[i].m_x = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_y = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_w = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_h = ScanInt(static_cast<bool*>(0));
            }
            continue;
        }
        if (tok == "[pvp start area by party slot]")        // 0x089dd309
        {
            int slotVal = ScanInt(static_cast<bool*>(0));   // -0x130（ORIG 先落槽再判）
            script->m_pvpByPartySlot = slotVal != 0;
            continue;
        }
        if (tok == "[map move keep state axis pos]")        // 0x089dd34d
        {
            int axisX = ScanInt(static_cast<bool*>(0));     // -0x12c
            int axisY = ScanInt(static_cast<bool*>(0));     // -0x128
            int axisZ = ScanInt(static_cast<bool*>(0));     // -0x124
            script->m_keepStateAxisX = axisX != 0;
            script->m_keepStateAxisY = axisY != 0;
            script->m_keepStateAxisZ = axisZ != 0;
            continue;
        }
        if (tok == "[pvp practice start area]")             // 0x089dd3db
        {
            for (int i = 3; i < 5; ++i)
            {
                script->m_pvpStartArea[i].m_x = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_y = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_w = ScanInt(static_cast<bool*>(0));
                script->m_pvpStartArea[i].m_h = ScanInt(static_cast<bool*>(0));
            }
            pvpPracticeRead = true;                         // cStack_139=1
            continue;
        }
        if (tok == "[shadow type]")                         // 0x089dd4e9
        {
            ScanStr(&sub);
            if (sub == "[mirror]")
                script->m_24c = 1;                          // +0x24c
            else if (sub == "[more darker]")
                script->m_24c = 2;
            continue;
        }
        if (tok == "[dungeon]")                             // 0x089dd56d
        {
            script->m_list4.clear();
            // [lane22 A/B 已回滚] ORIG 0x89dd57f 为 4.1 while 旋转 + bool 取反物化
            // （`movzbl ok; xor $1; test; je body`）。goto 尾标签环（探针 p4.cpp r3，
            // 唯一能出 `je` 的形）在真实上下文实测**不收敛**：4.4 多入口 `jmp L_cond`
            // + `nop` 且 body/cond 物理序仍相反（3673→3675，xor1 91→92）⇒ 回滚。
            while (okInt = ScanInt(&ok), ok == true)
                script->m_list4.push_back(okInt);
            continue;
        }
        if (tok == "[type]")                                // 0x089dd5d7
        {
            ScanStr(&sub);
            if (sub == "[normal]")
                script->m_c = 0;
            else if (sub == "[boss]")
                script->m_c = 1;
            else if (sub == "[dummy]")
                script->m_c = 2;
            continue;
        }
        if (tok == "[background pos]")                      // 0x089dd682
        {
            script->m_32c = ScanInt(static_cast<bool*>(0)); // param_1[0xcb]
            continue;
        }
        if (tok == "[greed]")                               // 0x089dd6b6
        {
            script->m_recoveryObjects.clear();
            ScanStr(&sub);
            // 声明序 = ORIG 初始化序（-0x11c/-0x118/-0x4b4/-0x114/-0x110）
            int len = static_cast<int>(sub.size());         // -0x11c
            int badCount = 0;                               // -0x118（未识别符号数）
            int pending = -1;                               // -0x4b4
            int pos = 0;                                    // -0x114
            int unusedGrid = 0;                             // -0x110（ORIG dead 槽，P31）
            while (pos < len - 1)
            {
                std::string two;                            // -0x4b8（每轮 ctor/dtor）
                two.append(sub.c_str() + pos, 2);           // Ss::append(PKcj)
                if (*two.c_str() == '\r' || *two.c_str() == '\n' ||
                    *two.c_str() == ' ')
                {
                    pos += 1;                               // 换行/空白：跳一格
                }
                else
                {
                    int sym = 0;                            // -0x4bc
                    for (; sym < 16; ++sym)
                    {
                        if (two.c_str()[0] == g_gridSymbols[sym][0] &&
                            two.c_str()[1] == g_gridSymbols[sym][1])
                            break;
                    }
                    if (sym == 16)
                    {
                        pos += 1;                           // 未识别符号：跳一格
                        ++badCount;
                    }
                    else
                    {
                        if (pending == -1)
                            pending = sym;
                        else
                        {
                            // [P6S3 round6 · p167fix 真 call 差集线索] ORIG 调用点为目标
                            // _ZNSt6vectorISt4pairIiiESaIS1_EE9push_backEOS1_（pair&& 重载），
                            // 我方原具名局部 objPair 走 const&（EXTRA）。改传临时实参
                            // （语义等价：值同 (pending, sym)）以命中 && 重载。
                            script->m_recoveryObjects.push_back(
                                std::pair<int, int>(pending, sym));
                            pending = -1;
                        }
                        pos += 2;
                    }
                }
            }
            if (badCount != 0)
                return false;                               // ORIG：有未识别格 → 失败
            (void)unusedGrid;
            continue;
        }
        if (tok == "[tile]")                                // 0x089dd90x 区
        {
            script->m_vecTile.clear();
            while (ScanStr(&sub))                           // 复用子 token（ORIG local_434）
                script->m_vecTile.push_back(sub);
            continue;
        }
        if (tok == "[extended tile]")
        {
            script->m_vecExtendedTile.clear();
            while (ScanStr(&sub))
                script->m_vecExtendedTile.push_back(sub);
            continue;
        }
        if (tok == "[elemental property]")                  // 0x089ddaef 区
        {
            ScanStr(&sub);
            script->m_element = getElement(sub.c_str());    // param_1[0x1b]
            continue;
        }
        if (tok == "[far sight scroll]")
        {
            script->m_sightScroll[0] = ScanInt(static_cast<bool*>(0));  // param_1[0x22]
            continue;
        }
        if (tok == "[middle sight scroll]")
        {
            script->m_sightScroll[1] = ScanInt(static_cast<bool*>(0));  // param_1[0x23]
            continue;
        }
        if (tok == "[near sight scroll]")
        {
            script->m_sightScroll[2] = ScanInt(static_cast<bool*>(0));  // param_1[0x24]
            continue;
        }
        if (tok == "[background animation]")                // 0x089ddaf0 区（__lookup 表段）
        {
            // ORIG：局部对象循环复用（BackAniScript ctor / string ctor 在 do 之前，
            // dtor 在段尾）；子键分派走函数局部静态表 __lookup / __lookup。
            script->m_vecBackAni.clear();                   // vector(+0xa0)::clear
            BackAniScript ani;                              // BackAniScript::BackAniScript
            std::string line;                               // local_4d8
            bool finished = false;                          // local_10d
            do
            {
                if (!ScanType(line, true))
                    return false;
                int id;
                while (true)                                // 构建轮完成后重入 find（0x1608）
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                for (;;)
                {
                    if (!__inited)
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(line.c_str());
                    if (it != __lookup.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x1ca:                                 // [ani info]
                {
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ani info]", 0x1ca));
                        /* fall through */
                        goto ani_next210;
                    }
                    ani.clear();                            // BackAniScript::clear
                    std::string subLine;                    // local_4e0
                    bool aniDone = false;                   // local_105
                    do
                    {
                        if (!ScanType(subLine, true))
                            return false;
                        int subId;
                        while (true)
                        {
                            static std::map<const char*, int, less_TCHAR> __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                        for (;;)
                        {
                            if (!__inited)
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                subId = 0;
                                break;
                            }
                            std::map<const char*, int, less_TCHAR>::const_iterator it =
                                __lookup.find(subLine.c_str());
                            if (it != __lookup.end())
                                subId = it->second;
                            break;
                        }
                        switch (subId)
                        {
                        case 0:
                        case 0x1d7:                         // [filename]
                        {
                            if (!__inited)
                            {
                                // [b53FIX · 惰性建链每环必须落到**下一个 case 的守卫**]
                                // ORIG 089de0ab（[filename] insert 返回后）`jmp 0x89de0f6`
                                // = `[layer]`(0x1dd) 的 `movzbl 0x94fe59c` 守卫；其后各环同形：
                                //   0x1dd → 0x89de198（[order]）
                                //   0x1e3 → 0x89de274（[loop y axis]）
                                //   0x1ee → 0x89de315（[screen pos]）
                                //   0x1f8 → 0x89de3e5（[/ani info]）
                                //   0x205 → 0x89de464（sub_next，末环）
                                // 本方旧形每环都写 `goto sub_next` ⇒ 建表轮只插入 **[filename]** 一项，
                                // 其余 5 个 sub-tag 的 find 永远失败；而 `int subId;` 无初值、失败棱形
                                // 不写回（ORIG 089de161/089de211 `je` 直接跳过 `mov %eax,-0x118(%ebp)`）
                                // ⇒ subId 保留上轮陈旧值 0x1d7。
                                // 实机 A/B 实证（Frida 17.18，/data/ab_sub_ours.js 钩住内层分派点
                                // 0x8621622 读 [ebp-0x118]）：
                                //   line="[filename]" → subId=0x1d7 ✓
                                //   line="[layer]"    → subId=0x1d7 ✗（应 0x1dd）
                                //   line="[order]"    → subId=0x1d7 ✗（应 0x1e3）
                                //   line="[/ani info]"→ subId=0x1d7 ✗（应 0x205）
                                // ⇒ 误入本分支，ScanStr 消费 `[/ani info]` 后的 token 失败
                                // ⇒ ImportMapScript("Lorien/s0.map") ret=0 ⇒ `Loading Maps Fail`。
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[filename]", 0x1d7));
                            }
                            else
                            {
                                if (!ScanStr(&sub))
                                    return false;
                                ani.m_filename = sub;           // Ss::operator=(RKSs)
                                goto sub_tail;
                            }
                        }
                        case 0x1dd:                         // [layer]
                        {
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[layer]", 0x1dd));
                            }
                            else
                            {
                                if (!ScanStr(&sub))
                                    return false;
                                ani.m_layer = getDrawLayer(sub.c_str());  // local_4d0
                                goto sub_tail;
                            }
                        }
                        case 0x1e3:                         // [order]
                        {
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[order]", 0x1e3));
                            }
                            else
                            {
                                if (!ScanStr(&sub))
                                    return false;
                                if (sub == "[below]")
                                    ani.m_orderUpper = false;   // local_4cc=0
                                else if (sub == "[upper]")
                                    ani.m_orderUpper = true;    // local_4cc=1
                                else
                                    return false;
                                goto sub_tail;
                            }
                        }
                        case 0x1ee:                         // [loop y axis]
                        {
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[loop y axis]", 0x1ee));
                            }
                            else
                            {
                                int flag = -1;                  // local_4e8
                                if (ScanInt(&flag) != 1)
                                    return false;
                                ani.m_loopYAxis = (flag != 0);  // uStack_4cb
                                goto sub_tail;
                            }
                        }
                        case 0x1f8:                         // [screen pos]
                        {
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[screen pos]", 0x1f8));
                            }
                            else
                            {
                                int v = -1;                     // local_4ec
                                if (ScanInt(&v) != 1)
                                    return false;
                                ani.m_screenX = v;              // uStack_4c8
                                if (ScanInt(&v) != 1)
                                    return false;
                                ani.m_screenY = v;              // uStack_4c4
                                ani.m_hasScreenPos = true;      // uStack_4ca
                                goto sub_tail;
                            }
                        }
                        case 0x205:                         // [/ani info]
                        {
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/ani info]", 0x205));
                                goto sub_next;
                            }
                            script->m_vecBackAni.push_back(ani);  // vector(+0xa0)
                            aniDone = true;
                            goto sub_tail;
                            break;
                        }
                        default:
                            break;
                        }
                    sub_next:
                            if (!__inited)
                        {
                                    __init_lock.leave();
                                __inited = true;
                            continue;                       // 重入本轮 find（0x1157）
                        }
                    sub_tail:
                        break;
                        }
                    } while (!aniDone);
                    break;
                }
                ani_next210:
                case 0x210:                                 // [/background animation]
                {
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/background animation]", 0x210));
                        break;
                    }
                    finished = true;
                    break;
                }
                default:
                    break;
                }
                    if (!__inited)
                {
                            __init_lock.leave();
                        __inited = true;
                    continue;                               // 重入本轮 find（0xbed）
                }
                        break;
                }
            } while (!finished);
            continue;
        }
        if (tok == "[town movable area]")                   // 0x089dbc5 区
        {
            script->m_vecTownMovableArea.clear();
            okInt = ScanInt(&ok);                           // 0x16bf：循环前一次消费
            while (ok == true)
            {
                TownPath tp;                                // -0x704 TownPath::TownPath
                tp.m_x = okInt;
                tp.m_y = ScanInt(static_cast<bool*>(0));
                tp.m_w = ScanInt(static_cast<bool*>(0));
                tp.m_h = ScanInt(static_cast<bool*>(0));
                tp.m_field1c = ScanInt(static_cast<bool*>(0));
                tp.m_field20 = ScanInt(static_cast<bool*>(0));
                {                                           // 可选 [quest condition]
                    bool isType;                            // -0x4ed（ORIG 无初始化存储）
                    std::string s;                          // -0x4ec
                    okInt = ScanTypeOrInt(isType, s, ok, true);
                    if (isType != false)
                    {
                        if (s == "[quest condition]")
                        {
                            while (okInt = ScanInt(&ok), ok == true)
                                tp.m_conds.push_back(okInt);
                            okInt = ScanInt(&ok);           // ORIG while 后额外消费一 token
                        }
                        else
                            ok = false;                     // 0x1814：非 quest → 终止循环
                    }
                }
                script->m_vecTownMovableArea.push_back(tp);
            }
            continue;
        }
        if (tok == "[virtual movable area]")
        {
            script->m_vecVirtualMovableArea.clear();
            okInt = ScanInt(&ok);                           // 0x18ee
            while (ok == true)
            {
                MapArea area;                               // -0x50c MapArea::MapArea
                area.m_x = okInt;
                area.m_y = ScanInt(static_cast<bool*>(0));
                area.m_w = ScanInt(static_cast<bool*>(0));
                area.m_h = ScanInt(static_cast<bool*>(0));
                {                                           // 可选 [quest condition]
                    bool isType;                            // -0x511（ORIG 无初始化存储）
                    std::string s;                          // -0x510
                    okInt = ScanTypeOrInt(isType, s, ok, true);
                    if (isType != false)
                    {
                        if (s == "[quest condition]")
                        {
                            while (okInt = ScanInt(&ok), ok == true)
                                area.m_list.push_back(okInt);
                            okInt = ScanInt(&ok);
                        }
                        else
                            ok = false;                     // 0x19ed：非 quest → 终止循环
                    }
                }
                script->m_vecVirtualMovableArea.push_back(area);
            }
            continue;
        }
        if (tok == "[guild agit entrance info]")
        {
            script->m_vecGuildAgitEntrance.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x1a95：赋值在循环头
            {
                TownPath tp;                                // -0x704 复用
                tp.m_x = okInt;
                tp.m_y = ScanInt(static_cast<bool*>(0));
                tp.m_w = ScanInt(static_cast<bool*>(0));
                tp.m_h = ScanInt(static_cast<bool*>(0));
                tp.m_field1c = ScanInt(static_cast<bool*>(0));
                tp.m_field20 = ScanInt(static_cast<bool*>(0));
                script->m_vecGuildAgitEntrance.push_back(tp);
            }
            continue;
        }
        if (tok == "[pathgate pos]")
        {
            for (int i = 0; i < 4; ++i)                 // -0xfc/-0xf8 双层 setle 循环
                for (int j = 0; j < 2; ++j)
                    script->m_pathgatePos[i * 2 + j] =
                        ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[pathgate recognize range]")
        {
            for (int i = 0; i < 4; ++i)                 // -0xf4/-0xf0
                for (int j = 0; j < 2; ++j)
                    script->m_pathgateRange[i * 2 + j] =
                        ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[dungeon start area]")
        {
            script->m_dungeonStartArea.m_x = ScanInt(static_cast<bool*>(0));
            script->m_dungeonStartArea.m_y = ScanInt(static_cast<bool*>(0));
            script->m_dungeonStartArea.m_w = ScanInt(static_cast<bool*>(0));
            script->m_dungeonStartArea.m_h = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[opening bgm]")
        {
            ScanStr(&sub);
            script->m_openingBgm = sub;                     // Ss::assign(RKSs)
            script->m_openingBgmParam = ScanInt(static_cast<bool*>(0));  // +0x190
            continue;
        }
        if (tok == "[sound]")
        {
            script->m_vecSound.clear();
            while (ScanStr(&sub))
                script->m_vecSound.push_back(sub);
            continue;
        }
        if (tok == "[animation]")
        {
            script->m_listAnimation.clear();
            while (ScanStr(&aniObj.m_name))                 // 函数头复用对象（无逐轮 clear）
            {
                ScanStr(&sub);
                aniObj.m_layer = getDrawLayer(sub.c_str());
                aniObj.m_field08 = ScanInt(static_cast<bool*>(0));
                aniObj.m_field0c = ScanInt(static_cast<bool*>(0));
                aniObj.m_field10 = ScanInt(static_cast<bool*>(0));
                script->m_listAnimation.push_back(aniObj);
            }
            continue;
        }
        if (tok == "[passive object]")
        {
            script->m_listPassiveObject.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x1ea8 循环头赋值
            {
                passiveObj.m_0 = okInt;                     // 函数头复用对象
                passiveObj.m_4 = ScanInt(static_cast<bool*>(0));
                passiveObj.m_8 = ScanInt(static_cast<bool*>(0));
                passiveObj.m_c = ScanInt(static_cast<bool*>(0));
                script->m_listPassiveObject.push_back(passiveObj);
            }
            continue;
        }
        if (tok == "[monster]" || tok == "[monster condition]")  // code_r0x089deeb7
        {
            std::vector<STAssignMonster>* target = &script->m_vecMonster;
            if (tok == "[monster]")
                script->m_vecMonster.clear();               // vector(+0x34)
            else
            {
                script->m_vecConditionMonster.clear();      // vector(+0x40)
                target = &script->m_vecConditionMonster;
            }
            while (okInt = ScanInt(&ok), ok == true)        // 0x1fd4 循环头
            {
                monObj.Clear();                             // 函数头复用对象
                monObj.m_0 = okInt;
                // [STMapScript sweep] ORIG 0x089defc1 段 `cmp $0x1770,%eax; jbe` ⇒ 计数为
                // 无符号（P196/T2 直证）；int 生 jle 语义分叉（负数计数值域）。
                static unsigned int named_count = 0;        // 函数级静态校验计数
                if ((okInt > 49999 && okInt < 0xc739) ||
                    (okInt > 55999 && okInt < 0xea61))
                {
                    named_count = named_count + 1;
                    if (named_count > 6000)
                        return false;
                }
                monObj.m_4 = ScanInt(static_cast<bool*>(0)) != 0;
                monObj.m_8 = ScanInt(static_cast<bool*>(0));
                monObj.m_c = ScanInt(static_cast<bool*>(0));
                monObj.m_10 = ScanInt(static_cast<bool*>(0));
                monObj.m_14 = ScanInt(static_cast<bool*>(0));
                monObj.m_pair18.m_pair.m_lo = ScanInt(static_cast<bool*>(0));   // +0x18
                monObj.m_pair18.m_pair.m_hi = ScanInt(static_cast<bool*>(0));   // +0x1c
                ScanStr(&sub);
                if (sub == "[fixed]")                       // uStack_750
                    monObj.m_20 = 0;
                else if (sub == "[half]")
                    monObj.m_20 = 1;
                else if (sub == "[ratio]")
                    monObj.m_20 = 2;
                ScanStr(&sub);
                if (sub == "[NPC]")                         // NPC 携带 idx→+0x58
                {
                    monObj.m_58 = ScanInt(static_cast<bool*>(0));
                    ScanStr(&sub);
                }
                if (sub == "[normal]")                      // uStack_74c
                    monObj.m_24 = 0;
                else if (sub == "[champion]")
                {
                    monObj.m_24 = 1;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0xe8
                    // [STMapScript sweep] ORIG 0x089dfxxx 段为 `test; jle <clear 块>`（Scan 直落）
                    // ⇒ 源码按 if(>0){Scan} else {clear} 书写才复现块序（P47/P222）。
                    if (cnt > 0)
                        ScanChampionFeatures(cnt, monObj);
                    else
                        monObj.m_vec28.clear();
                }
                else if (sub == "[super champion]")
                {
                    monObj.m_24 = 2;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0xe4
                    if (cnt > 0)
                        ScanChampionFeatures(cnt, monObj);
                    else
                        monObj.m_vec28.clear();
                }
                else if (sub == "[boss]")
                {
                    // [STMapScript sweep] ORIG 0x089df24e `cmp $0x5dc,%eax; jbe` ⇒ unsigned。
                    static unsigned int boss_count = 0;
                    boss_count = boss_count + 1;
                    if (boss_count > 0x5dc)
                        return false;
                    monObj.m_24 = 3;
                }
                monObj.m_34 = 0;                            // local_73c=0（push 前复位）
                int total = static_cast<int>(script->m_vecMonster.size());  // esi 先算
                total = total + static_cast<int>(script->m_vecConditionMonster.size());
                (void)total;                                // ORIG 死存储（校验和，未用）
                target->push_back(monObj);
                if (tok == "[monster]")
                    ++normalMonsterCount;                   // iStack_140++
            }
            continue;
        }
        if (tok == "[blood monster]" || tok == "[ultimate monster]")  // code_r0x089df2ff
        {
            int lineIdx = 0;                                // iStack_e4
            script->m_vecBloodSystem.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x23d9 循环头
            {
                bloodSys.Clear();                           // 函数头复用对象
                bloodSys.m_id = okInt;
                bloodSys.m_field04 = ScanInt(static_cast<bool*>(0));
                bloodSys.m_phase = ScanInt(static_cast<bool*>(0));
                bloodSys.m_field0c = ScanInt(static_cast<bool*>(0));
                bloodSys.m_field10 = ScanInt(static_cast<bool*>(0));
                bloodSys.m_timeA = ScanInt(static_cast<bool*>(0));
                bloodSys.m_timeB = ScanInt(static_cast<bool*>(0));
                bloodSys.m_startCnt = ScanInt(static_cast<bool*>(0));
                bloodSys.m_cntInc = ScanInt(static_cast<bool*>(0));
                bloodSys.m_totalCnt = ScanInt(static_cast<bool*>(0));
                if (bloodSys.m_timeA + -500 <= bloodSys.m_totalCnt * bloodSys.m_timeB)
                {
                    printf("\n(%s) [blood monster](%d) GROUP : MATCHING TIME ERROR!!\n",
                           path, lineIdx);
                    return false;
                }
                script->m_vecBloodSystem.push_back(bloodSys);
                lineIdx = lineIdx + 1;
            }
            continue;
        }
        if (tok == "[blood phase time]" || tok == "[ultimate phase time]")  // code_r0x089df4b2
        {
            script->m_vecBloodPhaseTime.clear();
            int e0 = 0;                                     // iStack_e0 行序
            int dc = 0;                                     // iStack_dc 已见 spawn 组数
            int d8 = 0;                                     // iStack_d8 上上 spawnCnt
            int d4 = 0;                                     // iStack_d4 上 spawnCnt（mismatch 基线）
            int d0 = 0;                                     // iStack_d0
            int cc = 0;                                     // iStack_cc 同 round 连击数
            int c8 = 0;                                     // iStack_c8 上 spawnCnt
            int c4 = 0;                                     // iStack_c4 上 round（首值 0）
            int c0 = -1;                                    // iStack_c0 上 order
            int bc = 0;                                     // iStack_bc 上 interval
            bool unusedB6 = false;                          // -0xb2（ORIG dead 槽，P31）
            (void)unusedB6;
            while (okInt = ScanInt(&ok), ok == true)        // 0x25ed 循环头
            {
                bloodPt.Clear();                            // 函数头复用对象
                bloodPt.m_smallRoundOrder = okInt;
                bloodPt.m_phaseIdx = ScanInt(static_cast<bool*>(0));
                bloodPt.m_interval = ScanInt(static_cast<bool*>(0));
                bloodPt.m_time = static_cast<float>(ScanFloat(static_cast<bool*>(0)));
                bloodPt.m_field10 = ScanInt(static_cast<bool*>(0));
                bloodPt.m_spawnCnt = ScanInt(static_cast<bool*>(0));
                if (tok == "[ultimate phase time]")         // ultimate 才读难度名
                {
                    bool diffOk = false;                    // -0xb1（char）
                    std::string diffTok;                    // -0x518（每轮 ctor/dtor）
                    diffOk = ScanStr(&diffTok);
                    if (!diffOk)
                        return false;                       // 0x2630 fail 出口
                    if (diffTok == "A")
                        bloodPt.m_difficulty = 1;
                    else if (diffTok == "B")
                        bloodPt.m_difficulty = 2;
                    else if (diffTok == "C")
                        bloodPt.m_difficulty = 3;
                    else if (diffTok == "D")
                        bloodPt.m_difficulty = 4;
                    else if (diffTok == "E")
                        bloodPt.m_difficulty = 5;
                    else
                        return false;                       // 0x26f1 未知难度 → 失败
                }
                if (bloodPt.m_phaseIdx >=
                    static_cast<int>(script->m_vecBloodSystem.size()))
                {
                    printf("\n(%s) [blood phase time](%d) MONSTER LISTINDEX ERROR!!\n",
                           path, e0);
                        return false;
                }
                if (bloodPt.m_smallRoundOrder != c4 && bloodPt.m_field10 != 0)
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : SPAWN ORDER IS NOT ZERO. ERROR!!\n",
                           path, e0);
                        return false;
                }
                if (bloodPt.m_spawnCnt > 10)
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : TIMER CNT OVER!!\n",
                           path, e0);
                        return false;
                }
                if (tok == "[blood phase time]" && bloodPt.m_smallRoundOrder == c4 &&
                    bloodPt.m_field10 == c0)
                {
                    if (d4 == 0 || bloodPt.m_spawnCnt == 0 || d4 < bloodPt.m_spawnCnt)
                    {
                        printf("\n(%s) [blood phase time](%d) SMALLROUND : ORDER - ORDER_CNT MISMATCHING!!\n",
                               path, e0);
                        return false;
                }
                    if (bloodPt.m_interval < bc && bloodPt.m_field10 == 0)
                    {
                        printf("\n(%s) [blood phase time](%d) SMALLROUND INTERVAL TIME ORDER ERROR:!!\n",
                               path, e0);
                        return false;
                }
                }
                // [STMapScript sweep] ORIG 0x089df8c1 段为 `jl ERR; sub c4-X; test; jle OK`
                // （无 js/je）；`(c4!=X && -1<c4-X)` 与 `c4-X>0` 整数等价（P214/P222 块序）。
                if (bloodPt.m_smallRoundOrder < c4 ||
                    c4 - bloodPt.m_smallRoundOrder > 0)
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : ROUND ERROR!!\n",
                           path, e0);
                        return false;
                }
                if (tok == "[blood phase time]" && bloodPt.m_smallRoundOrder == c4 &&
                    (bloodPt.m_field10 < c0 || c0 - bloodPt.m_field10 > 0))
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : ROUND ORDER ERROR!!\n",
                           path, e0);
                        return false;
                }
                if (e0 == 0 && (bloodPt.m_field10 != 0 || bloodPt.m_smallRoundOrder != 0))
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : FIRST SPAWN CNT || ORDER IS NOT ZERO. ERROR || ROUND IS NOT ZERO. ERROR!!\n",
                           path, 0);
                        return false;
                }
                if (d8 != 0 && c8 < bloodPt.m_spawnCnt)
                {
                    if (dc != d8)
                    {
                        printf("\n(%s) [blood phase time](%d) SMALLROUND : SPAWN CNT ERROR!!\n",
                               path, e0);
                        return false;
                }
                    dc = 0;
                    d8 = 0;
                }
                // [STMapScript sweep] ORIG 0x089df9xx 段为 `test; jle <spawnCnt<1 块>`（else 直落，
                // P272 德摩根块序交换）；`< 1` 形在 4.4 反产 `jg <else>`。
                if (bloodPt.m_spawnCnt >= 1)
                {
                    if (bloodPt.m_spawnCnt != 1)
                        d8 = bloodPt.m_spawnCnt;
                    dc = dc + 1;
                }
                else
                {
                    if (dc != 0)
                    {
                        if (dc != d8)
                        {
                            printf("\n(%s) [blood phase time](%d) SMALLROUND : SPAWN CNT ERROR!!\n",
                                   path, e0);
                        return false;
                }
                        dc = 0;
                        d8 = 0;
                }
                }
                if (bloodPt.m_smallRoundOrder == c4)
                {
                    d0 = bloodPt.m_smallRoundOrder;
                    cc = cc + 1;
                }
                else
                {
                    cc = 0;
                }
                (void)d0; (void)cc; (void)dc;
                c8 = bloodPt.m_spawnCnt;
                c4 = bloodPt.m_smallRoundOrder;
                c0 = bloodPt.m_field10;
                d4 = bloodPt.m_spawnCnt;
                bc = bloodPt.m_interval;
                e0 = e0 + 1;
                script->m_vecBloodPhaseTime.push_back(bloodPt);
                }
            {   // ORIG 收尾校验：最后两条 round 相同且最后一条 spawnCnt!=0 → 失败
                // [STMapScript sweep] P183：ORIG 0x089df541/0x089df55x 每处重发
                // `mov 0x8(%ebp),%edx; add $0x294,%edx`（三次独立 operator[]）；
                // 引用局部会缓存 &vec 到 -0xc8(%ebp) 并省掉两次 add。
                int last = script->m_vecBloodPhaseTime[
                    static_cast<unsigned int>(e0 - 1)].m_smallRoundOrder;
                int prev = script->m_vecBloodPhaseTime[
                    static_cast<unsigned int>(e0 - 2)].m_smallRoundOrder;
                if (last == prev &&
                    script->m_vecBloodPhaseTime[
                        static_cast<unsigned int>(e0 - 1)].m_spawnCnt != 0)
                {
                    printf("\n(%s) [blood phase time](%d) SMALLROUND : LAST SMALL ROUND ERROR!!\n",
                           path, e0);
                        return false;
                }
            }
            continue;
        }
        if (tok == "[time line]")                           // call 0x0889a930
        {
            // ORIG 只压 1 个入参（&m_timeLine）：按调用点原样，this 与引用同值。
            reinterpret_cast<advancealtar::StageTimeLineParameter*>(
                &script->m_timeLine)->importPhase(script->m_timeLine);
            continue;
        }
        if (tok == "[defense monster]")
        {
            int lineIdx = 0;                                // iStack_b4
            script->m_vecDefenseSystem.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x2cf7 循环头
            {
                defSys.Clear();                             // 函数头复用对象
                defSys.m_id = okInt;
                defSys.m_field04 = ScanInt(static_cast<bool*>(0));
                defSys.m_phase = ScanInt(static_cast<bool*>(0));
                defSys.m_field0c = ScanInt(static_cast<bool*>(0));
                defSys.m_field10 = ScanInt(static_cast<bool*>(0));
                defSys.m_timeA = ScanInt(static_cast<bool*>(0));
                defSys.m_timeB = ScanInt(static_cast<bool*>(0));
                defSys.m_startCnt = ScanInt(static_cast<bool*>(0));
                defSys.m_cntInc = ScanInt(static_cast<bool*>(0));
                defSys.m_totalCnt = ScanInt(static_cast<bool*>(0));
                if (defSys.m_timeA + -500 <= defSys.m_totalCnt * defSys.m_timeB)
                {
                    printf("\n(%s) [Defense monster](%d) GROUP : MATCHING TIME ERROR!!\n",
                           path, lineIdx);
                    return false;
                }
                script->m_vecDefenseSystem.push_back(defSys);
                lineIdx = lineIdx + 1;
            }
            continue;
        }
        if (tok == "[defense phase time]")
        {
            script->m_vecDefensePhaseTime.clear();
            int b0 = 0;                                     // iStack_b0 行序
            int cAc = 0;                                    // iStack_ac（dead，P31）
            int cA8 = 0;                                    // iStack_a8（dead）
            int a4 = 0;                                     // iStack_a4 上 spawnCnt
            int a0 = 0;                                     // iStack_a0
            int c98 = 0;                                    // iStack_98 上 spawnCnt
            int c9c = 0;                                    // iStack_9c 同 round 连击数
            int c94 = 0;                                    // iStack_94 上 round
            int c90 = -1;                                   // iStack_90 上 order
            int c8c = 0;                                    // iStack_8c 上 interval
            bool unused85 = false;                          // iStack_85（dead bool，P31）
            (void)cAc; (void)cA8; (void)unused85;
            while (okInt = ScanInt(&ok), ok == true)        // 0x2ee1 循环头
            {
                defPt.Clear();                              // 函数头复用对象
                defPt.m_smallRoundOrder = okInt;
                defPt.m_phaseIdx = ScanInt(static_cast<bool*>(0));
                defPt.m_interval = ScanInt(static_cast<bool*>(0));
                defPt.m_time = static_cast<float>(ScanFloat(static_cast<bool*>(0)));
                defPt.m_field10 = ScanInt(static_cast<bool*>(0));
                defPt.m_spawnCnt = ScanInt(static_cast<bool*>(0));
                if (defPt.m_phaseIdx >=
                    static_cast<int>(script->m_vecDefenseSystem.size()))
                {
                    printf("\n(%s) [Defense phase time](%d) MONSTER LISTINDEX ERROR!!\n",
                           path, b0);
                    return false;
                }
                if (defPt.m_smallRoundOrder != c94 && defPt.m_field10 != 0)
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : SPAWN ORDER IS NOT ZERO. ERROR!!\n",
                           path, b0);
                    return false;
                }
                if (defPt.m_spawnCnt > 10)
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : TIMER CNT OVER!!\n",
                           path, b0);
                    return false;
                }
                // ORIG 此处以大写 D 的 "[Defense phase time]" 与 token 比较（实际 token 为
                // 小写，判定恒 false）——按 ORIG 字面照录。
                if (tok == "[Defense phase time]" && defPt.m_smallRoundOrder == c94 &&
                    defPt.m_field10 == c90)
                {
                    if (a4 == 0 || defPt.m_spawnCnt == 0 || a4 < defPt.m_spawnCnt)
                    {
                        printf("\n(%s) [Defense phase time](%d) SMALLROUND : ORDER - ORDER_CNT MISMATCHING!!\n",
                               path, b0);
                        return false;
                    }
                    if (defPt.m_interval < c8c && defPt.m_field10 == 0)
                    {
                        printf("\n(%s) [Defense phase time](%d) SMALLROUND INTERVAL TIME ORDER ERROR:!!\n",
                               path, b0);
                        return false;
                    }
                }
                // [STMapScript sweep] 同 blood 段：ORIG 0x089e005d 为 `jl; sub; test; jle`。
                if (defPt.m_smallRoundOrder < c94 ||
                    c94 - defPt.m_smallRoundOrder > 0)
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : ROUND ERROR!!\n",
                           path, b0);
                    return false;
                }
                if (tok == "[Defense phase time]" && defPt.m_smallRoundOrder == c94 &&
                    (defPt.m_field10 < c90 || c90 - defPt.m_field10 > 0))
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : ROUND ORDER ERROR!!\n",
                           path, b0);
                    return false;
                }
                if (b0 == 0 && (defPt.m_field10 != 0 || defPt.m_smallRoundOrder != 0))
                {
                    printf("\n(%s) [Defense phase time](%d) SMALLROUND : FIRST SPAWN CNT || ORDER IS NOT ZERO. ERROR || ROUND IS NOT ZERO. ERROR!!\n",
                           path, 0);
                    return false;
                }
                if (defPt.m_smallRoundOrder == c94)
                {
                    a0 = defPt.m_smallRoundOrder;
                    c98 = c98 + 1;
                }
                else
                {
                    c98 = 0;
                }
                (void)a0; (void)c98;
                c94 = defPt.m_smallRoundOrder;
                c90 = defPt.m_field10;
                c8c = defPt.m_interval;
                a4 = defPt.m_spawnCnt;
                b0 = b0 + 1;
                script->m_vecDefensePhaseTime.push_back(defPt);
            }
            continue;
        }
        if (tok == "[summon start area]")
        {
            script->m_summonStartArea.m_x = ScanInt(static_cast<bool*>(0));
            script->m_summonStartArea.m_y = ScanInt(static_cast<bool*>(0));
            script->m_summonStartArea.m_w = ScanInt(static_cast<bool*>(0));
            script->m_summonStartArea.m_h = ScanInt(static_cast<bool*>(0));
            continue;
        }
        if (tok == "[center index]")                         // 0x089e042c
        {
            okInt = ScanInt(&ok);
            if (!ok)
                break;                                      // 0x3358 → 主循环出口（收尾）
            script->m_178 = okInt;                          // +0x178
            continue;
        }
        if (tok == "[/center index]")                        // 0x089e0457
            continue;
        if (tok == "[camera edge exception]")                // 0x089e0463
        {
            okInt = ScanInt(&ok);
            if (!ok)
                break;                                      // 0x33ca → 主循环出口
            script->m_17c = okInt;                          // +0x17c
            continue;
        }
        if (tok == "[/camera edge exception]")               // 0x089e0492
            continue;
        if (tok == "[camera edge recovery object]")         // 0x089e036c（0x341f 位序）
        {
            int x = -1;                                     // -0x51c
            int y = 0;                                      // -0x520
            while (true)                                    // 0x3434 两级 && 短路口
            {
                okInt = ScanInt(&ok);
                if (ok != true)
                    break;
                x = okInt;
                okInt = ScanInt(&ok);
                if (ok != true)
                    break;
                y = okInt;
                // P178/P135：ORIG push_back(pair&&)（MISSING EOS1_）；嵌套转换临时形
                script->m_vec180.push_back(std::pair<int, int>(std::make_pair(x, y)));
            }
            continue;
        }
        if (tok == "[heroes mode map index]")               // 0x089e0435
        {
            script->m_maxMonsterDropException = ScanInt(&ok);   // +0x284 直存
            if (!ok)
                break;                                      // 0x34fb → 主循环出口
            continue;
        }
        if (tok == "[monster team]")                        // 0x089e049f 区
        {
            for (int i = 0; i < normalMonsterCount; ++i)
                script->m_vecMonster[i].m_4c = ScanInt(static_cast<bool*>(0));
            ScanStr(&sub);
            continue;
        }
        if (tok == "[monster specific AI]")                 // 0x089e050f 区
        {
            for (int i = 0; i < normalMonsterCount; ++i)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[normal]")
                    script->m_vecMonster[i].m_34 = 0;
                else if (sub == "[zone defence]")
                {
                    script->m_vecMonster[i].m_34 = 1;
                    script->m_vecMonster[i].m_pair38.m_arr[0] = ScanInt(static_cast<bool*>(0));
                    script->m_vecMonster[i].m_pair38.m_arr[1] = ScanInt(static_cast<bool*>(0));
                }
                else if (sub == "[roaming]")
                {
                    script->m_vecMonster[i].m_34 = 2;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0x78
                    for (int k = 0; k < cnt; ++k)
                    {
                        std::pair<int, int> way;                // -0x528 默认 ctor
                        way.first = ScanInt(static_cast<bool*>(0));
                        way.second = ScanInt(static_cast<bool*>(0));
                        script->m_vecMonster[i].m_vec40.push_back(way);
                    }
                }
            }
            ScanStr(&sub);
            continue;
        }
        if (tok == "[monster collision group]")             // 0x089e0660 区
        {
            for (std::vector<STAssignMonster>::iterator it = script->m_vecMonster.begin();
                 it != script->m_vecMonster.end(); it++)
            {
                it->m_vec5c.clear();
                int cnt = ScanInt(&ok);
                for (int k = 0; k < cnt; ++k)
                {
                    unsigned char v = static_cast<unsigned char>(ScanInt(&ok));  // local_531
                    it->m_vec5c.push_back(v);
                }
            }
            ScanType(tok, true);                            // 收尾 consume 标记
            continue;
        }
        if (tok == "[apc collision group]")                 // 0x089e07f0 区
        {
            for (std::vector<STAICharacterMapArrangeData>::iterator it =
                     script->m_vecAICharacter.begin();
                 it != script->m_vecAICharacter.end(); it++)
            {
                it->m_collisionGroup.clear();
                int cnt = ScanInt(&ok);
                for (int k = 0; k < cnt; ++k)
                {
                    unsigned char v = static_cast<unsigned char>(ScanInt(&ok));  // local_539
                    it->m_collisionGroup.push_back(v);
                }
            }
            ScanType(tok, true);
            continue;
        }
        if (tok == "[champion]")                            // 0x089e097b
        {
            script->m_championRate = ScanInt(static_cast<bool*>(0));  // +0x4c
            continue;
        }
        if (tok == "[special passive object]")              // 0x089e099c 区
        {
            std::list<STAssignItem> assignItems(script->m_listItem);  // local_544 copy
            script->m_listSpecialPassive.clear();
            int group = 0;                                  // iStack_64
            while (okInt = ScanInt(&ok), ok == true)        // 0x3a7d 循环头
            {
                spoObj.Clear();                             // 函数头复用对象
                spoObj.m_0 = okInt;
                spoObj.m_4 = ScanInt(static_cast<bool*>(0));
                spoObj.m_8 = ScanInt(static_cast<bool*>(0));
                spoObj.m_c = ScanInt(static_cast<bool*>(0));
                int actionCount = ScanInt(static_cast<bool*>(0));   // local_60
                bool hasItem = false;                       // local_59
                for (int k = 0; k < actionCount; ++k)       // local_58
                {
                    bool actionError = false;               // local_51（ORIG 恒 false）
                    SPOBJ_ACTION_DATAS action;              // ctor memset 0xff
                    procSpecialPassiveObject(action, spoObj,
                                             g_stMapScriptFileList.FindIndexByFullScan(path));
                    if (action.getActionType() == 2)
                        hasItem = true;
                    if (action.getActionType() != 5)
                        spoObj.m_vec10.push_back(action);
                    if (actionError)
                        return false;
                }
                if (hasItem)
                {
                    spoObj.m_1c = group;                    // local_664（组序）
                    group = group + 1;
                }
                script->m_listSpecialPassive.push_back(spoObj);
            }
            continue;
        }
        if (tok == "[NPC]")                                 // 0x089e0ba1
        {
            script->m_vecNpc.clear();
            while (okInt = ScanInt(&ok), ok == true)        // 0x3c6c 循环头
            {
                bool wasStr;                                // -0x559（ScanStrOrInt 首参，无独立初始化）
                wasStr = false;                             // 0x3c96：movb $0,-0x559
                npcObj.m_npcIdx = okInt;                    // 函数头复用对象（npcObj+0）
                okInt = ScanStrOrInt(wasStr, tok3, ok, true);
                if (ok != true)
                    break;
                if (wasStr != true)                         // 第二 token 为 int
                {
                    npcObj.m_field04 = okInt;               // -0x478（npcObj+4）
                    ScanStr(&tok3);
                }
                if (tok3 == "[visible on dungeon clear]")
                {
                    npcObj.m_visible = true;                // uStack_478（ORIG 不复零）
                    ScanStr(&tok3);
                }
                if (tok3 == "[left]")
                    npcObj.m_dir = 0;                       // uStack_474=0
                else if (tok3 == "[right]")
                    npcObj.m_dir = 1;
                npcObj.m_x = ScanInt(static_cast<bool*>(0));
                npcObj.m_y = ScanInt(static_cast<bool*>(0));
                npcObj.m_z = ScanInt(static_cast<bool*>(0));
                script->m_vecNpc.push_back(npcObj);
            }
            continue;
        }
        if (tok == "[monster spawn pos]")                   // 0x089e0d43
        {
            script->m_vecMonsterSpawnPos.clear();
            int count = ScanInt(static_cast<bool*>(0));     // -0x4c
            STRivalMapData proto;                           // -0x568 ctor（循环外一次）
            script->m_vecMonsterSpawnPos.reserve(count * 3);  // 0x3e29 imul 形态
            for (int i = 0; i < count; ++i)
            {
                proto.m_field00 = ScanInt(static_cast<bool*>(0));
                proto.m_field04 = ScanInt(static_cast<bool*>(0));
                proto.m_field08 = ScanInt(static_cast<bool*>(0));
                script->m_vecMonsterSpawnPos.push_back(proto);
            }
            continue;
        }
        if (tok == "[ai character]")                        // 0x089e0e00 区
        {
            int type = -1;                                  // -0x44
            while (type = ScanInt(&ok), ok == true)         // 0x3edd 循环头
            {
                STAICharacterMapArrangeData ai;             // -0x704 ctor（含 vector 成员）
                ai.m_type = type;
                ai.m_field04 = ScanInt(static_cast<bool*>(0));
                ai.m_field08 = ScanInt(static_cast<bool*>(0));
                ai.m_field0c = ScanInt(static_cast<bool*>(0));
                {                                           // 阵营：character/monster/neutral
                    ScanTypeOrStr(ok, tok, true);            // local_430 被覆盖（ORIG 原样）
                    if (tok == "[character]")
                        ai.m_camp = 0;
                    else if (tok == "[monster]")
                        ai.m_camp = 100;
                    else if (tok == "[neutral]")
                        ai.m_camp = 200;
                }
                char hasTok = ScanStr(&sub);                // -0x3d（char 直存 al）
                if (sub == "[NPC]")                         // 0x3ff8 仅比对 token，不查 hasTok
                {
                    ai.m_npcIdx = ScanInt(static_cast<bool*>(0));
                    hasTok = ScanStr(&sub);
                }
                if (hasTok != 1)
                {
                    sub = "[normal]";                       // Ss::operator=(PKc)
                }
                if (sub == "[normal]")
                    ai.m_roleType = 5;
                else if (sub == "[champion]")               // uStack_6f4
                {
                    ai.m_roleType = 6;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0x3c
                    // [STMapScript sweep] 同 monster 段：ORIG 为 `test; jle <clear>`（Scan 直落）。
                    if (cnt > 0)
                        ScanChampionFeatures(cnt, ai);
                    else
                        ai.m_championFeatures.clear();
                }
                else if (sub == "[super champion]")
                {
                    ai.m_roleType = 7;
                    int cnt = ScanInt(static_cast<bool*>(0));   // -0x38
                    if (cnt > 0)
                        ScanChampionFeatures(cnt, ai);
                    else
                        ai.m_championFeatures.clear();
                }
                else if (sub == "[boss]")
                {
                    // [STMapScript sweep] ORIG 0x089e10d3 `cmp $0x5dc,%eax; jbe` ⇒ unsigned。
                    static unsigned int apc_boss_count = 0;
                    // 两句式：ORIG 是 `count = count + 1;` 后重新读槽比较（0x089e10c0 段）；
                    // `++count >= K` 前缀形在 4.4 生 seta 物化（P225 族），偏离 jbe 直跳。
                    apc_boss_count = apc_boss_count + 1;
                    if (apc_boss_count > 0x5dc)
                        return false;
                    ai.m_roleType = 8;
                }
                    else
                    return false;                           // 未知 role → 失败
                ai.m_posX = ScanInt(&ok);                   // -0x6f0（char 直存 al）
                if (!ok)
                    return false;
                ai.m_posY = ScanInt(&ok);                   // -0x6eb
                if (!ok)
                    return false;
                script->m_vecAICharacter.push_back(ai);
            }
            continue;
        }
        if (tok == "[apc random point]")                    // 0x089e11c8
        {
            script->m_apcRandomPoint = ScanInt(static_cast<bool*>(0));  // +0x268
            continue;
        }
        if (tok == "[event monster position]")              // 0x089e11ec
        {
            STRivalMapData proto;                           // -0x574 ctor 循环外一次
            while (true)
            {
                // [b53FIX · 首 token 失败 = **正常列表结束**，不是 import 失败]
                // ORIG 089e1216..089e1225：第 1 个 ScanInt(&ok) 失败棱形为
                //   `movzbl ok; xor $0x1; test %al,%al; je <第二读>; nop; jmp 0x89dd0ab`，
                // 而 0x89dd0ab 正是**主循环头**（`movl $0x1,0x4(%esp); lea -0x42c(%ebp);
                // call ScanType`）⇒ 等价于本块的 `continue;`（跳到下一 tag）。
                // 只有第 2/3 读失败才走 return-false 棱形
                // （089e1251 / 089e127d：`mov $0x0,%ebx; jmp 0x89e3b6c`）。
                // 本方旧形把首读也写成 `return false`（注释亦误记「首 token 失败也 false」），
                // 实机 A/B 实证（Lorien/s0.map，Frida /data/ab_ms_ours.js）：
                //   `[event monster position]` 正常读完 4 组 (682,288,0)/(597,201,0)/
                //   (502,250,0)/(763,270,0) 后，第 5 轮首读撞上下一 TYPE tag
                //   （tok#216）⇒ !ok ⇒ ret=0 ⇒ `Loading Maps Fail`。
                proto.m_field00 = ScanInt(&ok);
                if (!ok)
                    break;                                  // ORIG 089e1225 → 0x89dd0ab
                proto.m_field04 = ScanInt(&ok);
                if (!ok)
                    return false;
                proto.m_field08 = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_vecEventMonsterPos.push_back(proto);
            }
            continue;
        }
        if (tok == "[map loading image path]")              // 0x089e12cc
        {
            ScanStr(&script->m_mapLoadingImagePath);        // param_1+0x9b
            continue;
        }
        if (tok == "[quest animation]")
        {
            script->m_listQuestAnimation.clear();
            STQuestAnimation qani;                          // 段内一次 ctor，跨轮复用
            while (ScanStr(&qani.m_name))
            {
                ScanStr(&sub);
                qani.m_layer = getDrawLayer(sub.c_str());
                qani.m_field08 = ScanInt(static_cast<bool*>(0));
                qani.m_field0c = ScanInt(static_cast<bool*>(0));
                qani.m_field10 = ScanInt(static_cast<bool*>(0));
                qani.m_field14 = ScanInt(static_cast<bool*>(0));
                qani.m_flag18 = ScanInt(static_cast<bool*>(0)) != 0;
                script->m_listQuestAnimation.push_back(qani);
            }
            continue;
        }
        if (tok == "[attacked monster info]")
        {
            script->m_vecAttackedMonster.clear();
            int acc = 0;                                    // -0x34 累加权重
            STAttackedMonster mon;                          // -0x5ac（POD，无 ctor）
            while (mon.m_id = ScanInt(&ok), ok == true)     // 0x44da 循环头直存字段
            {
                mon.m_field04 = ScanInt(&ok);
                if (!ok)
                    return false;
                mon.m_field08 = ScanInt(&ok);
                if (!ok)
                    return false;
                mon.m_field0c = ScanInt(&ok);
                if (!ok)
                    return false;
                mon.m_attackWeight = ScanInt(&ok);          // iStack_5a0（先存原始值）
                if (!ok)
                    return false;
                mon.m_attackWeight = acc + mon.m_attackWeight;   // local_38 += w
                acc = mon.m_attackWeight;
                mon.m_field14 = ScanInt(&ok);
                if (!ok)
                    return false;
                mon.m_field18 = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_vecAttackedMonster.push_back(mon);
            }
            continue;
        }
        if (tok == "[max monster drop exception]")          // 0x089e17xx 区
        {
            script->m_maxMonsterCountException = ScanInt(&ok);  // param_1[0xc9]
            if (!ok)
                return false;                               // 0x4679 fail→false
            continue;
        }
        if (tok == "[max monster count exception]")
        {
            script->m_328 = ScanInt(&ok);                   // +0x328
            if (!ok)
                return false;                               // 0x46cc fail→false
            continue;
        }
        if (tok == "[background effect]")                   // 0x089e1640 区
        {
            script->m_vecBackGroundEffect.clear();
            BackGroundEffect effect;                        // -0x704 段外一次 ctor，跨轮复用
            while (true)
            {
                effect.clear();
                if (!ScanStr(&sub))
                    break;
                if (sub == "[swing]")                       // type=0 分支
                {
                    if (!ScanStr(&sub))
                        break;                              // 0x476a
                    effect.m_layer = getDrawLayer(sub.c_str());
                    effect.m_type = 0;                      // -0x704（layer 后写）
                    effect.m_field08 = ScanInt(static_cast<bool*>(0));
                    effect.m_field0c = ScanInt(static_cast<bool*>(0));
                    effect.m_field10 = ScanInt(static_cast<bool*>(0));
                    effect.m_field14 = ScanInt(static_cast<bool*>(0));
                    effect.m_field18 = ScanInt(static_cast<bool*>(0));
                    {                                       // 可选动画名 → +0x1c string
                        std::string anim;                   // -0x5b4（每轮 ctor/dtor）
                        anim.clear();                       // Ss::clear
                        ScanStr(&anim);
                        if (anim == "")
                            effect.m_animName = "";         // Ss::operator=(PKc)
                        else
                            effect.m_animName = anim;       // Ss::operator=(RKSs)
                    }
                }
                else if (sub == "[dust]")                   // type=1 分支
                {
                    effect.m_type = 1;
                    if (!ScanStr(&sub))
                        break;                              // 0x48d1
                    effect.m_layer = getDrawLayer(sub.c_str());
                    effect.m_field0c = ScanInt(static_cast<bool*>(0));
                    effect.m_field10 = ScanInt(static_cast<bool*>(0));
                    effect.m_field14 = ScanInt(static_cast<bool*>(0));
                }
                script->m_vecBackGroundEffect.push_back(effect);
            }
            continue;
        }
        if (tok == "[block effect]")                        // 0x089e18fc 区
        {
            script->m_vecBlockEffect.clear();
            BackGroundEffectBlock block;                    // -0x5c0 段外一次 ctor
            while (true)
            {
                block.clear();
                if (!ScanStr(&sub))
                    break;
                if (sub == "[swing]")
                    block.m_type = 0;
                else if (sub == "[dust]")
                    block.m_type = 1;                       // ORIG 字面判定
                if (!ScanStr(&sub))
                    break;
                if (sub == "[object]")                      // uStack_5c0=0
                {
                    block.m_mode = 0;
                    block.m_val = ScanInt(static_cast<bool*>(0));
                }
                else if (sub == "[animation]")              // uStack_5c0=1
                {
                    block.m_mode = 1;
                    ScanStr(&sub);
                    block.m_animName = sub;                 // Ss::operator=(RKSs)
                }
                script->m_vecBlockEffect.push_back(block);
            }
            continue;
        }
        if (tok == "[absolute start path]")                 // 0x089e19xx 区
        {
            script->m_startPathDir = 4;
            for (;;)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[left]")        script->m_startPathDir = 0;
                else if (sub == "[right]")  script->m_startPathDir = 1;
                else if (sub == "[up]")     script->m_startPathDir = 2;
                else if (sub == "[down]")   script->m_startPathDir = 3;
            }
            continue;
        }
        if (tok == "[block path]")
        {
            for (int i = 0; i < 4; ++i)
            {
                if (!ScanStr(&sub))
                    break;
                script->m_blockPath[i] =
                    static_cast<char>(sub == "[close]");
            }
            continue;
        }
        if (tok == "[tile option]")
        {
            for (;;)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[upper]")
                    script->m_tileOptionUpper = 1;          // byte(param_1+0x36)
            }
            continue;
        }
        if (tok == "[dialog]")                              // 0x089e1c73 区
        {
            STDialog dlg;                                   // -0x5c8 ctor
            char scanOk;                                    // -0x2b（char 物化）
            scanOk = ScanStr(&dlg.m_name);
            if (scanOk != 1)
                return false;                               // 0x4d2b → dlg dtor false
            {
                std::string when;                           // -0x5cc（每入构造）
                scanOk = ScanStr(&when);
                if (scanOk != 1)
                    return false;                           // 0x4d62
                if (when == "on start")
                {
                    dlg.m_flag[0] = 1;                      // -0x5c4（dlg+4）
                    script->m_vecDialog.push_back(dlg);     // vector(+0xc4)
                }
                else if (when == "on complete")
                {
                    dlg.m_flag[1] = 1;                      // -0x5c3（dlg+5）
                    script->m_vecDialog.push_back(dlg);
                }
                else
                {
                    LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                          0x84f,
                                          "Error on scanning tower dialog. There is no match string %s",
                                          when.c_str());   // 未知串仅记日志，继续
                }
            }
            continue;
        }
        if (tok == "[map dialog]")                          // 0x089e1c61
        {
            ScanStr(&script->m_str34c);                     // 0x4ec1：直存 +0x34c
            continue;
        }
        if (tok == "[tournament monster]")                  // 0x089e1e53 区
        {
            std::vector<TournamentMonster> mons;            // -0x5d8（先 ctor）
            char teamCount = 0;                             // -0x5d9
            teamCount = ScanInt(&ok);                       // mov %al 截断直存
            if (teamCount < 1 || teamCount > 2)             // signed char 双判定
                return false;
            if (!ok)
                return false;                               // 0x4f4d → mons dtor false
            while (true)
            {
                TournamentMonster mon;                      // -0x5e4 每轮 ctor
                mon.m_mobId = ScanInt(&ok);
                if (!ok)
                    break;
                mon.m_rate = ScanInt(&ok);
                if (!ok)
                    return false;
                mons.push_back(mon);
            }
            if (script->m_tournamentMonsters.insert(
                    std::make_pair(teamCount, mons)).second != true)   // make_pair<char&,vector&>
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0x878,
                                      "[tournament monster] map index(%d) is duplicated",
                                      static_cast<int>(teamCount));
                return false;
            }
            continue;
        }
        if (tok == "[tournament start area]")               // 0x089e1f00 区
        {
            TournamentStartPos pos;                         // -0x5f0（先 ctor）
            char t = 0;                                     // -0x2a
            t = ScanInt(&ok);                               // mov %al 直存
            if (t < 1 || t > 2)
                return false;
            if (!ok)
                return false;
            if (t == 1)
            {
                pos.m_x = ScanInt(&ok);
                if (!ok)
                    return false;
                pos.m_y = ScanInt(&ok);
                if (!ok)
                    return false;
                char slot = 0;                              // -0x29
                slot = ScanInt(&ok);
                if (slot < 1 || slot > 2)
                    return false;
                pos.m_slot = slot;
                if (slot == 1)
                    script->m_tournamentStartPos1.push_back(pos);   // vector@0x2d0
                else if (slot == 2)
                    script->m_tournamentStartPos2.push_back(pos);   // vector@0x2dc
            }
            continue;
        }
        if (tok == "[move layered map]")                    // 0x089e2216 区（__lookup 表段）
        {
            std::string line;                               // local_5f8（循环外构造一次）
            bool loop;                                      // local_5f5（ORIG 无初值存储）
            do
            {
                loop = ScanType(line, true);
                if (!loop)
                    return false;
                // [b53FIX · 惰性建表轮的回跳点必须是「find 循环头」而不是 do 体首]
                // ORIG 089e25ff/089e2614：switch 之后收尾为
                //   `movzbl __inited; xor $1; test; jne <do 条件>` +
                //   `leave SpinLock; movb $1,__inited; jmp 0x89e223e`。
                // 0x89e223e 正是**本块静态对象守卫段入口**
                // （089e223e `movzbl 0x94fe3f8` = map ctor guard → 089e22ba SpinLock
                //  guard 0x94fe400 → 089e2336 `movzbl 0x94fe5c8`(__inited) 的
                //  `for(;;)` find 头）⇒ 建表轮结束后**不重新 ScanType**，直接以同一个
                // `line` 重跑 find/switch（此时 __inited 已置 1，find 命中真实 id）。
                // 本方旧形把 `layered_next:` 放在 do 体尾（`while(loop)` 之前）
                // ⇒ 建表轮结束经 `while(loop)` 回到 do 体首，**多读一个 token**。
                // 实机 A/B 实证（cartelcommand/15282(0.0)start.map，/data/ab_ly_ours.js
                // 钩内层分派点 0x8625a37 读 id/[ebp-0x609]/__inited）：
                //   tok#97 `[move layered map]`（主循环 +0x16d）
                //   → tok#98 `[zpos under]`（+0x537e）→ LAYDISP id=0x0 loop=1 inited=0（建表轮）
                //   → 旧回边 → ScanType 消费 tok#99（数字，非 TYPE 不报 getType）
                //   → ScanType 内 `ok` 一旦置 false 便不再复位（ORIG 同形，089bc8ae
                //      `movb $0x0,-0x9(%ebp)`；089bc8b2 起只判 tok==5/0）
                //   → tok#100 `[/move layered map]` 虽为 TYPE 仍返回 false
                //   ⇒ ImportMapScript ret=0 ⇒ `Loading Maps Fail`（#1564，前 1563 份全过）。
                layered_next:
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    int id;
                    for (;;)
                    {
                        if (!__inited)
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            id = 0;
                            break;
                        }
                        std::map<const char*, int, less_TCHAR>::const_iterator it =
                            __lookup.find(line.c_str());
                        if (it != __lookup.end())
                            id = it->second;
                        break;
                    }
                    switch (id)
                    {
                    case 0:
                    case 0x8b8:                             // [zpos under]
                    {
                        if (__inited)
                        {
                            script->m_moveLayeredType = 1;  // movl $1,0x350
                            script->m_zposUnder = ScanInt(&loop);  // -0x354 ← &-0x5f5
                            if (loop != true)
                                return false;
                            // ORIG 089e24e4/089e24f0：ok 成功棱形 `je 0x89e25ef` →
                            // `nop; jmp 0x89e25f3` = switch **之后**的公共收尾
                            // （`if(!__inited){…}` + `while(loop)`），**不是** find 头。
                            // 旧形 `goto layered_next` 会把同一个 `[zpos under]` 再分派一次，
                            // 追加一次 ScanInt ⇒ 吞掉下一个 token（A/B 实证 tok#100）。
                            break;
                        }
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[zpos under]", 0x8b8));
                        /* fall through */
                    }
                    case 0x8be:                             // [all dead case passable]
                    {
                        if (__inited)
                        {
                            script->m_allDeadCasePassable = 1;  // movb $1,0x358
                            if (loop != true)
                                return false;
                            // ORIG 089e2578/089e2584：成功棱形 `je 0x89e25f2` →
                            // `nop` → 0x89e25f3 公共收尾（同上一分支，非 find 头）。
                            break;
                        }
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[all dead case passable]", 0x8be));
                        /* fall through */
                    }
                    case 0x8c3:                             // [/move layered map]
                    {
                        if (__inited)
                            loop = false;
                        else
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/move layered map]", 0x8c3));
                        break;
                    }
                    default:
                        break;
                    }
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        goto layered_next;
                    }
                }
            } while (loop);
            continue;
        }
        if (tok == "[static player start pos]")             // 0x089e26a0 区（__lookup 表段）
        {
            // ORIG：string / StaticStartPos 每轮 do 体内构造（dtor 同轮尾）；
            // ScanInt 全部走 bool* 版（局部 ok 复用 ScanType 的 flag 槽）。
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            int cont = 0;
            do
            {
                std::string line;                           // local_604（每轮 ctor/dtor）
                StaticStartPos pos;                         // local_614（每轮 ctor）
                bool ok2 = ScanType(line, true);            // local_615
                if (!ok2)
                    return false;
                int id;
                while (true)
                {
                for (;;)
                {
                    if (!__inited)
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(line.c_str());
                    if (it != __lookup.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x8dc:                                 // [player 1]
                {
                    if (__inited)
                    {
                        pos.m_playerIdx = 0;                // movl $0,-0x604
                        goto static_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 1]", 0x8dc));
                    /* fall through */
                }
                case 0x8df:                                 // [player 2]
                {
                    if (__inited)
                    {
                        pos.m_playerIdx = 1;
                        goto static_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 2]", 0x8df));
                    /* fall through */
                }
                case 0x8e2:                                 // [player 3]
                {
                    if (__inited)
                    {
                        pos.m_playerIdx = 2;
                        goto static_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 3]", 0x8e2));
                    /* fall through */
                }
                case 0x8e5:                                 // [player 4]
                {
                    if (__inited)
                    {
                        pos.m_playerIdx = 3;
                        goto static_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 4]", 0x8e5));
                    /* fall through */
                }
                case 0x8e8:                                 // [/static player start pos]
                {
                    if (__inited)
                        ok2 = false;                        // movb $0,-0x611
                    else
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/static player start pos]", 0x8e8));
                    break;
                }
                default:
                    break;
                }
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    continue;                               // 重入本轮 find（0x26dd）
                }
            static_after:
                break;
                }
                if (!ok2)
                    break;                                  // iVar6 = 1 → 退出 do
                pos.m_x = ScanInt(&ok2);
                if (!ok2)
                    return false;
                pos.m_y = ScanInt(&ok2);
                if (!ok2)
                    return false;
                pos.m_z = ScanInt(&ok2);
                if (!ok2)
                    return false;
                script->m_vecStaticStartPos.push_back(pos);  // vector(+0x2e8)
                cont = 2;
            } while (cont != 1);
            continue;
        }
        if (tok == "[limit player move area]")              // 0x089e2c8c 区（__lookup 表段）
        {
            // ORIG：每轮 string + MapArea（ctor 后额外 clear()）；未知键 idx 保持 0
            // 且仍读 4 int（id=-1 同路），find==end 才 insert。
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            int cont = 0;
            do
            {
                std::string line;                           // local_620（每轮 ctor/dtor）
                int playerIdx = 0;                          // local_624
                MapArea area;                               // local_640：ctor + clear
                area.clear();
                bool ok2 = ScanType(line, true);            // local_641
                if (!ok2)
                    return false;
                int id;
                while (true)
                {
                for (;;)
                {
                    if (!__inited)
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(line.c_str());
                    if (it != __lookup.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x911:                                 // [player 1]
                {
                    if (__inited)
                    {
                        playerIdx = 0;                      // movl $0,-0x620
                        goto limit_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 1]", 0x911));
                    /* fall through */
                }
                case 0x914:                                 // [player 2]
                {
                    if (__inited)
                    {
                        playerIdx = 1;
                        goto limit_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 2]", 0x914));
                    /* fall through */
                }
                case 0x917:                                 // [player 3]
                {
                    if (__inited)
                    {
                        playerIdx = 2;
                        goto limit_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 3]", 0x917));
                    /* fall through */
                }
                case 0x91a:                                 // [player 4]
                {
                    if (__inited)
                    {
                        playerIdx = 3;
                        goto limit_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[player 4]", 0x91a));
                    /* fall through */
                }
                case 0x91d:                                 // [/limit player move area]
                {
                    if (__inited)
                        ok2 = false;
                    else
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/limit player move area]", 0x91d));
                    break;
                }
                default:
                    break;
                }
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    continue;                               // 重入本轮 find（0x2ce1）
                }
            limit_after:
                break;
                }
                if (!ok2)
                    break;                                  // iVar6 = 1 → 退出 do
                area.m_x = ScanInt(&ok2);
                if (!ok2)
                    return false;
                area.m_y = ScanInt(&ok2);
                if (!ok2)
                    return false;
                area.m_w = ScanInt(&ok2);
                if (!ok2)
                    return false;
                area.m_h = ScanInt(&ok2);
                if (!ok2)
                    return false;
                std::map<int, MapArea>::iterator it =
                    script->m_limitPlayerMoveAreas.find(playerIdx);
                if (it == script->m_limitPlayerMoveAreas.end())
                    script->m_limitPlayerMoveAreas.insert(
                        std::make_pair(playerIdx, area));   // make_pair + insert
                cont = 2;
            } while (cont != 1);
            continue;
        }
        if (tok == "[customized screen edge]")              // 0x089e3404 区（__lookup 表段）
        {
            // ORIG：结束键以 eq 前置判定；ScanType 返回 false（自然耗尽）→ 整函数失败。
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            std::string line;                               // local_64c（循环外一次）
            bool done = false;
            while (ScanType(line, true))
            {
                if (line == "[/customized screen edge]")
                {
                    done = true;
                    break;
                }
                int id;
                while (true)
                {
                for (;;)
                {
                    if (!__inited)
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        id = 0;
                        break;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(line.c_str());
                    if (it != __lookup.end())
                        id = it->second;
                    break;
                }
                switch (id)
                {
                case 0:
                case 0x946:                                 // [left]
                {
                    if (__inited)
                    {
                        int leftV;                          // -0x650（ORIG 无初始化）
                        if (ScanInt(&leftV) != 1)
                            return false;
                        script->m_screenEdgeLeft = leftV;   // +0x35c
                        goto edge_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[left]", 0x946));
                    /* fall through */
                }
                case 0x94f:                                 // [right]
                {
                    if (__inited)
                    {
                        int rightV;                         // -0x654
                        if (ScanInt(&rightV) != 1)
                            return false;
                        script->m_screenEdgeRight = rightV; // +0x360
                        goto edge_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[right]", 0x94f));
                    /* fall through */
                }
                case 0x958:                                 // [top]
                {
                    if (__inited)
                    {
                        int topV;                           // -0x658
                        if (ScanInt(&topV) != 1)
                            return false;
                        script->m_screenEdgeTop = topV;     // +0x364
                        goto edge_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[top]", 0x958));
                    /* fall through */
                }
                case 0x961:                                 // [bottom]
                {
                    if (__inited)
                    {
                        int bottomV;                        // -0x65c
                        if (ScanInt(&bottomV) != 1)
                            return false;
                        script->m_screenEdgeBottom = bottomV;  // +0x368
                        goto edge_after;
                    }
                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[bottom]", 0x961));
                    break;
                }
                default:
                    break;
                }
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    continue;                               // 重入本轮 find（0x644b）
                }
            edge_after:
                break;
                }
            }
            if (!done)
                return false;
            continue;
        }
        if (tok == "[block use active skill]")              // else 链最外层
        {
            script->m_blockUseActiveSkill = 1;              // byte @0xdb
            continue;
        }
        if (tok == "[block use stackable item]")
        {
            script->m_blockUseStackableItem = 1;            // byte 0x36d
            continue;
        }
        if (tok == "[fix champion]")
        {
            script->m_fixChampion = 1;                      // byte 0x36e
            continue;
        }
        if (tok == "[basic action]")
        {
            if (ScanStr(&script->m_str370) != 1)            // 0x6a72：直存 +0x370
                return false;
            continue;
        }
        if (tok == "[visible minimap]")
        {
            if (ScanInt(&okInt) != 1)
                return false;
            script->m_visibleMinimap = okInt;               // +0x374
            continue;
        }
        if (tok == "[visible town minimap]")
        {
            if (ScanInt(&okInt) != 1)
                return false;
            script->m_visibleTownMinimap = okInt;           // +0x378
            continue;
        }
        // 未识别 token：继续扫描（ORIG 无 default 分支动作）
    }

    // ---- 收尾（code_r0x089e3a93）----
    if (!pvpPracticeRead)
    {
        // words: [0x84..0x87]=[0x76..0x79]、[0x8b..0x8e]=[0x7d..0x80]
        // 即 MapArea[3].xywh = MapArea[1].xywh、MapArea[4].xywh = MapArea[2].xywh
        script->m_pvpStartArea[3].m_x = script->m_pvpStartArea[1].m_x;
        script->m_pvpStartArea[3].m_y = script->m_pvpStartArea[1].m_y;
        script->m_pvpStartArea[3].m_w = script->m_pvpStartArea[1].m_w;
        script->m_pvpStartArea[3].m_h = script->m_pvpStartArea[1].m_h;
        script->m_pvpStartArea[4].m_x = script->m_pvpStartArea[2].m_x;
        script->m_pvpStartArea[4].m_y = script->m_pvpStartArea[2].m_y;
        script->m_pvpStartArea[4].m_w = script->m_pvpStartArea[2].m_w;
        script->m_pvpStartArea[4].m_h = script->m_pvpStartArea[2].m_h;
    }
    script->m_index = g_stMapScriptFileList.FindIndexByFullScan(path);  // *param_1
    return true;
}