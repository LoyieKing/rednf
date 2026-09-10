void CPowerWarEvent::EndEvent()
{
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        return;
    }
    if (IsEventing(0) != 0)
    {
        SetEventFlag(false);
        m_param.m_duration = 0;
        m_param.m_field2 = 0;
        if (!G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
        {
            if (Timer_DungeonInoutOpenTime::checkDungeonInoutTime(0x2aff) != 0)
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, true);
                G_GameWorld()->send_all_dungeon_inout_message(0x2aff, true);
                cMyTrace(__PRETTY_FUNCTION__, 0xde, 0)
                    ("ULTIMATE_LOG : POWER WAR EVENT END!! ULTIMATE DUNGEON OPEN!!");
            }
            else
            {
                cMyTrace(__PRETTY_FUNCTION__, 0xe1, 0)
                    ("ULTIMATE_LOG : NOT UTIMATE DUNGEON OPEN TIME!!");
            }
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xe4, 0)   // ORIG 0x0826834d 行号 0xe4
                // [b28d-solo-CPowerWarEvent] 真语义 bug 修正：ORIG .rodata 0x8bf0544 实为
                // "…DUNGEON OPEN!!"（objdump -s 核验），旧文写 CLOSE 与 ORIG 不一致。
                ("ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON OPEN!!");
        }
        PacketGuard packet;
        packet.put_header(0, 0xa0);
        G_GameWorld()->OnEndPowerWar(packet);
        G_GameWorld()->CancelPowerUp();
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        CleanupPowerWarServerMessage();
        pvp_assault::GetInstanceAssaultMgr()->FrocedCloseAssaults();
        Word_Param param;
        param.m_duration = 0;
        param.m_field2 = 0;
        GlobalData::s_event_manager->TriggerEventStart(0x35, param);
        cMyTrace(__PRETTY_FUNCTION__, 0x110, 0)
            ("Power War Event End");
    }
