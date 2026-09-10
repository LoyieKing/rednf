int CServerXml::GetEventRGBA(int nEventID) const
{
    std::map<int, int>::const_iterator pIter;
    pIter = m_map_EventRGBA.find(nEventID);
    if (pIter != m_map_EventRGBA.end())
    {
        return pIter->second;
    }
    return 0;
