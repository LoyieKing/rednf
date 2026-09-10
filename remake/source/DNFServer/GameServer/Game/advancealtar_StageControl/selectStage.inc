int StageControl::selectStage(CUser* user, int stageId,
                              advancealtar::StageDifficulty::T diff)
{
    if (user == 0)
    {
        return 3;
    }
    // [b28d-D9] ORIG 0x0812fdd4 段每次重算 `add $0x8df60,%eax`（P183 地址重物化，
    // 无具名引用槽）；具名 CharacAdvanceAltarManager& 使 4.4 落 -0x18 槽往返。
    if (!user->m_advanceAltar.isAvailableTciket())
    {
        return 0xf5;
    }
    if (!user->m_advanceAltar.isEnterStage(stageId))
    {
        return 8;
    }

    // [b28d-solo-StageControl] P183/P0：ORIG 每个访问点重算 this+0x98 / this+0x4
    // （无 getCharacData/getProcStage call、无具名指针槽）；逐点表达式直用。
    reinterpret_cast<CharacData*>(m_characData)->m_field0 =
        user->m_advanceAltar.getCurrentRidableId();
    reinterpret_cast<CharacData*>(m_characData)->setUser(user);
    reinterpret_cast<CharacData*>(m_characData)->setSlotItem();

    reinterpret_cast<ProcStage*>(m_procStage)->resetAll();
    // P198①：ORIG 在 startStage 前有 `movl $0x0,-0xc(%ebp)` 预清 ⇒ 两句式。
    int err = 0;
    err = reinterpret_cast<ProcStage*>(m_procStage)->startStage(stageId, diff);
    if (err != 0)
    {
        return err;
    }

    sendStageInfo();
    sendMapInfo();
    reinterpret_cast<ProcStage*>(m_procStage)->setNotSurvivalModeFirstRound();
    user->set_state((ch_state)0xd);
    return 0;
