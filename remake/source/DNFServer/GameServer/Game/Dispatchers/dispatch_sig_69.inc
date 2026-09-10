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
