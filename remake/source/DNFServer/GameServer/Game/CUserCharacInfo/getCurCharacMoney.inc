int CUserCharacInfo::getCurCharacMoney() const
{
    if (m_selected)
        return ((const CInventory*)getCurCharacInvenR())->get_money();
    return 0;
