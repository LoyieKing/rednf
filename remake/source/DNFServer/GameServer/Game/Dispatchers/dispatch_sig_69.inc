// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08222e00 orig/ours=97/93 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movb   $0x0,-0x11(%ebp) | movw   $0x0,-0x14(%ebp) | lea    -0x11(%ebp),%eax
int Dispatcher_RequestItemLock::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
        return 0;
    char lock = 0;                                         // -0x11
    short slot = 0;                                        // -0x14
    if (!packet.get_byte(lock))
    {
        return LineFunc(0xdd26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(slot))
    {
        return LineFunc(0xdd27,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int err = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x17);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10f, (unsigned char)err);
        return 0;
    }
    err = user->IsAbleItemLock((int)lock, (int)slot);      // ORIG 0x08646a9c
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10f, (unsigned char)err);
        return 0;
    }
    user->DoItemLock((int)lock, (int)slot);                // 0x08646d4a
    return 0;
}