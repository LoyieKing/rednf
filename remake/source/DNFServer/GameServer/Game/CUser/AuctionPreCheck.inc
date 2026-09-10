int CUser::AuctionPreCheck(ENUM_CMDPACKET cmd, PAY_TYPE pay)
{
    // b28d-solo-CUser 重写（2026-09-09，P226/P214①）：ORIG 无 fail 具名局部——两段门
    //   均为「pay 值比较 && !IsRunning()」**直接 if 短路**（4.1 物化 `mov $1,%eax;
    //   jmp; mov $0,%eax; test %al,%al; je` 到寄存器，0 槽往返）；旧 `bool fail`+goto
    //   SET01/SET02 结构被 4.4 落槽（movb $1,-0xd / cmpb $0,-0xd）。裸 if 形即复现。
    if (pay == (PAY_TYPE)1 && !GlobalData::s_cera_auction_proxy->IsRunning())
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (pay == (PAY_TYPE)0 && !GlobalData::s_auction_proxy->IsRunning())
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (get_state() <= 2 || getCurCharacR() == 0)
    {
        return 0x8151;
    }

    int err =
        GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)0x35);
    if (err != 0)
    {
        SendCmdErrorPacket(cmd, (unsigned char)err, pay);
        return 0;
    }

    return -1;
