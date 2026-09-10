void COnlinePreliminary::_SendMatchStatePacket(int a, int b)
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(a);
    pkt.finalize(true);
    for (int t = 0; t < b; ++t)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = m_teams[t].m_seats[i].m_user;
            /* ORIG 内层尾 call 后无跳板直落 addl：双 je 直指循环增量块
             * （正向 && 【技巧11】，同 _IncreaseEachOtherPlayCount()） */
            if (m_teams[t].m_seats[i].m_flag != 0 && u != 0)
                u->Send(pkt);
        }
    }
