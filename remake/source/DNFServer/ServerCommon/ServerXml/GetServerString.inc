std::string CServerXml::GetServerString(int nTextID, bool* pbResult) const{
    std::string ret = "";
    std::map<int, std::string>::const_iterator pIter;
    pIter = m_mapServerStr.find(nTextID);
    if (pIter != m_mapServerStr.end())
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
