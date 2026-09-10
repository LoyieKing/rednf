int Dispatcher_AuctionBuyItemApiece::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08214e44: AuctionPreCheck(0x14f, PAY_TYPE 0)
    int result = user->AuctionPreCheck(static_cast<ENUM_CMDPACKET>(0x14f),
                                       static_cast<PAY_TYPE>(0));
    if (result >= 0)
        return result;

    // +0x03a: PCK_AUCTION_BUY_ITEM_APIECE_GA 请求包
    PCK_AUCTION_BUY_ITEM_APIECE_GA pck;
    // +0x045: pck+0x12 = GetUID；pck+0x16 = getCurCharacNo
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&pck) + 0x12) = user->GetUID();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pck) + 0x16) = user->getCurCharacNo();

    int price = 0;
    if (!packet.get_int(price))
    {
        return LineFunc(0xc501, __PRETTY_FUNCTION__, 0, 0);
    }
    // pck+0x26 = 竞拍单价/件数 int
    if (!packet.get_int(*reinterpret_cast<int*>(pck.m_body.m_count)))
    {
        return LineFunc(0xc502, __PRETTY_FUNCTION__, 0, 0);
    }
    // pck+0x1e = char[8] 竞拍键
    if (!packet.get_binary(reinterpret_cast<char*>(&pck) + 0x1e, 8))
    {
        return LineFunc(0xc503, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x139: char itemKey[13] 清零后读 0xd 字节
    char itemKey[13];
    itemKey[0] = 0;  itemKey[1] = 0;  itemKey[2] = 0;  itemKey[3] = 0;
    itemKey[4] = 0;  itemKey[5] = 0;  itemKey[6] = 0;  itemKey[7] = 0;
    itemKey[8] = 0;  itemKey[9] = 0;  itemKey[10] = 0; itemKey[11] = 0;
    itemKey[12] = 0;
    if (!packet.get_binary(itemKey, 13))
    {
        return LineFunc(0xc508, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x1a0: 余额不足 → error 0xbc,0x90
    if (user->getCurCharacMoney() < price)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xbc), 0x90);
        return 0;
    }

    // +0x1fc: use_money(price, 0x1a, true)
    if (!(user->getCurCharacInvenW()->use_money(
              price, static_cast<eMoneySubReason>(0x1a), true)))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xbc), 0x93);
        return 0;
    }

    // +0x233: SaveMoneyCoin → 发 GA 包
    user->SaveMoneyCoin();
    GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));

    // +0x24f: price > 4999 → 统计流 SIG_AUCTION_MONEY_SUB
    if (price > 4999)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xc529);
        CStreamGuard guard(stream, true);
        **guard << 0x309;
        **guard << static_cast<int>(user->GetUID());
        SIG_AUCTION_MONEY_SUB* sig = guard->GetInBuffer<SIG_AUCTION_MONEY_SUB>();
        memset(sig, 0, 0x28);
        *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 4) = price;
        strncpy(reinterpret_cast<char*>(sig) + 8, itemKey, 13);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    }

    return 0;
