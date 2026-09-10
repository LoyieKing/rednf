unsigned int CUserCharacInfo::getCurCharacPayCoin() const
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetPayCoin();
    return 0;
