std::string CServerXml::GetEventString(int nTextID, _eStringType type, bool* pbResult) const{
    std::string ret = "";
    std::map<int, std::string>::const_iterator pIter;
    switch (type)
    {
    case ENUM_EVENT_STRING_START:
        pIter = m_mapEventStartStr.find(nTextID);
        if (pIter != m_mapEventStartStr.end())
        {
            if (pbResult != (bool*)0)
            {
                *pbResult = true;
            }
        }
        else
        {
            if (pbResult != (bool*)0)
            {
                *pbResult = false;
            }
            return ret;
        }
        return pIter->second;
    case ENUM_EVENT_STRING_END:
        pIter = m_mapEventEndStr.find(nTextID);
        if (pIter != m_mapEventEndStr.end())
        {
            if (pbResult != (bool*)0)
            {
                *pbResult = true;
            }
        }
        else
        {
            if (pbResult != (bool*)0)
            {
                *pbResult = false;
            }
            return ret;
        }
        return pIter->second;
    default:
        return ret;
    }
}
