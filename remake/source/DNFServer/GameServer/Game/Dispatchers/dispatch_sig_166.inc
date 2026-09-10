int Dispatcher_Change_PartyMemberPosition::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822759c [0x0822759c, 0x08227861]
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return 0;
    }
    if (party->getManager() != user)
    {
        return 0;
    }
    char from = 4;                                           // -0xd（ctor 初始 4）
    char to = 0;                                             // -0xe
    if (!packet.get_byte(from))
    {
        return LineFunc(0xe384,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(to))
    {
        return LineFunc(0xe385,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PacketGuard guard;
    if (*reinterpret_cast<unsigned char*>(&from) > 0x7f || 3 < (int)(char)from ||
        (to != 3 && to != 1))
    {
        guard.put_header(1, 0x150);
        guard.put_byte(0);
        guard.put_byte((int)from);
        guard.put_byte((int)(char)to);
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        to = (char)(from + to) & 3;
        party->switchPostion((char)from, (char)to);
        party->send_party_realtime_info();
        party->send_party_ipinfo();
        guard.put_header(1, 0x150);
        guard.put_byte(1);
        guard.put_byte((int)from);
        guard.put_byte((int)(char)to);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
