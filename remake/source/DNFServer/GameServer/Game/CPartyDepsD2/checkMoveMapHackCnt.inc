// ---------------------------------------------------------------------------
// checkMoveMapHackCnt —— ORIG T 0x085a83a2
// 报告：docs/class_func_reports/CParty/checkMoveMapHackCnt.md
// 反汇编分支边界（+0x57d..+0x5fc）语义：
//   c==0 → 直接到任务豁免段；sec%5==0 → c>sec/5 记疑；sec%5!=0 → c>=sec/5 记疑。
// §6.2 巨件核销：P167 33/33 [0缺0多]；gap 645→629（修前 611）；真修 1 处：
//   任务豁免三段各自独立 call getCurCharacQuestR（修前具名 UserQuest* 缓存
//   合并为 1 call → P167 MISSING 2，P48 反向实证）。
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：N82 全帧
//   槽复用差（0x6c vs 0x8c）、P183 反向 m_mapHitCount 重读 vs -0x50 缓存、
//   kill 的 -0x46 槽缓存 vs ORIG 逐次 movzwl 重读、N-88/setne 物化假象
//   （IsVisitedCurrentMap xor$1 vs setne）、P196 折向（info.m_39 movsbl vs
//   movzbl：槽位值域 0..3 无 ≥0x80 分叉，非真反）——语义与调用序列零缺口。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] gap 13→-1（645 vs 646），p167 33/33 CLEAR。本轮真修：
// ①[真 bug] 技能节奏阈值 q/rem 的基准错 1000 倍——旧码用原始 tick（playTime/5），
//   ORIG 0x085a88e0 读 -0x20 = m_playedTime/1000（0x085a8897 段写入）⇒ 改
//   `secQ = bd(this).m_field8/1000; q = secQ/5; rem = secQ - q*5`；边界比较
//   （rem==0 → c>q 记疑 / rem!=0 → c>=q）经汇编复核与原码一致，保留。
// ②P0/P329 去 lastMap/playTime 具名局部（MapClear 实参内联，ORIG 0x085a8591
//   `mov 0x4(%eax),%esi` 直解引用 + 每次重算 /1000）。③CDungeon::m_89c
//   uchar→bool（SweepCPartyDepsD2-2）后，本函数 0x085a876d 处 `m_89c != 1`
//   由 cmp 形转为 ORIG 的 `movzbl; xor$1; test; je`（指令 645→646）。残余：
//   帧 0x6c vs 0x8c（N82 槽复用差）+ 4.4 continue 中继岛（N-P223）。
//   语义/调用集齐，byte 层封顶。
// [R19a05 ④ 2026-09-13] 出口形态对齐 ORIG（两处"我方多出物化/发明分支"）：
//   ① visited 守卫改 ORIG 形 `if (!bf(this).IsVisitedCurrentMap()) { …body… }`
//      —— ORIG 0x085a8433 `xor $0x1,%eax; test %al,%al; je 0x085a8c80`（bool
//      callee 取反直判）；visited 时直落 0x085a8c80 裸 epilogue、eax=!visited=0
//      ⇒ 返回 0。旧码 `if (IsVisitedCurrentMap()) return true;` 系我方发明（多
//      `test;setne;test;je`+`mov $1,%eax;jmp`，且返回值相反）。
//   ② 函数尾删 `return true;` —— ORIG 0x085a8c7a 直落 0x085a8c80（无 return、
//      无 `mov $1,%eax`）⇒ 返回值 = 遗留 eax（正常路径非 0）。
//   头声明仍为 bool（mangled/ABI 不变，唯一调用点 CParty::move_map 丢弃返回值）。
bool CParty::checkMoveMapHackCnt(MSG_MOVE_MAP& info)
{
    // [b28d-solo-CPartyDepsD2] P171 死读照抄：ORIG 0x085a83ab 先 `mov 0x378` 到
    // -0x38，随即被 `mov 0x1ab8` 覆盖（同一槽）。
    int prevAux = bd(this).m_field4c;                    // -0x38（+0x378 死读）
    prevAux = m_field1ab8;                               // -0x38 覆盖
    m_field1ab8 = 0;
    bd(this).m_field4c = 0;                              // +0x378 dword 清零

    // 地图游玩时长：(u64)now - enterTick(+0x344/0x348) - baseTick(+0x33c/0x340)
    // → 存回 +0x334/+0x338（ORIG 读 getCurTickCount 后零扩 edx,eax 走双 sub/sbb）。
    bd(this).m_playedTime =
        (unsigned long long)GlobalData::s_systemTime_.getCurTickCount()
        - bd(this).m_enterTick - bd(this).m_baseTick;

    // [R19a05 ④] ORIG 0x085a8433 `xor $0x1,%eax; test %al,%al; je 0x085a8c80`：
    //   原码为 `if (!IsVisitedCurrentMap()) { …全 body … }`（bool callee 取反直判形）；
    //   visited 时直落**裸 epilogue** 0x085a8c80（无 `mov $0,…`），eax = !visited = 0
    //   ⇒ 返回 0。旧码 `if (IsVisitedCurrentMap()) return true;` 系我方发明：多出
    //   `test;setne;test;je` + `mov $1,%eax;jmp` 6 条，且 visited 路径返回值 1（ORIG 0）。
    if (!bf(this).IsVisitedCurrentMap())                 // W 0x085bf2e8（bool）
    {
        int sumKill = 0;                                     // -0x34

        if (prevAux != 0)
        {   // 上一张图的第三/二组均值并入历史（idx=当前 m_mapHitCount）
            // [R22a05 ④-form 修复] ORIG 不缓存 m_mapHitCount：0x085a8597/85a8670/
            //   85a8897/85a88d2 四处各自 `mov 0x8(%ebp),%eax; mov 0x1874(%eax),%edx/%ecx`
            //   重读（4 读 + 1 写 m_mapHitCount），旧码 `const int idx` 缓存 -0x44
            //   （1 读 + 3 槽读）。索引基数 0x678 = (m_mapHitList3@+0x19ec − 0xc)/4
            //   由 ORIG `add $0x678,%edx` 直证。
            m_mapHitList3[m_mapHitCount] = m_mapHitList3[m_mapHitCount] / prevAux;
            m_mapHitSum3 += m_mapHitList3[m_mapHitCount];
            m_mapHitList2[m_mapHitCount] = m_mapHitList2[m_mapHitCount] / prevAux;
            m_mapHitSum2 += m_mapHitList2[m_mapHitCount];
        }

        for (int i = 0; i < 4; ++i)
        {
            // [R22a05 ④-form 修复] ORIG 无 kill 具名局部：0x085a8535/85a853f 段每次
            //   `mov -0x28(%ebp),%edx; mov 0xc(%ebp),%eax; add $0x8,%edx;
            //   movzwl 0xb(%eax,%edx,2)` 重读 info.m_1b[i]（`cmp $0xffff,%ax` 无槽）；
            //   旧码具名 kill 令 4.4 物化 `mov %ax,-0x3a; cmpw $0xffff,-0x3a`（+2/处）。
            if (info.m_1b[i] == 0xffff)
                continue;
            if (!checkValidUser(i))                          // W 0x08145868
                continue;

            // 逐张进入型副本只统计座位匹配成员（ORIG 直接调 IsEnterEachMap，无空判）
            if (m_battleField.m_pDungeon->IsEnterEachMap() && (int)info.m_39 != i)
                continue;

            // cUserHistoryLog::MapClear(ushort,int,int) T 0x08684b22
            // 首参 = user+0x79700 日志门面；第 3 参 = *(int*)(lastMap+4)=m_mapIndex
            // [P0/P329] 无 lastMap/playTime 具名局部：ORIG 0x085a8591 直接
            // `mov 0x4(%eax),%esi`（GetLastMapInfo 返回值直解引用），0x085a8597
            // 每次重算 +0x334/1000（-0x4c 除法临时），均无槽往返。
            memberUser(this, i)->m_historyLog.MapClear(
                // [SemA16] 直证（T2）：ORIG 0x085a85ae 段 `mul %edx; mov %edx,%eax;
                // shr $0x6` = 无符号 /1000（m_field8 为 u64 m_playedTime 低 32 位）。
                info.m_1b[i], (int)((unsigned int)bd(this).m_field8 / 1000),
                bf(this).GetLastMapInfo()->m_mapIndex);

            if (info.m_1b[i] == 0)
            {   // 击杀数为 0 却换图 → 0x19b
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x19b, 1, 0, 0);
            }

            // 时长/击杀比异常：quotient=kill/sec，>4 时上报 min(q-4,100) → 0x340
            // [SemA16] 直证（T2）：ORIG 0x085a8687 段 `mov $0x10624dd3,%edx; mul %edx;
            // mov %edx,%eax; shr $0x6` = **无符号** /1000；m_field8 为 int 时 4.4 产
            // `imul + sar $0x6 + sar $0x1f + sub`（有符号）⇒ 负值输入两侧行为不同（真差异）。
            const unsigned int sec = (unsigned int)bd(this).m_field8 / 1000;
            if (sec != 0)
            {
                const unsigned int quotient = (unsigned int)info.m_1b[i] / sec;
                if (4 < quotient)
                {
                    const unsigned int over = quotient - 4;  // -0x3c
                    const unsigned int cap = 100;            // -0x40
                    // [R22a05 ④-form 修复] ORIG 无 min 结果写回：0x085a8a18 `call min` 后
                    //   0x085a8a1b `mov (%eax),%esi` 直进 addServerHackCnt 实参；
                    //   旧码 `over = std::min(over,cap)` 多一次回写 + 重读（mov -0x50）。
                    memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                        memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x340,
                        std::min<unsigned int>(over, cap), 0, 0);
                }
            }

            sumKill += info.m_1b[i];

            // 维度刷图频率检测前置条件（asm +0x3be..+0x40f 组合）
            // [b28d-round1-L24] P0/P183：ORIG 无 bool 槽（widthscan clr_b orig=0 vs
            // 旧码 3）⇒ 条件直接内联进 if，不设 dimTrack 具名局部（clr_b 3→2）。
            if (m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->m_89c != 1 &&
                m_battleField.m_pDungeon->get_dimension_possible() == 0 &&
                prevAux != 0 && m_field1aa0 != 1)
            {   // user+0x8eb48 计数超 9 归零并上报 0xdc
                ++memberUser(this, i)->m_field8eb48;
                if (9 < memberUser(this, i)->m_field8eb48)
                {
                    memberUser(this, i)->m_field8eb48 = 0;
                    memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                        memberUser(this, i), (WongWork::ENUM_HACKTYPE)0xdc, 1, 0, 0);
                }
            }
            m_field1aa0 = 0;

            // [R22a05 ④ 支配域修复] ORIG 0x085a8897..0x085a88b5 在 0x085a888d
            //   `movb $0x0,0x1aa0(%eax)` 之后**无条件**计算 secQ（m_playedTime/1000，
            //   结果落 -0x20 槽），skill 判定段（0x085a890e/85a8894/85a8a20 三处）
            //   只读该槽；旧码把 secQ 下沉进 level>=0x14 && arr54!=0 && f1aa4!=0
            //   三层分支内 ⇒ 求值次数/支配域与 ORIG 分叉（§26.2/§27.4 轴）。
            const unsigned int secQ = (unsigned int)bd(this).m_field8 / 1000;

            // 技能节奏记疑判定（边界语义见函数头注）：
            //   [R4a05 ④ 修复] ORIG 0x085a88da `jle 85a8ace` + 0x085a88f0 `je 85a8ace`：
            //   level<=0x13 或 m_arr54[i]==0 ⇒ **整个豁免/记疑段被跳过**
            //   （0x85a8ace `mov $0x0,%eax` → 0x85a8ad5 `je 85a8b78` 跳过记疑）。
            //   旧码把这两个前置检查短路折叠进 suspectBoundary ⇒ 低等级/未存活成员
            //   仍落入豁免段，未豁免即被误记 hack（0x84）。
            //   进入豁免段 = level>0x13 && arr54!=0 && (c==0 || (rem==0 ? c<=q : c<q))：
            //   c==0 按 ORIG 0x085a8908 `je 85a89a4` 进豁免段（suspectBoundary 保持 false）。
            bool suspectBoundary = false;
            {
                // [R19a05 ③-form] P183/P157：ORIG 不缓存 level/c —— 0x085a88d2
                //   直用 get_charac_level() 返回值（无槽、`cmp $0x13; jle`），
                //   m_field1aa4[i] 在 0x085a88f6 / 0x085a8932 / 0x085a897d /
                //   0x085a8ae1 四处各自 `add $0x6a8; mov 0x4(...)` 重读（4 读 + 1 写）；
                //   旧码两个具名局部是多出对象（我方同址访问仅 3 处）。
                if (memberUser(this, i)->get_charac_level() >= 0x14 &&
                    m_battleData.m_arr54[i] != 0)
                {
                    if (m_field1aa4[i] != 0)
                    {
                    // [真 bug 修正] 基准是 m_playedTime/1000（ORIG 0x085a88e0 读 -0x20，
                    // 由 0x085a8897 段 /1000 写入），不是原始 tick（旧码差 1000 倍）。
                    // [SemA16] 直证（T2）：ORIG 0x085a88ae 段 `mul %edx; mov %edx,%eax;
                    // shr $0x6` = 无符号 /1000（负值时无符号商 ≠ 有符号商，真差异）。
                    // [R22a05 ④ 支配域修复] secQ 已上提至 m_field1aa0=0 之后无条件求值。
                    const unsigned int q = secQ / 5;         // mul 0xcccccccd shr 2
                    const unsigned int rem = secQ - q * 5;   // sec%5
                    // [R19a05 ③-form] ORIG 两臂各自重读 m_field1aa4[i]（0x085a8932 /
                    //   0x085a897d 各一条 `add $0x6a8; mov 0x4(...)`）⇒ 不缓存。
                    if (rem == 0)
                        suspectBoundary = ((unsigned int)m_field1aa4[i] > q);
                    else
                        suspectBoundary = ((unsigned int)m_field1aa4[i] >= q);
                    }

                    if (!suspectBoundary)
                    {
                        // 任务豁免：(0x10d3,idx 0xb,diff 2)/(0x10d4,0xc,2)/(0x10d8,0xf,diff>0)
                        bool exempt = false;
                        // P48：ORIG 三段豁免各自独立 call getCurCharacQuestR（0 缓存），
                        // 具名 UserQuest* 局部会合并为单次 call → P167 MISSING 2。
                        if (memberUser(this, i)->getCurCharacQuestR()->IsDoingQuest(0x10d3) &&
                            bf(this).get_dungeon_index() == 0xb &&
                            bf(this).get_dungeon_diff() == 2)
                            exempt = true;
                        else if (memberUser(this, i)->getCurCharacQuestR()->IsDoingQuest(0x10d4) &&
                                 bf(this).get_dungeon_index() == 0xc &&
                                 bf(this).get_dungeon_diff() == 2)
                            exempt = true;
                        else if (memberUser(this, i)->getCurCharacQuestR()->IsDoingQuest(0x10d8) &&
                                 bf(this).get_dungeon_index() == 0xf &&
                                 bf(this).get_dungeon_diff() > 0)
                            exempt = true;

                        if (!exempt)
                        {   // SetETC(0x84, c) 结果作为 addServerHackCnt 第 4 参 → 0x84
                            const int etc =
                                memberUser(this, i)->SetETC(0x84, m_field1aa4[i]);
                            memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                                memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x84, 1,
                                (unsigned int)etc, 0);
                        }
                    }
                }
            }
            m_field1aa4[i] = 0;

            // 单人模式换图检测：msg.m_field35==0 且 IsSinglePlay → 0x1a0
            // （ORIG 0x85a8b8f `mov 0x35(%eax),%eax` 单 dword 直读）
            if (info.m_field35 == 0 && IsSinglePlay())
            {
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x1a0, 1, 0, 0);
            }
        }

        // 本图第一组样本落盘并推进游标
        m_mapHitList1[m_mapHitCount] = sumKill;              // this+(n+0x61c)*4+0xc
        m_mapHitSum += sumKill;
        if (m_mapHitCount <= 0x2b)                           // jg 跳过递增
            m_mapHitCount = m_mapHitCount + 1;

    }
    // [R19a05 ④] ORIG 函数尾**无 return**（0x085a8c7a 直落 0x085a8c80 epilogue）：
    //   返回值 = 遗留 eax（visited 路径 0；正常路径 = 末表达式结果，非 0）。
    //   为对齐 ORIG 出口形态此处不写 return（头声明 bool 不变，mangled/ABI 不变）。
}