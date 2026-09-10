bool CCreatureMgr::RegistTempPeriod(int a, int b, int c)
{
    (void)c;
    std::map<int, int>::iterator it = m_field30.find(a);
    if (it != m_field30.end())
        return false;
    m_field30.insert(std::make_pair(a, b));
    return true;
