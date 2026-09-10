void DisPatcher_PvPTimeout::send(CUser* user, ParamBase& param)
{
    stOnPvPTimeOut& out = static_cast<stOnPvPTimeOut&>(param);
    PacketGuard guard;
    if (0 < out.m_timeOutResult)
    {
        guard.put_header(1, 0x3b);
        guard.put_byte(0);
        guard.put_byte(out.m_timeOutResult);
        guard.finalize(true);
        user->Send(guard);
    }
