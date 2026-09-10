int Inter_AuctionSubMoney::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionSubMoneyPktView* pPkt = reinterpret_cast<Inter_AuctionSubMoneyPktView*>(data);
    if (user != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x2d3, 1,
            pPkt->m_0,
            pPkt->m_4);
    }
    return 0;
