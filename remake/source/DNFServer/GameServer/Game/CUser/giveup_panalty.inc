void CUser::giveup_panalty()
{
    // ORIG 0x86786cb `test %al,%al; jne` 无 setne 物化 ⇒ IsLightServer bool 返回（声明 DataManagerInitDepends.h:42/62 待 bool 化；(bool)cast 在 char 声明下反产 setne，故不加）
    if (IsLightServer())
        return;
    // ORIG 0x086786d6 `movzbl 0x711d8; test; jne`（bool 直读；= CUser::m_gmUserFlag）
    if (m_gmUserFlag)
        return;
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) != 0)
        return;
    if (get_charac_level() <= 0x11)
        return;
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) != 0)
        return;
    // ORIG 0x8678742 cmp $0x1;setne;test;je merge：物化 true→fall body（probe20 b 形）
    if ((bool)(getMoveSpace() != 1))
    {
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x21);
    setCurCharacStamina((unsigned char)0xa);
    ((InterfacePacketBuf*)&packet)->put_byte(10);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
    }
