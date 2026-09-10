void DisPatcher_StartGame::send(CUser* user, ParamBase& param)
{
    stStartGameParam& out = static_cast<stStartGameParam&>(param);
    if (out.m_errNo == 0x7fffffff)
        return;
    if (out.m_errNo != 0)
    {
        PacketGuard guard;
        guard.put_header(1, 0xf);
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
        if (out.m_8 >= 0)
        {
            guard.put_byte(out.m_8);
        }
        guard.finalize(true);
        user->Send(guard);
    }
