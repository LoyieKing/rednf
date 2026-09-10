int CPcRoomPlayTime::processEventPcRoomPlayTime(CUser* user)
{
    // [b28c-L0 N22] 首比较 ORIG `cmp %edx,%eax`（call 结果留 %eax），4.4 恒
    // `mov %eax,%edx` 中转；register int 局部复现 ORIG edi/esi/ebx callee-saved
    // 分配（P0 仅禁 register asm 绑定）。116=116，语义全同。
    user->collectPcRoomPlayTime();               // UNLINKED_CALLEE（T 待落地）
    if ((unsigned int)user->getTotalPcRoomPlayTime() <
        (unsigned int)static_cast<PcBangPlayTimeScript*>(m_script1c)
            ->m_playTime)
        return 0x13;

    user->incPcRoomPlayTimeRewardCnt();
    register int rewardCnt = user->getPcRoomPlayTimeRewardCnt();
    register int totalTime = user->getTotalPcRoomPlayTime();
    register int characNo = (int)user->getCurCharacNo();
    register int accId = (int)user->get_acc_id();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x8ea, 0);
    trace(
        "<PCROOM_PLAY_TIME_EVENT> send_reward m_id(%d), charac_no(%d), "
        "total_time(%d), reward_cnt(%d)",
        accId, characNo, totalTime, rewardCnt);
    user->resetTotalPcRoomPlayTime();

    ::DB_SavePcRoomPlayTimeEvent::makeRequest(
        (unsigned int)user->get_acc_id(),
        (unsigned int)user->getTotalPcRoomPlayTime(),
        user->getPcRoomPlayTimeRewardCnt());

    Packet_PcRoomPlayTimeReward packet;
    // ORIG +0x121..+0x13c：then/else 两臂各自 store（`mov %al,-0x31(%ebp)` ×2），
    // 三目形会被 4.4 合并为单一汇合 store ⇒ 写 if/else。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        packet.m_field0a = (unsigned char)user->GetServerGroup();
    else
        packet.m_field0a =
            (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    packet.m_accId = user->get_acc_id();
    packet.m_characNo = user->getCurCharacNo();

    GlobalData::s_monitor_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x13);
    return 0;
