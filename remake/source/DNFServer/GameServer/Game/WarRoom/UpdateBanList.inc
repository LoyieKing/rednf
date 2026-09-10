void WarRoom::UpdateBanList()
{
    int cur = GlobalData::s_systemTime_.getCurSec();
    std::map<unsigned int, int>::iterator it = m_map.begin();
    std::map<unsigned int, int>::iterator e = m_map.end();
    while (it != e)
    {
        int t = (*it).second;   // ORIG 用 operator*（de）
        if ((cur - t) >= G_CDataManager()->m_warAreaCounter.m_banInterval / 1000)
        {
            m_map.erase(it++);
        }
        else
        {
            ++it;
        }
    }
