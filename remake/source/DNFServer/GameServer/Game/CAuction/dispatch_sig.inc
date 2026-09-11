bool Timer_CheckAuctionReady::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    if (!GlobalData::s_auction_proxy->IsRunning())
    {
        GlobalData::s_auction_proxy->SendCheckAuctionReadyPacket();
    }
    if (!GlobalData::s_cera_auction_proxy->IsRunning())
    {
        GlobalData::s_cera_auction_proxy->SendCheckCeraAuctionReadyPacket();
    }
    return 1;
}