CInventory* CUserCharacInfo::getCurCharacInvenRefW()
{
    enableSaveInven();
    return &m_selected->m_curCharacInven;
}
