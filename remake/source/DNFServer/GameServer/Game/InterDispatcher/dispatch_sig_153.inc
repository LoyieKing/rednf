int Inter_PointAuctionLogMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
    {
        return 0;
    }
    SIG_AUCTION_LOG_MESSAGE_PG* message =
        reinterpret_cast<SIG_AUCTION_LOG_MESSAGE_PG*>(data);
    user->m_historyLog.AuctionPoint(message);
    return 0;
