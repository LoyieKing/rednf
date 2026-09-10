void CAssaultMgr::FrocedCloseAssaults()
{
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.begin();
    while (it != m_places.end())
    {
        CAssaultPlace* place = it->second;
        if (place != 0)
        {
            place->EndAssault();
        }
        ++it;
    }
