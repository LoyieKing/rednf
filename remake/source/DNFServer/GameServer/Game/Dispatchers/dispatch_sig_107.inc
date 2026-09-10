int Dispatcher_AuctionRegistCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08214b96..0x08214e43 (0x2ae 字节)。GM 表 _ZL6gmList @0x093abb4c。
    // 包：PCK_AUCTION_REGIST_CANCEL_GP（ctor W 0x0822ea1c）/ _GA（ctor W 0x0822e5c4）
    //   uid@0x12, characNo@0x16, auctionId[8]@0x1a。
    static const int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

    // ORIG 0x08214b96：[4] `lea -0x29(%ebp),%eax` 直取址、无 `movb $0x0` 预清（P328⑤）。
    unsigned char payType;
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc47f, __PRETTY_FUNCTION__, 0, 0);
    if (payType > 1)
        return LineFunc(0xc482, __PRETTY_FUNCTION__, 0, 0);

    const int pay = payType;
    const int preCheck = user->AuctionPreCheck((ENUM_CMDPACKET)0xbb, (PAY_TYPE)pay);
    if (preCheck >= 0)
        return preCheck;

    const unsigned int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x33);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbb, (unsigned char)(secuErr & 0xff));
        return 0;
    }

    if (pay == 1)
    {
        PCK_AUCTION_REGIST_CANCEL_GP pck;
        char* const body = reinterpret_cast<char*>(&pck);
        // ORIG [64]/[67]：`mov %eax,-0x39(%ebp)` / `mov %eax,-0x35(%ebp)`
        //   —— uid/characNo 具名 int 槽（我方可变体：直接 lea 链 ⇒ 帧 +0x10）。
        unsigned int uid = user->GetUID();
        int characNo = user->getCurCharacNo();
        *reinterpret_cast<unsigned int*>(body + 0x12) = uid;
        *reinterpret_cast<int*>(body + 0x16) = characNo;
        if (packet.get_binary(body + 0x1a, 8) != 1)
            return LineFunc(0xc4b1, __PRETTY_FUNCTION__, 0, 0);
        GlobalData::s_cera_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }
    else
    {
        PCK_AUCTION_REGIST_CANCEL_GA pck;
        char* const body = reinterpret_cast<char*>(&pck);
        // ORIG [97]/[100]：同上（GA 支复用 -0x39/-0x35 槽）。
        unsigned int uid = user->GetUID();
        int characNo = user->getCurCharacNo();
        *reinterpret_cast<unsigned int*>(body + 0x12) = uid;
        *reinterpret_cast<int*>(body + 0x16) = characNo;
        if (packet.get_binary(body + 0x1a, 8) != 1)
            return LineFunc(0xc4c4, __PRETTY_FUNCTION__, 0, 0);
        GlobalData::s_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }

    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xc4d4, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionRegistCancel",
                  user->getCurCharacNo());
            break;
        }
    }
    return 0;
