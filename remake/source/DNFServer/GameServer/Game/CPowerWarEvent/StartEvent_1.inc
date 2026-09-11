// ---------------------------------------------------------------------------
// ORIG 0x082679fa：同 StartEvent(Word_Param)，但不写 m_param。
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。与 StartEvent(Word_Param) 同族：
//   P221 临时链后唯一曾存残差=虚调 IsEventing(0) 目标寄存器（ORIG 0x08267a90
//   `mov (%eax),%edx` + 0x08267aa0 `call *%edx`，this 入 %eax；4.4.7 恒 %eax/%edx 对调）。
//   P230：446h47 路由后本件由 NEAR 转 AE（compare_tu_game_orig near 3→0）；p167 CLEAR 50=50。
void CPowerWarEvent::StartEvent()
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
            if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) != 0)
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, false);
                G_GameWorld()->send_all_dungeon_inout_message(0x2aff, false);
                cMyTrace(__PRETTY_FUNCTION__, 0x3a, 0)
                    ("ULTIMATE_LOG : POWER WAR START!! ULTIMATE DUNGEON CLOSE!!");
            }
            else
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x3d, 0)
                    ("ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON CLOSE!!");
            }
            GlobalData::s_power_manager->Init();
            G_GameWorld()->CleanupPowerWarVillToStart();
            GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendPowerWarCfg(
                    static_cast<unsigned short>(
                        G_CDataManager()->m_powerParameter.m_killPointToEndPowerWar),
                    G_CDataManager()->m_powerParameter.m_basicPowerWarPoint,
                    G_CDataManager()->m_powerParameter.m_firstGradeBonusPoint,
                    G_CDataManager()->m_powerParameter.m_rankingBonusDecreasePoint,
                    G_CDataManager()->m_powerParameter.m_rankingBonusGrade);
            PacketGuard packet;
            packet.put_header(0, 0x9f);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)0x7b,
                G_CDataManager()->m_powerParameter.m_refreshProcessInfo, 0, 0);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7c,
                G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c,
                0, 0);
            GlobalData::s_power_manager->SetPowerUpRemainTime(
                G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
            GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7d,
                                        5, 0, 0);
            register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
            cMyTrace(__PRETTY_FUNCTION__, 0x70, 0)
                ("Power War Event Start! server(%d)", serverNo);
        }
    }
}