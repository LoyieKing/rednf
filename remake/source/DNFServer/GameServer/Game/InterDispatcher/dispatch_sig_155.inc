int Inter_AuctionResultBuyItemApiece::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultBuyItemApiecePktView* pPkt = reinterpret_cast<Inter_AuctionResultBuyItemApiecePktView*>(data);
    if (user == 0)
        return 0;

    if (pPkt->m_0 !=               // msg+0x0 characNo
        user->getCurCharacNo())
        return 0;

    for (int i = 0; i <= 4; ++i)                       // cmpl $4; setle
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3e12, 0)("Trace Auction Delay, %s(%d)",
                  "ACK AuctionResultBuyItemApiece",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0x14f);
    packet.put_byte(
        (int)pPkt->m_8);
    if (pPkt->m_8 == 0)
    {
        const int money = pPkt->m_4;  // msg+0x4
        CInventory* inven =
            static_cast<CInventory*>(user->getCurCharacInvenW());
        const int gained =
            inven->gain_money(money, (eMoneyAddReason)0x14, true, 0);
        if (gained < money)
        {
            user->SendMoneyFullReason(
                (ENUM_MONEY_FULL_REASON)0,
                (unsigned long)money, (unsigned long)gained);
        }
        packet.put_byte(
            (int)pPkt->m_9);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
