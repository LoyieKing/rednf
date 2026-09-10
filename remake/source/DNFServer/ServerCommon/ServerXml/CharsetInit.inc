void CServerXml::CharsetInit(TiXmlNode* pRootNode)
{
    const char* cTemp = (const char*)0;
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("option");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <option> Tag Error");
        exit(-1);
    }
    pRoot = pRoot->FirstChild("charset");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <charset> Tag Error");
        exit(-1);
    }
    cTemp = pRoot->ToElement()->Attribute("type");
    if (cTemp == (const char*)0)
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    if (strcmp(cTemp, "kor") == 0)
    {
        m_eCharSet = ENUM_CHARSET_KOR;
    }
    else if (strcmp(cTemp, "chn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_CHN;
    }
    else if (strcmp(cTemp, "jpn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_JPN;
    }
    else if (strcmp(cTemp, "usa") == 0)
    {
        m_eCharSet = ENUM_CHARSET_USA;
    }
    else if (strcmp(cTemp, "twn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_TWN;
    }
    else
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    strcpy(m_cCharSet, cTemp);
}
