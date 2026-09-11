CAuctionAveragePrice* CGameManager::GetAuctionAveragePriceManager()
{
    if (m_pAuctionAveragePriceMgr == 0)
    {
        m_pAuctionAveragePriceMgr = new (std::nothrow) CAuctionAveragePrice;
    }
    return m_pAuctionAveragePriceMgr;
}