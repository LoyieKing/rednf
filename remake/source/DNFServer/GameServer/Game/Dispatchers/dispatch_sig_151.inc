int Dispatcher_RequestItemUnlockCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
        return 0;
    char slot = 0;                                   // -0x11
    short itemIdx = 0;                               // -0x14
    if (!packet.get_byte(slot))
        return LineFunc(0xdd7b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(itemIdx))
        return LineFunc(0xdd7c, __PRETTY_FUNCTION__, 0, 0);
    // SECURITY_PROTCTION 0x19（ItemUnlockCancel 门禁）
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x19);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x111, (unsigned char)secu);
        return 0;
    }
    int able = 0;                                    // ORIG：调用前 -0xc 预清零
    able = user->IsAbleItemUnlockCancel(slot, itemIdx);
    bool nz;                      // ORIG: cmp $0,-0xc; setne %al; test %al,%al; je
    nz = (able != 0);
    if (nz)                                          // 技巧22：bool 物化不折叠
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x111, (unsigned char)able);
        return 0;
    }
    user->DoItemUnlockCancel(slot, itemIdx);
    return 0;
