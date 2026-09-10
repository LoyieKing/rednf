void CAssaultPlace::DieUser(int victimIdx, int killerIdx, MSG_DIE_ASSAULT_PLAYER& msg)
{
    char killerName[0x15] = { 0 };               // ORIG 0x15 字节初始化清零展开（非 memset call）

    if (m_state != 4)
        throw (ENUM_ERROR)0x13;

    CUser* victim = m_assaulters[victimIdx].GetUser();
    if (m_assaulters[victimIdx].GetState() != 5)
        throw (ENUM_ERROR)0x12;

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ebda6 在函数头预清 killer/victim
    //   两槽（-0x2c/-0x28）；实测三形（头部双预清 ndiff 390 / 仅 killer 391 /
    //   原位 347）以原位最优 ⇒ 保持原位（槽序差异归 N82）。
    CUser* killer = 0;
    if (killerIdx >= 0 && killerIdx <= 7) {
        killer = m_assaulters[killerIdx].GetUser();
        if (m_assaulters[killerIdx].GetState() != 5) {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xd37, 0);
            trace("CAssaultPlace::DieUser => killer is already died!");
        }
        strncpy(killerName, m_assaulters[killerIdx].GetCharacName(), 0x14);
        if (killer != 0 && msg.m_damage != 0) {
            if (msg.m_damage == 1) {
                killer->getHackAnalyzer()->addServerHackCnt(
                    killer, static_cast<WongWork::ENUM_HACKTYPE>(0x7d), 1, 0, 0);
            }
            if ((int)(msg.m_attackCount / msg.m_damage) > 100000) {  // ORIG divl + jle（有符号比较，P196）
                killer->getHackAnalyzer()->addServerHackCnt(
                    killer, static_cast<WongWork::ENUM_HACKTYPE>(0x7e), 1, 0, 0);
            }
        }
    } else {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xd54, 0);
        trace("CAssaultPlace::DieUser invalid killer index income(%d,%d)", victimIdx, killerIdx);
    }

    if (victim != 0) {
        cUserHistoryLog* hist =
            &victim->m_historyLog;                             // +0x79700
        if (victim->getCurCharacVill() == 7
            && GlobalData::s_power_manager->IsPowerWarEventOn()) {
            hist->DiePvP((ENUM_PVP_HISTORY_REASON)2,
                         msg.m_damage, msg.m_attackCount,
                         (int)(short)msg.m_field08, (int)(short)msg.m_field0a,
                         (int)(short)msg.m_field0c, (int)(short)msg.m_field0e,
                         killerName);
        } else {
            hist->DiePvP((ENUM_PVP_HISTORY_REASON)6,
                         msg.m_damage, msg.m_attackCount,
                         (int)(short)msg.m_field08, (int)(short)msg.m_field0a,
                         (int)(short)msg.m_field0c, (int)(short)msg.m_field0e,
                         killerName);
        }
    }

    if (_IsChangeDieState())
        _ChangeDieState(victimIdx);

    if (victim != 0
        && !(victim->getCurCharacVill() == 7
             && GlobalData::s_power_manager->IsPowerWarEventOn())) {
        PacketGuard packet;
        _MakeUserState(&packet, victim);
        G_GameWorld()->send_all(packet);
    }

    if (killer != 0
        && !(killer->getCurCharacVill() == 7
             && GlobalData::s_power_manager->IsPowerWarEventOn())) {
        if (_IsProcsssChaos())
            _ProcessChaos(victimIdx, killerIdx);
    }

    if (!_CheckAssaultable())
        return;

    if (victim != 0 && killer != 0
        && victim->getCurCharacVill() == 7
        && GlobalData::s_power_manager->IsPowerWarEventOn()) {
        int winCnt = 0, loseCnt = 0, aliveCnt = 0;
        int side = 0;          // ORIG local_28 int 槽（movsbl 物化后 dword 存）
        int winnerType = 0;
        if (killerIdx >= 0 && killerIdx <= 7) {
            side = (int)(signed char)killer->getPowerSide();
            winnerType = 1;
        } else {
            side = (int)(signed char)victim->getPowerSide();
            winnerType = 2;
        }
        if (_CheckPowerWarInPlayer((ENUM_POWER_SIDE_TYPE)side,
                                   (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                   winCnt, loseCnt, aliveCnt)) {
            if (winnerType == 1)
                _SendPowerWarPointToGuildServer(killer,
                                                (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                                winCnt, loseCnt, aliveCnt);
            else
                _SendPowerWarPointToGuildServer(victim,
                                                (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                                winCnt, loseCnt, aliveCnt);
            PacketGuard packet;
            _MakeUserState(&packet);
            G_GameWorld()->send_all(packet);
            _SendStraightVictories(killer);
        }
    }

    if (victim != 0
        && victim->getCurCharacVill() == 7
        && GlobalData::s_power_manager->IsPowerWarEventOn()) {
        CParty* party = static_cast<CParty*>(victim->GetParty());
        if (party != 0)
            party->resetStraightVictories();
        else
            victim->resetStraightVictories();
    }

    m_state = (ENUM_ASSAULT_STATE)5;
    // ORIG 直读 m_parties[i].m_field00（+0x12c/+0x130），无 Get() call
    if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\x05');
    if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\x05');
    EndAssault();
