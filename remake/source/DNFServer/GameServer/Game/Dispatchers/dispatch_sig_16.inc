int Dispatcher_ChangeHost::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8204c1e：state>=3 才处理；teleport_state 在 [0,3) 反走城错误 0x9e
    if (user->get_state() <= 2)
    {
        return LineFunc(0x9cbf,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char slot;
    if (!packet.get_byte(slot))
    {
        return LineFunc(0x9cc2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x8204ca4：`movl $0x0,-0x10(%ebp)` 先落 ret 的 0 初值（-0x10 远槽），
    // party 后声明占 -0xc（技巧21 声明序定槽号；ORIG 0x8204cbx `mov %eax,-0xc(%ebp)`）。
    int ret = 0;
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x7c, 0x13);
        return 0;
    }
    if (static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->get_teleport_state()
            >= 0
        && static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->get_teleport_state()
            <= 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x7c, 0x9e);
        return 0;
    }
    ret = party->change_manager(user, (int)slot);
    if (ret > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x7c, (unsigned char)ret);
    }
    return 0;
}