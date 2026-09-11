// ----------------------------------------------------------------------------
// ORIG T 0x081301e0（sendMapInfo.md 全文，0416B）：user/map/timeLine 三重判空；
 // _NotiAdvanceAltarMapInfo(0x14 packed){stageType, clearSurvivalRoundNo,
 // mapIndex=CMap::get_index, currentPhaseIndex=timeline+0xc, mapBasisLevel}；
 // put_header(0,0x20a)+put_binary(0x14)；put_int(phaseSummarys.size()) 后逐
 // _PhaseSummary put_binary(0xc)；生存模式且非首轮：put_int(getCenterCount())
 // 并遍历怪物表逐只构造 _MapMonsterSummary（仅 getTowerInfo 命中者
 // addCreatedMapMonster + put_binary(0xd)）；否则 put_int(0)；finalize→Send。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] sendMapInfo：ORIG 293 / ours 290。已修：P183 去
//   getCharacData/getProcStage out-of-line call 与具名 procStage 槽（每点重算
//   this+0x98/this+0x4）；body ctor 移到 map 判空后/getTimeLine 前（ORIG 0x0813026e）；
//   P214① sendMonsters 改直接 `if (a||b)`（无具名 bool 槽）；else 分支具名 zero 局部；
//   Send 前重新 `this+0x98` 调 getUser。
// [b28d-sweep 2026-09-09 重判] 旧判词「-0x20 死计数器/-0x18 list 指针槽/it++ 引入即
//   反劣」**全部作废**——旧读数是在 3 处真 bug + put_binary 重载 + hit 槽未补的前提下
//   测的。本轮修后（clean 口径 ORIG 289 / ours 292）：
//   ①真 bug：`!isSurvivalModeFirstRound()` 极性反（ORIG je/jne 同落 L_true，P224）；
//   ②真 bug：phase 循环 put_binary 误选 PKc 重载（ORIG 三处均 Pc @0x0811df08，P167）；
//   ③真 bug：getTowerInfo 首实参误传 m_orderIdx，ORIG 取 it->+0x00=m_mobId（0x08130454）；
//   ④补 hit 具名 uchar 槽（ORIG 0x0813047c `mov %al,-0xd`，P0）；
//   ⑤phase/monsters 两循环改后置 `it++`（ORIG 调 ppEi，P28 增量侧）；
//   ⑥补 phase 死计数器（ORIG 0x08130308 `movl $0,-0x20` + 每轮 `addl $1,-0x20`，P171）；
//   ⑦补 monsters 具名 list 指针槽（ORIG 0x081303d9 `mov %eax,-0x18`）；
//   ⑧删 cam 具名引用槽（ORIG 每轮重算 lea 0x8df60，P183）；
//   ⑨删原两个撑帧 double（补 ④⑥⑦ 后 4.4 帧自涨到 0xa0，删后回 0x90 与 ORIG 逐字对齐）。
//   残余 = hit 取反物化（ORIG movzbl/xor/test/jne 4 条 vs 4.4 `cmpb $0` 2 条，P168a④
//   /N18b 封顶）+ 槽号分配差（N95/N126 版本常数）。p167：put_binary 差集清零，仅剩
//   D1/D2、C1/C2 同址别名与 list const/非 const 迭代器符号选择差（非语义缺口）。
// [round1-L29 2026-09-10 复核] 我方 292 / ORIG 289；判据① 分支数 66 vs 67，无目标错位。
//   残差含 getTowerInfo 的 X 族（同 getCenterCount，待 callee bool 化）+ 槽号 N95/N126。
//   ORIG 0x081301e0。
// [S4AltarStageMisc 2026-09-11 复核] p167：46 vs 48 calls，差集**全部**为 C1/C2、D1/D2
//   同址别名 + `_ZNKSt4listI11_mapMonsterSaIS0_EE5beginEv/3endEv`（ORIG const 版）vs
//   我方 non-const + `_List_const_iteratorI11_mapMonsterEC1ERKSt14_List_iteratorIS0_E`×2
//   （仅符号选择差，语义等价；const 形实测 identical −1，见函数体注）⇒ 维持非 const。
//   §6.5 串核查：本 TU .rodata 全串（跳转表 + "%d"）在 ORIG 逐字存在，无发明串；
//   §6.5.1 编码：enc_kr_scan 全库 0 命中。
//   GI 假缺口（按名跨 TU 错配；ORIG 该符号实测 493 个实例，工具取首个 0x0807ee76 属
//   PacketTypes）⇒ 逐 TU 读数不可比；我方 16 条 = `ios_base::Init`+`__cxa_atexit` 正确形，
//   本 TU .bss 仅 STL 内部对象、无 advancealtar 静态对象 ⇒ 零改动。
void StageControl::sendMapInfo()
{
    // [b28d-solo-StageControl] P183：ORIG 每访问点重算 this+0x98 / this+0x4
    // （无 getCharacData/getProcStage out-of-line call、无具名 procStage 指针槽）。
    // [b28d-sweep] 原 P190 两个取址 double 撑帧（0x80→0x90）；补 hit/phaseIndex/
    //   monsterList 具名局部后 4.4 帧已自涨到 0xa0（ORIG 0x90）⇒ 撑帧局部转为冗余，
    //   删除后帧回落 0x90（实测 sub 立即数逐字对齐）。
    CUser* user = reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return;
    }
    CMap* map = reinterpret_cast<ProcStage*>(m_procStage)->getMap();
    if (map == 0)
    {
        return;
    }
    // ORIG 0x0813026e：body ctor 在 map 判空之后、getTimeLine 之前（Ghidra local_5c）。
    NOTIPacketStruct::_NotiAdvanceAltarMapInfo body;
    const TimeLine* timeLine =   // [b28d-Main 代落·S374] getTimeLine 返回 const TimeLine*（ORIG 调用点 0x8130237 为 `_ZNK...getTimeLineEv`，消费端仅读）
        reinterpret_cast<ProcStage*>(m_procStage)->getTimeLine();
    if (timeLine == 0)
    {
        return;
    }

    body.m_stageType =
        reinterpret_cast<ProcStage*>(m_procStage)->getStageType();
    body.m_currentSurvivalRoundNo =
        reinterpret_cast<ProcStage*>(m_procStage)->getCurrentSurvivalRoundNo();
    body.m_mapIndex = map->get_index();
    // TimeLine+0x0c = m_summary（TimeLineSummary）+0x00 = m_field0（int，`mov 0xc(%eax)`）。
    body.m_currentPhaseIndex = timeLine->m_summary.m_field0;
    body.m_mapBasisLevel =
        reinterpret_cast<ProcStage*>(m_procStage)->getMapBasisLevel();

    PacketGuard guard;
    guard.put_header(0, 0x20a);
    guard.put_binary(reinterpret_cast<char*>(&body), 0x14);
    guard.put_int(static_cast<int>(timeLine->m_summary.m_phaseSummarys.size()));
    // [b28d-sweep] P167：ORIG 三处 put_binary 目标均为 `_ZN18InterfacePacketBuf10put_binaryEPci`
    //   （char* 重载 @0x0811df08），此前 phase 循环误用 `put_binary(const char*,int)`
    //   （@0x0815096a）⇒ 调用点差集 MISSING/EXTRA 各 1。改传 char*（const_cast 去 const）。
    // [b28d-sweep] P28 增量侧：ORIG 0x08130393 增量段调
    //   `_ZN9__gnu_cxx17__normal_iteratorIPKN...EEEppEi`（后置，带独立 sret 临时槽
    //   -0x40）⇒ 源形写 `it++`。
    // P328⑥/P171：ORIG 在 `call begin`（sret -0x68）之后发 `movl $0x0,-0x20(%ebp)`，
    //   每轮增量段 `addl $0x1,-0x20(%ebp)`（死计数器，全函数无读者）⇒ 照抄死写入。
    std::vector<_PhaseSummary>::const_iterator it =
        timeLine->m_summary.m_phaseSummarys.begin();
    int phaseIndex = 0;
    for (; it != timeLine->m_summary.m_phaseSummarys.end(); it++, ++phaseIndex)
    {
        guard.put_binary(reinterpret_cast<char*>(
                             const_cast<_PhaseSummary*>(&*it)), 0xc);
    }

    // ORIG 0x08130391：`cmp $1,%eax; jne L_true; call; test; je L_false; L_true: mov $1;
    // jmp; L_false: mov $0; test %al,%al; je L_else` —— 无具名 bool 槽（P214①）。
    // 极性：jne/je 均落 L_true ⇒ monsters 分支 = `stageType != 1 || isSurvivalModeFirstRound()`
    // （P224：je 落点定极性；此前误写 `!isSurvivalModeFirstRound()` = 真语义 bug）。
    if (reinterpret_cast<ProcStage*>(m_procStage)->getStageType() != 1 ||
        reinterpret_cast<ProcStage*>(m_procStage)->isSurvivalModeFirstRound())
    {
        // [b28d-sweep] 试过 const 化 monsters（ORIG 0x081303ce 段确调 `_ZNKSt4list...5beginEv`
        //   const 版）：p167 调用差集 48→46=ORIG，但本 TU 少发射
        //   `_ZNSt20_List_const_iteratorI11_mapMonsterEC1ERKSt14_List_iteratorIS0_E`
        //   ⇒ check_tu identical 80→79 回退，故维持非 const 形（语义等价，仅符号选择差）。
        std::list<_mapMonster> monsters = map->get_monster_list();
        // ORIG 0x081303d6：`lea -0x3c(%ebp),%eax; mov %eax,-0x18(%ebp)` ⇒ 具名 list
        //   指针槽（begin/end 均从该槽取 this）。
        std::list<_mapMonster>* monsterList = &monsters;
        guard.put_int(getCenterCount());
        // [b28d-sweep] P183：ORIG 0x08130456 每轮重算 `mov -0x2c(%ebp),%edx;
        //   lea 0x8df60(%edx),%ecx`（无具名引用槽）⇒ 去掉 CharacAdvanceAltarManager& 局部。
        // [b28d-sweep] P28 增量侧：ORIG 增量段调 `_ZNSt20_List_const_iteratorI11_mapMonsterEppEi`
        //   （后置，带 sret 临时槽）⇒ 源形写 `it++`。
        for (std::list<_mapMonster>::const_iterator it = monsterList->begin();
             it != monsterList->end(); it++)
        {
            advancealtar::_MapMonsterSummary summary;
            summary.m_orderIdx = it->m_orderIdx;          // _mapMonster+0x2c
            summary.m_seq = 0;                            // addCreatedMapMonster 覆写
            int towerValue = 0;                           // getTowerInfo 出参 1
            int towerCount = 0;                           // getTowerInfo 出参 2
            // [b28d-sweep] 真 bug：ORIG 0x08130454 `mov (%eax),%eax` 取 it->+0x00
            //   （_mapMonster::m_mobId）作 getTowerInfo 首实参；此前误传 m_orderIdx
            //   （+0x2c，仅用于下方 summary.m_orderIdx 赋值，ORIG 0x0813042c 实证）。
            //   ORIG 0x0813047c `mov %al,-0xd(%ebp)` + 回读 ⇒ 具名字节局部（P0 槽往返）。
            unsigned char hit = user->m_advanceAltar.getTowerInfo(
                it->m_mobId, towerValue, towerCount);
            // [round1-L29 2026-09-10 复核] bool 具名局部 + P223 goto 尾标签形实测劣化
            //   （295 vs ORIG 289）⇒ 回退保持 uchar 形；残差归 N（ORIG 0x0813047c）。
            // [g12 2026-09-11 试形·实测反劣回退] P168a① 守卫形 `if (!hit) goto skip;`
            if (hit)
            {
                summary.m_towerRefA =
                    static_cast<short>(towerValue);       // u16 截断 [+0x06]
                summary.m_towerRefB =
                    static_cast<unsigned char>(towerCount);  // 低字节 [+0x08]
                summary.m_roleType = static_cast<unsigned char>(
                    it->m_roleType);                      // +0x18 低字节
                summary.m_posX = static_cast<unsigned char>(
                    it->m_posX);                          // +0x20 低字节
                summary.m_champion = it->m_champion;      // +0x1c
                reinterpret_cast<ProcStage*>(m_procStage)->
                    addCreatedMapMonster(summary);
                guard.put_binary(reinterpret_cast<char*>(&summary), 0xd);
            }
        }
    }
    else
    {
        // ORIG 0x08130570：`movl $0x0,-0xc(%ebp); mov -0xc(%ebp),%eax; ... put_int`
        // —— 具名零局部（P0 槽往返），非字面量直压。
        int zero = 0;
        guard.put_int(zero);
    }
    guard.finalize(true);
    // ORIG 0x0813059c：Send 前重新 `this+0x98` 调 getUser（不复用开头 user 槽）。
    reinterpret_cast<CharacData*>(m_characData)->getUser()->Send(guard);
}