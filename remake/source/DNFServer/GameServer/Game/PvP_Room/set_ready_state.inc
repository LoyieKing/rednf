// [b28d-solo-PvP_Room] 已归零(AE): P248 档① check_team_pvp int→bool（调用点 xor $1 取反形）+ 正极性 if(check_start_state()) + ready 形参直用（P0 槽回读 -0x1c）
int PvP_Room::set_ready_state(CUser* user, bool ready)
{
    if (((IMatch*)m_matching)->OnReady(user, ready) != 1)
    {
        return 0;
    }
    PacketGuard local_1c;
    if (m_seats[7] == user)
    {
        if (ready)
        {
            lock();
            P_SEATSTATE(m_managerSeat) = (ENUM_SEAT_STATE)ready;
            m_relayMgr.OnReady(m_managerSeat, ready);
            unlock();
            if (check_start_state())
            {
                if (!check_team_pvp())
                {
                    return 0xe;
                }
                make_ready_info((char*)&local_1c, m_managerSeat);
                send_to_pvp(local_1c);
                return 0xffffffff;
            }
            P_SEATSTATE(m_managerSeat) = 0;
            m_relayMgr.OnReady(m_managerSeat, false);
            return 0x16;
        }
    }
    else
    {
        int local_10 = 0;
        lock();
        for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) == user)
            {
                P_SEATSTATE(local_10) = ready ? 1 : 0;
                m_relayMgr.OnReady(local_10, ready);
                break;
            }
        }
        unlock();
        if (local_10 < 8)
        {
            make_ready_info((char*)&local_1c, local_10);
            send_to_pvp(local_1c);
        }
    }
    return 0;
}