int Dispatcher_AuctionBidding::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821522e..0x082159fb (0x7ce 字节)。GM 表 _ZL6gmList @0x093abb4c。
    // 包布局（ServerLab/Auction auction_header）：
    //   PCK_AUCTION_BIDDING_GA: uid@0x12, characNo@0x16, name[13]@0x1a, price(int)@0x27, auctionId[8]@0x2b
    //   PCK_AUCTION_ASK_OWNER_IS_VIP_GP: uid@0x12, auctionId[8]@0x16, characNo@0x1e, name[13]@0x22, price(uint)@0x2f, extra(int)@0x33

    unsigned char payType = 0;
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc554, __PRETTY_FUNCTION__, 0, 0);
    if (payType > 1)   // cmp $0x1 / jbe
        return LineFunc(0xc557, __PRETTY_FUNCTION__, 0, 0);

    const int pay = payType;   // movzbl
    if (pay == 1 && user->get_charac_level() < 0x14)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x74, (PAY_TYPE)pay);
        return 0;
    }

    const int preCheck = user->AuctionPreCheck((ENUM_CMDPACKET)0xbc, (PAY_TYPE)pay);
    if (preCheck >= 0)
        return preCheck;

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 5))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0xd1);
        return 0;
    }

    const unsigned int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x34);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, (unsigned char)(secuErr & 0xff),
                                 (PAY_TYPE)pay);
        return 0;
    }

    if (pay == 1)
    {
        unsigned int price = 0;   // -0x44
        if (packet.get_int(price) != 1)
            return LineFunc(0xc5a9, __PRETTY_FUNCTION__, 0, 0);

        if ((unsigned int)user->GetCera() < price)   // asm setb（无符号比较）
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x90, (PAY_TYPE)pay);
            return 0;
        }

        if (G_CPrePayInfoMgr()->FindPrePayInfo(user->getCurCharacNo()) != 0
            || user->GetBuyingGold())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x9e, (PAY_TYPE)pay);
            return 0;
        }

        char auctionId[8];   // -0x74..-0x6d（get_binary 目标 = -0x78+4）
        if (packet.get_binary(auctionId, 8) != 1)
            return LineFunc(0xc5bb, __PRETTY_FUNCTION__, 0, 0);
        const unsigned int priceCopy = price;   // ORIG mov -0x44 → -0x78

        int extra = 0;   // -0x48
        if (packet.get_int(extra) != 1)
            return LineFunc(0xc5c1, __PRETTY_FUNCTION__, 0, 0);

        // PCK_AUCTION_ASK_OWNER_IS_VIP_GP ctor ORIG W 0x0822ebd8
        PCK_AUCTION_ASK_OWNER_IS_VIP_GP pck;
        char* const body = reinterpret_cast<char*>(&pck);
        *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
        memcpy(body + 0x16, auctionId, 8);
        *reinterpret_cast<int*>(body + 0x1e) = user->getCurCharacNo();
        // [G1-Dispatchers] ORIG 该处为 `call getCurCharacR; strncpy(dst,(char*)(%eax+4),0xd)`
        // ⇒ 原码直取 `_Charac_info::m_name`（+0x04），不经 getCurCharacName 的
        //   `m_selected ? … : ""` 防御（p167 EXTRA getCurCharacName x2 之一）。
        strncpy(body + 0x22, user->getCurCharacR()->m_name, 0xd);
        *reinterpret_cast<unsigned int*>(body + 0x2f) = price;
        *reinterpret_cast<int*>(body + 0x33) = extra;
        GlobalData::s_cera_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pck));

        user->SetBuyingGold(true);
        (void)priceCopy;
        return 0;
    }

    static const int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

    // ---- gold 分支：PCK_AUCTION_BIDDING_GA ctor ORIG W 0x0822e62c ----
    PCK_AUCTION_BIDDING_GA pck;
    char* const body = reinterpret_cast<char*>(&pck);
    *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
    *reinterpret_cast<int*>(body + 0x16) = user->getCurCharacNo();
    strncpy(body + 0x1a, user->getCurCharacR()->m_name, 0xd);   // 同上（ORIG getCurCharacR+0x04）

    int price = 0;   // 读入包 +0x27
    if (packet.get_int(price) != 1)
        return LineFunc(0xc642, __PRETTY_FUNCTION__, 0, 0);
    *reinterpret_cast<int*>(body + 0x27) = price;

    char auctionId[8];   // 包 +0x2b（get_binary 直接写包内）
    if (packet.get_binary(body + 0x2b, 8) != 1)
        return LineFunc(0xc643, __PRETTY_FUNCTION__, 0, 0);

    // [G1-Dispatchers] P214③：ORIG 无 `call memset`（p167 我方 EXTRA memset ×1）⇒
    // 改零初始化形，4.4 对 `char[N] = {0}` 内联 rep stos。
    char buyerName[14] = {0};   // -0x42，13B 有效 + pad，读取前清零
    if (packet.get_binary(buyerName, 0xd) != 1)
        return LineFunc(0xc648, __PRETTY_FUNCTION__, 0, 0);

    if (user->getCurCharacMoney() < price)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x90, (PAY_TYPE)pay);
        return 0;
    }

    if (!user->getCurCharacInvenW()->use_money(price, (eMoneySubReason)0x1a, true))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x93, (PAY_TYPE)pay);
        return 0;
    }

    user->SaveMoneyCoin();
    GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));

    if (4999 < price)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp",
                                                            0xc678);
        CStreamGuard guard(stream, true);
        **guard << 0x309;
        **guard << (int)user->GetUID();
        SIG_AUCTION_MONEY_SUB* sig = guard->GetInBuffer<SIG_AUCTION_MONEY_SUB>();
        memset(sig, 0, 0x28);
        sig->m_arg04 = price;
        strncpy(sig->m_name, buyerName, 0xd);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }

    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xc686, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionBidding", user->getCurCharacNo());
            break;
        }
    }
    return 0;
