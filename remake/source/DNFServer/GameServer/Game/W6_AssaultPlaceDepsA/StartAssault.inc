bool CAssaultPlace::StartAssault()
{
    ClearBadP2PUser();

    if (m_field100 /*+0x100 参战人数*/ == 0) {
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\a');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\a');
        if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
            GlobalData::s_power_manager->GetPowerWarLog()->IncStartFailureCount();
            GlobalData::s_power_manager->GetPowerWarLog()->IncP2PBadCountFailure();
        }
        m_state = (ENUM_ASSAULT_STATE)7;
        CloseAssault();
        return false;
    }

    if (!_CheckPlayable()) {   // xor $1 后 test：!playable 分支
        m_state = (ENUM_ASSAULT_STATE)6;
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x06');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x06');
        if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
            GlobalData::s_power_manager->GetPowerWarLog()->IncStartFailureCount();
            GlobalData::s_power_manager->GetPowerWarLog()->IncP2PBadCountFailure();
        }
        EndAssault();
        return false;
    }

    m_state = (ENUM_ASSAULT_STATE)4;
    for (int i = 0; i < 8; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e96b7 `jne +0x1f6`（nop 岛）
        //   + 体尾 `jmp +0x1f7` ⇒ P223 双标签 goto 形。
        if (m_assaulters[i].Empty())
            goto LOOP_CONT;
        {
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)5);
            disconnect_detecter::GetInstanceDisconnectDetecter()->RegisterUser(
                m_assaulters[i].GetUser());
        }
        goto LOOP_END;
LOOP_CONT:
        ;
LOOP_END:
        ;
    }
    if (m_parties[0].m_field00 != 0)
        ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x04');
    if (m_parties[1].m_field00 != 0)
        ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x04');

    PacketGuard packet;
    _MakeStartAssault(&packet);
    SendPacket(packet);
    packet.clear();  // InterfacePacketBuf::clear @0x080cb8e6
    _MakeViewAssaultIconPlayer(&packet, true);
    _SendPacketToArea(packet);
    _SendUserState();
    TimerEndAssault::registNextTimer(
        0xb4, m_field104 /*+0x104*/, m_authenValue /*+0x108*/);

    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        GlobalData::s_power_manager->GetPowerWarLog()->IncStartSuccessCount();
        // [b28d-solo-W6_AssaultPlaceDepsA] P221：ORIG 槽序 trace1 -0x40 / traceB
        //   -0x30 / traceR -0x20（先声明深）⇒ 全表达式临时链；具名 trace 使 4.4
        //   反向（-0x34/-0x24/-0x14）并把 PacketGuard 挤到 -0x24。
        cMyTrace(__PRETTY_FUNCTION__, 0x897, 0)("POWER WAR START SUCCESS");
        for (int i = 0; i < 8; ++i) {
            // ORIG 0x082e986a `test %al,%al; jne +0x436`（nop 岛）+ 体尾 jmp。
            if (m_assaulters[i].Empty())
                goto TRACE_CONT;
            {
                if ((int)m_assaulters[i].GetTeam() == 0) {
                    // [b28d-solo-W6_AssaultPlaceDepsA] P227：ORIG 0x082e9835 段
                    //   `call getCurCharacName; mov %eax,%ebx; ...ctor...; mov %ebx,0x8(%esp)`
                    //   ⇒ 值升格 callee-saved 跨 ctor 保活；具名 name 局部使 4.4 落槽
                    //   （+store/+load 两条 = OURS-only mov 2）。改全表达式临时链内联实参。
                    cMyTrace(__PRETTY_FUNCTION__, 0x89f, 0)(
                        "BLUE TEAM : %s",
                        ((CUserCharacInfo*)m_assaulters[i].GetUser())
                            ->getCurCharacName());
                } else {
                    cMyTrace(__PRETTY_FUNCTION__, 0x8a1, 0)(
                        "RED TEAM : %s",
                        ((CUserCharacInfo*)m_assaulters[i].GetUser())
                            ->getCurCharacName());
                }
            }
            goto TRACE_END;
TRACE_CONT:
            ;
TRACE_END:
            ;
        }
    }

    m_abuseSec = /*+0x13c*/
        (int)GlobalData::s_systemTime_.getCurSec();
    _PowerWarAssaultCheckIn();
    return true;
