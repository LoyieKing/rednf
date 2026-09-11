// ORIG T 0x08636e06 (_ZN26TimerCheckForcedDisconnect12dispatch_sigEiij)
// 参数语义（实测 2026-08-30）：getUser(p1, 0x2)，比较 unique_id!=p2；
// NEW 曾误用 p2/p3，已修正。
bool TimerCheckForcedDisconnect::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08636e06：结果驻留 %ebx；四段 else-if 链（jne 岛 mov$0/%1）；
    // RegistNextTimer 为成员调用（+0xca mov 0x8(%ebp)=this；+0xd5 bool 落 -0x9）。
    // [P7HeaderAxis 2026-09-11] bool 化（ORIG 整族 dispatch_sig 返回 bool）：ORIG 尾 `mov %ebx,%eax` 直返、无 test/setne。
    register bool result;
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
}