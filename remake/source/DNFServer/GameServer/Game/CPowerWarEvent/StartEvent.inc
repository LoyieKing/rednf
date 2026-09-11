// ---------------------------------------------------------------------------
// ORIG 0x08267dea：环境为 1/2 时向 monitor 发 Event_End 包；
// 否则事件未进行中则直接返回，进行中则开始力战（关终极副本、发配置、
// 挂三个定时器）。
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。P221 全表达式临时链收口槽位/块序；
//   唯一曾存残差=虚调 IsEventing(0) 间接目标寄存器（ORIG 0x08267e80 `mov (%eax),%edx` +
//   0x08267e90 `call *%edx`、this 入 %eax；4.4.7 恒目标 %eax、this 入 %edx = N7）。
//   P230：本件属 N7 敏感族，Main 已将本 TU 纳入 446h47「仅换 4.4.6 后端」路由 ⇒
//   compare_tu_game_orig: identical=2 ae=11 near=0（本件由 NEAR 转 AE）；p167 CLEAR 50=50。
void CPowerWarEvent::StartEvent(Word_Param param)
{
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        Packet_Monitor_Event_End monEnd;
        monEnd.m_eventCode = 0x1e;
        int group = G_CEnvironment()->get_server_group();
        GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group)
            ->SendTcpPacket(reinterpret_cast<char*>(&monEnd), 0xe);
    }
    else
    {
        if (!IsEventing(0))
        {
            SetEventFlag(true);
            m_param = param;
            if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) != 0)
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, false);
                G_GameWorld()->send_all_dungeon_inout_message(0x2aff, false);
                // [b28d-26] P221：具名 tr 改构造即调用（临时链），复现 ORIG 槽序
                cMyTrace(__PRETTY_FUNCTION__, 0x8f, 0)
                    ("ULTIMATE_LOG : POWER WAR START!! ULTIMATE DUNGEON CLOSE!!");
            }
            else
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x92, 0)
                    ("ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON CLOSE!!");
            }
            GlobalData::s_power_manager->Init();
            G_GameWorld()->CleanupPowerWarVillToStart();
            GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendPowerWarCfg(
                    static_cast<unsigned short>(
                        G_CDataManager()->m_powerParameter.m_killPointToEndPowerWar),   // +0xa634
                    G_CDataManager()->m_powerParameter.m_basicPowerWarPoint,            // +0xa6a4
                    G_CDataManager()->m_powerParameter.m_firstGradeBonusPoint,          // +0xa6a8
                    G_CDataManager()->m_powerParameter.m_rankingBonusDecreasePoint,     // +0xa6b0
                    G_CDataManager()->m_powerParameter.m_rankingBonusGrade);            // +0xa6ac
            PacketGuard packet;
            packet.put_header(0, 0x9f);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)0x7b,
                G_CDataManager()->m_powerParameter.m_refreshProcessInfo, 0, 0);        // +0xa658
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7c,
                G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c,
                0, 0);                                                                  // +0xa670
            GlobalData::s_power_manager->SetPowerUpRemainTime(
                G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
            GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7d,
                                        5, 0, 0);
            register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;       // +0x1b0
            cMyTrace(__PRETTY_FUNCTION__, 0xc6, 0)
                ("Power War Event Start! server(%d)", serverNo);
        }
    }
}