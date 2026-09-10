void CServerXml::EventLoad(TiXmlNode* pRootNode)
{
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("event_str");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <event_str> Tag Skip!!");
    }
    else
    {
        pRoot = pRoot->FirstChild("event");
        if (pRoot == (TiXmlNode*)0)
        {
            puts("[CServerXml] <event> Tag Error");
            exit(-1);
        }
        for (; pRoot != (TiXmlNode*)0; pRoot = pRoot->NextSibling())
        {
            int un_ID = 0;
            char* strTemp = (char*)0;
            char* cSentence = (char*)0;
            TiXmlNode* cNode = (TiXmlNode*)0;
            pRoot->ToElement()->Attribute("id", &un_ID);
            RGBALoad(un_ID, pRoot);
            cNode = pRoot->FirstChild(m_cCharSet);
            if (cNode == (TiXmlNode*)0)
            {
                printf("%s Tag Error\n", this);
                exit(-1);
            }
            cSentence = (char*)cNode->ToElement()->Attribute("start_msg");
            StrPunish(un_ID, cSentence, ENUM_EVENT_STRING_START);
            cSentence = (char*)cNode->ToElement()->Attribute("end_msg");
            StrPunish(un_ID, cSentence, ENUM_EVENT_STRING_END);
        }
    }
