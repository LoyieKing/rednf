void CServerXml::auctionStringLoad(TiXmlNode* rootNode)
{
    TiXmlNode* xmlNode = (TiXmlNode*)0;
    xmlNode = rootNode->FirstChild("auction");
    if (xmlNode == (TiXmlNode*)0)
    {
        puts("[CServerXml] <auction> Tag Skip!!");
    }
    else
    {
        xmlNode = xmlNode->FirstChild("string");
        if (xmlNode != (TiXmlNode*)0)
        {
            for (; xmlNode != (TiXmlNode*)0; xmlNode = xmlNode->NextSibling())
            {
                int textNo = 0;
                const char* stringTemp = (const char*)0;
                TiXmlNode* localeNode = (TiXmlNode*)0;
                xmlNode->ToElement()->Attribute("id", &textNo);
                localeNode = xmlNode->FirstChild(m_cCharSet);
                if (localeNode == (TiXmlNode*)0)
                {
                    printf("%s Tag Error\n", this);
                    return;
                }
                stringTemp = localeNode->ToElement()->Attribute("text");
                if (auctionString_.find(textNo) != auctionString_.end())
                {
                    printf("%s Tag Invalid text id : %d\n", this, textNo);
                    exit(-1);
                }
                if (!auctionString_.insert(std::pair<int, std::string>(textNo, stringTemp)).second)
                {
                    printf("%s Tag Invalid text : %d, %s\n", this, textNo, stringTemp);
                    exit(-1);
                }
            }
        }
    }
}
