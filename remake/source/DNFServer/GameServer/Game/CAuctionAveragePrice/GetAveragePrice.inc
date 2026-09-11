int CAuctionAveragePrice::GetAveragePrice(int itemID, unsigned char upgrade) const
{
    std::map<int, std::vector<std::pair<unsigned char, int> > >::const_iterator it =
        m_averagePriceMap.find(itemID);
    if (it != m_averagePriceMap.end())
    {
        std::vector<std::pair<unsigned char, int> >::const_iterator pit =
            std::find_if(it->second.begin(), it->second.end(), FindPrice(upgrade));
        if (pit != it->second.end())
        {
            return pit->second;
        }
    }
    return 0;
}