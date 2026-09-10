int TimerCheckForcedDisconnect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08636e06：结果驻留 %ebx；四段 else-if 链（jne 岛 mov$0/%1）；
    // RegistNextTimer 为成员调用（+0xca mov 0x8(%ebp)=this；+0xd5 bool 落 -0x9）。
    register int result;
    CUser* user = G_CGameManager()->getUser(p1, 0x2);
    if (user == 0)
    {
        result = 0;
    }
    else if (user->get_unique_id() != p2)
    {
        result = 0;
    }
    else if (user->get_state() != 7)
    {
        result = 1;
    }
    else
    {
        PacketGuard packet;
        packet.put_header(0, 0x7b);
        packet.finalize(true);
        user->Send(packet);
        const bool registered = RegistNextTimer(user);
        if (!registered)
            result = 0;
        else
            result = 1;
    }
    return result;
