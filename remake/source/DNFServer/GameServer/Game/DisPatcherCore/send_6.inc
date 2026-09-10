void DisPatcher_WalkoutParty::send(CUser* user, ParamBase& param)
{
    stWalkoutPartyParam& out = static_cast<stWalkoutPartyParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0xe);
    if (out.m_errNo == 0x13)
    {
        guard.put_byte(0);
        guard.put_byte(0x13);
        guard.finalize(true);
        user->Send(guard);
    }
    else if (0 < out.m_errNo)
    {
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
        guard.finalize(true);
        user->Send(guard);
    }
