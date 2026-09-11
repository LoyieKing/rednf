CInventory* CUserCharacInfo::getCurCharacInvenRefW()
{
    enableSaveInven();
    return (CInventory*)CUR->m_inven;
}