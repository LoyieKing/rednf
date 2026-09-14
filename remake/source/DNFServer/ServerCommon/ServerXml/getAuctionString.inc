std::string CServerXml::getAuctionString(int textNo, bool& result)
{
    std::map<int, std::string>::const_iterator findResult = auctionString_.find(textNo);
    if (findResult != auctionString_.end())
    {
        result = true;
        return findResult->second;
    }
    else
    {
        result = false;
        return std::string("");
    }
}
