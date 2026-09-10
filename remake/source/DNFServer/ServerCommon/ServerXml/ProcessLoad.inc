void CServerXml::ProcessLoad(TiXmlNode* pRootNode)
{
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("str");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <str> Tag Skip!!");
    }
    else
    {
        pRoot = pRoot->FirstChild("string");
        if (pRoot == (TiXmlNode*)0)
        {
            puts("[CServerXml] <string> Tag Error");
            exit(-1);
        }
        for (; pRoot != (TiXmlNode*)0; pRoot = pRoot->NextSibling())
        {
            int un_ID = 0;
            char* strTemp = (char*)0;
            char* cSentence = (char*)0;
            TiXmlNode* cNode = (TiXmlNode*)0;
            pRoot->ToElement()->Attribute("id", &un_ID);
            cNode = pRoot->FirstChild(m_cCharSet);
            if (cNode == (TiXmlNode*)0)
            {
                printf("%s Tag Error\n", this);
                return;
            }
            cSentence = (char*)cNode->ToElement()->Attribute("text");
            StrPunish(un_ID, cSentence, ENUM_SERVER_STRING);
        }
    }
