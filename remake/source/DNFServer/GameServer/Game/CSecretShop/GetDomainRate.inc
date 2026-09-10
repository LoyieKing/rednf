int SecretShopScript::GetDomainRate(stDungeonSales& sales)
{
    int total = 0;
    for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
         it != sales.m_sales.end(); ++it)
        total += it->second.m_rate;
    return total;
