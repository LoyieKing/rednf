void COnlinePreliminary::OnCompleteLoad(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(0, 0x116);
    pkt.put_short((int)(user->get_unique_id() & 0xffff));
    pkt.finalize(true);
    for (int t = 0; t <= 1; ++t)
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
