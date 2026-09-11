void CInventory::UpdateDailyCoin(int value)
{
    if ((int)m_coin < value)
    {
        m_coin = value;
        ((CUserCharacInfo*)m_pParent)->enableSaveInven();
    }
}