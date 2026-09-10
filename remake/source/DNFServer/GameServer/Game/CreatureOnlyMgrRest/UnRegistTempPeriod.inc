bool CCreatureMgr::UnRegistTempPeriod(int uid)
{
    std::map<int, int>::iterator it = m_field30.find(uid);
    if (it != m_field30.end())
    {
        m_field30.erase(uid);
        return true;
    }
    return false;
