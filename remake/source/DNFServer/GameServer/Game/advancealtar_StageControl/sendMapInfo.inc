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
