int Inter_OwnerisVIP::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b28d-lane2] ORIG 084e7ca4 段只有一个 data 派生槽（-0x14，`cmp $0,-0x14(%ebp)` 判空
    // + 每处访问 `mov -0x14(%ebp),%eax` 重载）⇒ 原码单具名 msg，其余视图逐点内联
    // （P183/P0：旧 pPkt/pWin_msg 双缓存各占 4B 并多发 8 条 lea/6 条 mov）。
    char* msg = data;                                      // -0x14（ORIG cmpl $0,-0x14 检查 data）
    if (msg == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6aa6, 5)("[Check point auction error] internal data is null.");
        return 0x6aa7;
    }

    CUser* target = user;                                  // -0x10
    const int price =                                      // -0xc（ORIG 先物化 price 再填 needInfo）
        reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_22;
    ItemVendingMachine::AuctionNeedInfo needInfo;          // -0x4c（0x28）
    needInfo.m_price = static_cast<unsigned int>(price);
    needInfo.m_itemUid = reinterpret_cast<Inter_OwnerisVIPMsgWinView*>(msg)->m_4;   // lo/hi

    if (!GlobalData::s_pItemVendingMachine->BuyAuctionItem(
            target, needInfo,
            reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_26,           // +0x26
            reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_c,          // +0x0c
            reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_d))          // +0x0d
    {
        target->SetBuyingGold(false);
        target->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x93, (PAY_TYPE)1);
        return 0;
    }

    PCK_AUCTION_BIDDING_GP pck;                            // -0xa7（0x33，ctor W 0x084e9a98）
    // ORIG：GetUID/getCurCharacNo 各调用一次，结果仅存栈（-0x95/-0x91）未使用——
    // 保留死调用以对齐 ORIG 指令序列（pck 除 characName 外保持 ctor 清零）。
    (void)target->GetUID();
    (void)target->getCurCharacNo();
    strncpy(reinterpret_cast<char*>(&pck) + 0x1a,
            reinterpret_cast<const char*>(target->getCurCharacR()) + 4, 0xd);

    PrePayInfo prePay;                                     // -0x74（0x28 按值）
    *reinterpret_cast<unsigned int*>(prePay.m_data + 0x00) =
        reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_4;
    *reinterpret_cast<unsigned int*>(prePay.m_data + 0x04) =
        reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_8;
    *reinterpret_cast<unsigned int*>(prePay.m_data + 0x08) =
        static_cast<unsigned int>(price);
    *reinterpret_cast<Inter_OwnerisVIPTailView*>(prePay.m_data + 0x0c) =
        *reinterpret_cast<const Inter_OwnerisVIPTailView*>(
            reinterpret_cast<const char*>(&needInfo) + 0x0c);      // needInfo+0xc..+0x27

    const unsigned int characNo = target->getCurCharacNo();
    G_CPrePayInfoMgr()->InsertPrePayInfo(static_cast<int>(characNo), prePay);

    GlobalData::s_cera_auction_proxy->SendPacket(
        reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    return 0;
