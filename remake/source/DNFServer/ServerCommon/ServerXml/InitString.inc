void CServerXml::InitString()
{
    m_eCharSet = ENUM_CHARSET_KOR;
    memset(m_cCharSet, 0, sizeof(m_cCharSet));
    m_mapServerStr.clear();
    m_mapEventStartStr.clear();
    m_mapEventEndStr.clear();
    m_map_EventRGBA.clear();
