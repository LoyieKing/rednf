int Dispatcher_RequestItemUnlock::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
        return 0;
    char slot = 0;                                   // -0x11
    short itemIdx = 0;                               // -0x14
    if (!packet.get_byte(slot))
        return LineFunc(0xdd4c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(itemIdx))
        return LineFunc(0xdd4d, __PRETTY_FUNCTION__, 0, 0);
    // SECURITY_PROTCTION 0x18（ItemUnlock 门禁）
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x18);
    if (secu != 0)   // ORIG：无布尔物化，直 cmpl $0,-0x10(%ebp); je
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x110, (unsigned char)secu);
        return 0;
    }
    // 槽位精确版：able@-0xc、err@-0x18、slot@-0x11、itemIdx@-0x14、secu@-0x10。
    // 声明序 able→err：4.1 初始化 store 序 = 声明序（ORIG -0xc 先清）。
    int able = 0;                                    // -0xc
    int err = 0;                                     // -0x18（IsAbleItemUnlock 出参）
    able = user->IsAbleItemUnlock(slot, itemIdx, err);
    bool anz;                     // 同上技巧22 布尔物化
    anz = (able != 0);
    if (anz)
    {
        PacketGuard packet_;                         // -0x24
        packet_.put_header(1, 0x110);
        packet_.put_byte(0);
        packet_.put_byte(able);
        packet_.put_int(err);
        packet_.finalize(true);
        user->Send(packet_);
        return 0;
    }
    user->DoItemUnlock(slot, itemIdx);
    return 0;
