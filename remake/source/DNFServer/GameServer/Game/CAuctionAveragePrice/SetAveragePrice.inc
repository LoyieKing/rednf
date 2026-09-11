void CAuctionAveragePrice::SetAveragePrice(int itemID, unsigned char upgrade, int price)
{
    std::map<int, std::vector<std::pair<unsigned char, int> > >::iterator it =
        m_averagePriceMap.find(itemID);
    if (it != m_averagePriceMap.end())
    {
        std::pair<unsigned char, int> p(upgrade, price);
        it->second.push_back(p);
    }
    else
    {
        std::vector<std::pair<unsigned char, int> > vec;
        std::pair<unsigned char, int> p(upgrade, price);
        vec.push_back(p);
        m_averagePriceMap.insert(std::make_pair(itemID, vec));
    }
}