CInventory* CUserCharacInfo::getCurCharacInvenW()
{
    enableSaveInven();
    return (CInventory*)CUR->m_inven;
