unsigned int CUserCharacInfo::getCurCharacCoin() const
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetCoin();
    return 0;
