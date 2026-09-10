unsigned int CUserCharacInfo::getCurCharacEventCoin() const
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->GetEventCoin();
    return 0;
