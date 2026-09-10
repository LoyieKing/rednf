int Inter_AuctionResultBidding::dispatch_sig(CUser* user, char* data, int len)
{
    if (user == 0)
        return 0;
    return dispatch_sig_taiwan(user, data, len);
