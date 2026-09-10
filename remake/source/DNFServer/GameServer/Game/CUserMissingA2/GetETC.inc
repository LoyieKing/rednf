int CUser::GetETC(short key)
{
    std::map<short, int>::iterator e = m_mapEtc.end();
    std::map<short, int>::iterator it = m_mapEtc.find(key);
    if (it == e)
        return 0;
    int val = m_mapEtc[key];
    m_mapEtc.erase(key);
    return val;
