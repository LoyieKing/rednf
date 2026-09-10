void CServerXml::StrPunish(int nTextID, const char* pText, _eStringType type)
{
    if (pText == (const char*)0)
    {
        return;
    }
    std::string strText = pText;
    switch (type)
    {
    case ENUM_SERVER_STRING:
        m_mapServerStr.insert(std::pair<int, std::string>(nTextID, strText));
        break;
    case ENUM_EVENT_STRING_START:
        m_mapEventStartStr.insert(std::pair<int, std::string>(nTextID, strText));
        break;
    case ENUM_EVENT_STRING_END:
        m_mapEventEndStr.insert(std::pair<int, std::string>(nTextID, strText));
        break;
    }
}
