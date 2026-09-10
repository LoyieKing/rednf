int Dispatcher_AuctionAskAveragePrice::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08213940..0x08213e14 (0x4d5 字节)。GM 表 _ZL6gmList @0x093abb4c。
    // 包偏移（asm 权威）：GA/PCK_AUCTION_ASK_AVERAGE_PRICE_GA（ctor W 0x0822e4d8）
    //   uid@0x12, characNo@0x16, itemIdx@0x1a, flag@0x1e,
    //   循环 i=0..2: byte@0x1f+i, short@0x22+2i, char@0x2a+i, 尾 byte@0x2e；ROI sort@0x22。
    // GP/PCK_AUCTION_ASK_AVERAGE_PRICE_GP（ctor W 0x0822e94c）
    //   uid@0x12, characNo@0x16, itemIdx@0x1a, flag@0x1e。
    static const int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

    unsigned char payType = 0;
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc1e5, __PRETTY_FUNCTION__, 0, 0);
    if (payType > 1)
        return LineFunc(0xc1e8, __PRETTY_FUNCTION__, 0, 0);

    if (IsAuctionServerDisconnected(payType))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb9, 0x8c, (PAY_TYPE)(int)payType);
        return 0;
    }

    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return LineFunc(0xc207, __PRETTY_FUNCTION__, 0, 0);

    // GA 包 ctor + 头两字段
    PCK_AUCTION_ASK_AVERAGE_PRICE_GA pck;
    char* const body = reinterpret_cast<char*>(&pck);
    *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
    *reinterpret_cast<int*>(body + 0x16) = user->getCurCharacNo();

    unsigned int itemIdx = 0;
    if (packet.get_int(itemIdx) != 1)
        return LineFunc(0xc217, __PRETTY_FUNCTION__, 0, 0);
    *reinterpret_cast<unsigned int*>(body + 0x1a) = itemIdx;

    unsigned char flag = 0;
    if (packet.get_byte(flag) != 1)
        return LineFunc(0xc21a, __PRETTY_FUNCTION__, 0, 0);
    body[0x1e] = static_cast<char>(flag);

    if (payType == 1)
    {
        PCK_AUCTION_ASK_AVERAGE_PRICE_GP pckGp;   // ctor W 0x0822e94c
        char* const gp = reinterpret_cast<char*>(&pckGp);
        *reinterpret_cast<unsigned int*>(gp + 0x12) = user->GetUID();
        *reinterpret_cast<int*>(gp + 0x16) = user->getCurCharacNo();
        *reinterpret_cast<unsigned int*>(gp + 0x1a) = itemIdx;
        gp[0x1e] = body[0x1e];   // movzbl -0x35 / mov %al,-0x35（GA→GP 包 flag 拷贝）
        GlobalData::s_cera_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pckGp));
    }
    else
    {
        for (int i = 0; i <= 2; i++)   // cmpl $0x2 / setle
        {
            unsigned char upgrade = 0;
            if (packet.get_byte(upgrade) != 1)
                return LineFunc(0xc22f, __PRETTY_FUNCTION__, 0, 0);
            body[0x1f + i] = static_cast<char>(upgrade);

            short category = 0;
            if (packet.get_short(category) != 1)
                return LineFunc(0xc232, __PRETTY_FUNCTION__, 0, 0);
            *reinterpret_cast<short*>(body + 0x22 + i * 2) = category;

            char refine = 0;
            if (packet.get_byte(refine) != 1)
                return LineFunc(0xc235, __PRETTY_FUNCTION__, 0, 0);
            body[0x2a + i] = refine;
        }

        CItem* item = G_CDataManager()->find_item((int)itemIdx);
        if (item == 0)
            return LineFunc(0xc23a, __PRETTY_FUNCTION__, 0, 0);

        if (item->IsRandomOption())   // ORIG 虚调用 vptr+0x4c
        {
            for (int i = 0; i <= 2; i++)
            {
                short* const v = reinterpret_cast<short*>(body + 0x22 + i * 2);
                if (*v == 0)
                    *v = 0x7530;   // 30000
            }
        }

        pck.m_roi._sort();   // +0x22（GA 头 ROI_Category m_roi）

        unsigned char tail = 0;
        if (packet.get_byte(tail) != 1)
            return LineFunc(0xc247, __PRETTY_FUNCTION__, 0, 0);
        body[0x2e] = static_cast<char>(tail);

        GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }

    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xc258, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionAskAveragePrice",
                  user->getCurCharacNo());
            break;
        }
    }
    return 0;
