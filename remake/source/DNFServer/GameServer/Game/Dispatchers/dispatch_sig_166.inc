// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0822759c orig/ours=189/189 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN37Dispatcher_Change_PartyMemberPosition12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN37Dispatcher_Chan
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
    if ((signed char)from >= 0 && (int)(char)from <= 3 &&
        (to == 3 || to == 1))
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
    else
    {
        guard.put_header(1, 0x150);
        guard.put_byte(0);
        guard.put_byte((int)from);
        guard.put_byte((int)(char)to);
        guard.finalize(true);
        user->Send(guard);
    }

    return 0;
}