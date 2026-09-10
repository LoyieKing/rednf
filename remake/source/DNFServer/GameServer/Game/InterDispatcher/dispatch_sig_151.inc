int Inter_AuctionLogMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
    {
        return 0;
    }
    SIG_AUCTION_LOG_MESSAGE_AG* message =
        reinterpret_cast<SIG_AUCTION_LOG_MESSAGE_AG*>(data);
    user->m_historyLog.Auction(message);
    return 0;
