bool CMailBox::FindPackageLoadLack(unsigned int id)
{
    if (id == 0)
        return 0;
    return std::find(m_loadLackList.begin(), m_loadLackList.end(), id)
               != m_loadLackList.end();
