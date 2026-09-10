void DisPatcher_SetArea::send(CUser* user, ParamBase& param)

{
    stSetAreaParam& p = static_cast<stSetAreaParam&>(param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
    {
        PacketGuard guard;
        guard.put_header(1, 0x26);
        guard.put_byte(0);
        guard.put_byte(p.m_errNo);
        if (p.m_errNo == 0x44)
        {
            guard.put_byte((char)p.m_9);
            guard.put_int(
                G_GameWorld()->GetLeftTimeOfForbiddenMove(
                    user->getCurCharacName()) /
                60000);
        }
        else
        {
            guard.put_byte((char)p.m_8);
        }
        guard.finalize(true);
        user->Send(guard);
    }
}
