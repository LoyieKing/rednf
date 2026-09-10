void CServerXml::RGBALoad(int eventID, TiXmlNode* pRootNode)
{
    TiXmlNode* cNode = (TiXmlNode*)0;
    cNode = pRootNode->FirstChild("color");
    if (cNode == (TiXmlNode*)0)
    {
        printf("%s Tag Error\n", this);
        exit(-1);
    }
    union uniRGBA
    {
        int color;
        char c[4];
    };
    uniRGBA color;
    color.color = 0;
    color.c[0] = (char)atoi(cNode->ToElement()->Attribute("red"));
    color.c[1] = (char)atoi(cNode->ToElement()->Attribute("green"));
    color.c[2] = (char)atoi(cNode->ToElement()->Attribute("blue"));
    color.c[3] = (char)atoi(cNode->ToElement()->Attribute("alpha"));
    m_map_EventRGBA.insert(std::pair<int, int>(eventID, color.color));
}
