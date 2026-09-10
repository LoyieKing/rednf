// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::set_ready_state（原 TU 第 1945 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

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
