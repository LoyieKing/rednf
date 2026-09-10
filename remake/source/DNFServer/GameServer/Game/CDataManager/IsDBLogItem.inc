int CDataManager::IsDBLogItem(unsigned int itemIdx)
{
    int cur = g_systemTimeInst.getCurSec();
    for (std::vector<stDBLogItem>::const_iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); it++)
    {
        if (it->m_itemIdx == itemIdx && cur <= it->m_time)
        {
            return it->m_value;
        }
    }
    return 0xffffffff;
