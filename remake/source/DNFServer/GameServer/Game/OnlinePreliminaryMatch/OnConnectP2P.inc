void COnlinePreliminary::OnConnectP2P(CUser* user, PvP_Room* room,
                                      char* addr, int len)
{
    if (m_field04 != 4)
        return;

    for (int i = 0; i <= 7 && i < len; ++i)
    {
        if (addr[i] == 0)
        {
            _SendMatchStatePacket(4, 2);
            OnEndFight(room);
            return;
        }
    }

    ++m_p2pReadyCount;                                    /* this+0x6c */
    if ((int)m_p2pReadyCount ==
        (int)(unsigned char)GetPlayCountPerOneTeam() * 2)
    {
        PacketGuard ready;
        ready.put_header(0, 0x117);
        ready.finalize(true);
        for (int t = 0; t <= 1; ++t)
        {
            for (int i = 0; i <= 3; ++i)
            {
                CUser* u = m_teams[t].m_seats[i].m_user;
                /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08561cb0-0x151：
                 * `test %al,%al; je skip` + `cmpl $0,-0x10; je skip`（双 je
                 * 直指增量块、无 setne/无岛）⇒ 正向 && 单语句（P296/技巧11）；
                 * 旧 `==0 || ==0 continue` 产 xor$1 物化 + 双 jmp/nop 岛。 */
                if (m_teams[t].m_seats[i].m_flag != 0 && u != 0)
                    u->Send(ready);
            }
        }
    }

    {
        PacketGuard st;
        st.clear();
        room->battle_reset();                             /* T 0x85d6430 */
        for (int s = 0; s <= 7; ++s)
        {
            if (room->get_user_seat(s) != 0)
                room->m_relayMgr.OnReady(s, true);        /* +0x620 子对象 */
        }
        room->start_pvp();                                /* T 0x85d8c3a */
        // ORIG：make_state_info 写入 st 自身缓冲（Ghidra local_38 复用，ORIG `(char*)local_38`），
        // send_all(st) 发送同一包。
        room->make_state_info(reinterpret_cast<char*>(&st));  /* T 0x85dbd3a */
        G_GameWorld()->send_all(st);
    }
