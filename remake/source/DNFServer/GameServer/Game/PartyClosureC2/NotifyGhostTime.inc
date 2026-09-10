void CAssaultMgr::NotifyGhostTime(CUser* user, int a, int ghostTime)
{
    if (user == 0)
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x185);
    packet.put_int(a);
    packet.put_int(ghostTime);
    packet.finalize(true);
    user->Send(packet);

    Timer_AssaultGhostTimer::RegisterNextTimer(user, ghostTime,
                                               (unsigned int)user->getCurCharacNo());
