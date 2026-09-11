int PvP_Room::reset()
{
    battle_reset();
    lock();
    m_joinCount = 0;
    m_state = 0;
    m_pvpBattleMode = 2;
    m_mode8 = 0;
    memset(m_roomName, 0, 0x1e);
    m_seats[7] = 0;
    m_managerSeat = 0;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        P_SEATS(local_10) = 0;
        P_TEAMS(local_10) = 0xff;
        P_SEATSTATE(local_10) = 0;
    }
    if (G_GameWorld()->GetChannelType() == 6)
    {
        m_guildWarLog.Clear();
    }
    m_relayMgr.Reset();
    m_relayMgr.SetParent(this);
    m_deathMatch.Reset();
    m_deathMatch.SetParent(this);
    m_battleEndFlag = 0;
    m_rankRequested = 0;
    m_isInsertTimerRecvPvpRank = 0;
    m_isEndPvpBattle = 0;
    unlock();
}