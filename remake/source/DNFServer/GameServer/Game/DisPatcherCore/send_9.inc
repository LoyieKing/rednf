void DisPatcher_DeleteItem::send(CUser* user, ParamBase& param)
{
    stDeleteItemParam& p = static_cast<stDeleteItemParam&>(param);
    PacketGuard guard;
    if (p.m_4 != 0x7fffffff)
    {
        if (p.m_4 == 0)
        {
            guard.put_header(1, 0x12);
            guard.put_byte(1);
            guard.put_byte((char)p.m_8);
            guard.put_byte((char)p.m_9);
            for (int i = 0; i < (int)(char)p.m_9; ++i)
            {
                guard.put_short((int)p.m_slotsD2[i]);
                guard.put_int(p.m_counts19c[i]);
                guard.put_short((int)p.m_slotsA[i]);
            }
            guard.finalize(true);
            user->Send(guard);
        }
        else
        {
            guard.put_header(1, 0x12);
            guard.put_byte(0);
            guard.put_byte(p.m_4);
            guard.put_byte((char)p.m_8);
            guard.finalize(true);
            user->Send(guard);
        }
    }
