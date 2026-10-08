// [b28d-lane2] verdict=DIFF ndiff=136（ORIG 0x084e7c90）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e7c90）：本轮 memcpy→0x1c 结构赋值（N84/P83，p167 CLEAR）；余 N82/P112 帧槽平移+P30/N4。
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
    // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x084e7d87..0x084e7d9d：GetUID →
    //   `mov %eax,-0x95(%ebp)`、getCurCharacNo → `mov %eax,-0x91(%ebp)`；pck 位于
    //   -0xa7，故二者是 pck+0x12 / pck+0x16（紧邻 +0x1a 的 m_name），随包发往
    //   cera auction proxy。旧注释「仅存栈未使用」误判：丢弃后包内 uid/charac_no 恒 0。
    pck.m_uid = target->GetUID();
    pck.m_characNo = target->getCurCharacNo();
    strncpy(pck.m_name, target->getCurCharacR()->m_name, 0xd);
    // [DNF-FIX 2026-10-08 c30] ORIG 0x084e7dce..0x084e7ddd：itemUid(needInfo+0x4) → pck+0x2b、price → pck+0x27。
    pck.m_itemUid = needInfo.m_itemUid;
    pck.m_price = price;

    PrePayInfo prePay;                                     // -0x74（0x28 按值）
    *reinterpret_cast<unsigned int*>(&prePay.m_data[0]) =
        reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_4;
    *reinterpret_cast<unsigned int*>(&prePay.m_data[4]) =
        reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_8;
    *reinterpret_cast<unsigned int*>(&prePay.m_data[8]) =
        static_cast<unsigned int>(price);
    *reinterpret_cast<Inter_OwnerisVIPTailView*>(&prePay.m_data[0x0c]) =
        *reinterpret_cast<const Inter_OwnerisVIPTailView*>(&needInfo.m_fee);

    const unsigned int characNo = target->getCurCharacNo();
    G_CPrePayInfoMgr()->InsertPrePayInfo(static_cast<int>(characNo), prePay);

    GlobalData::s_cera_auction_proxy->SendPacket(
        reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    return 0;
}