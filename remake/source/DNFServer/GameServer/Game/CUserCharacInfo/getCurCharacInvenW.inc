CInventory* CUserCharacInfo::getCurCharacInvenW()
{
    enableSaveInven();
    return &m_selected->m_curCharacInven;
}
