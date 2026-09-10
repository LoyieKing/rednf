void CInGameAdvertisementManager::InitADExposeInfo()
{
    for (std::multimap<short, CAdvertisementExposeInfo*>::iterator it =
             m_exposeInfo.begin();
         it != m_exposeInfo.end(); ++it)
    {
        CAdvertisementExposeInfo* info = it->second;
        delete info;
    }
    m_exposeInfo.clear();
