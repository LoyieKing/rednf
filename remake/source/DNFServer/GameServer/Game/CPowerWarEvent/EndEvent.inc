// ---------------------------------------------------------------------------
// ORIG 0x082681e2：环境为 1/2 时直接返回；事件未进行中直接返回；
// 进行中则关力战（开终极副本、OnEndPowerWar/CancelPowerUp 广播、
// 清共享服务器消息、强制关闭 assault、触发 0x35 事件）。
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。P221 临时链（含 0xde/0xe1/0xe4/0x110
//   四 trace）收口槽位/块序，唯一曾存残差=虚调 IsEventing(0) 目标寄存器（ORIG 0x08268226
//   `mov (%eax),%edx` + 0x08268236 `call *%edx`、this 入 %eax；4.4.7 恒目标 %eax = N7）。
//   P230：446h47 路由后本件由 NEAR 转 AE（compare_tu_game_orig near 3→0）；p167 CLEAR 34=34。
//   另修正真语义 bug：0xe4 trace 文本 ORIG .rodata 0x8bf0544 为 "…DUNGEON OPEN!!"（旧写 CLOSE）。
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
}