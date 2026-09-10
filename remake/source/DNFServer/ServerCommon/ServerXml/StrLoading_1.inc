void CServerXml::StrLoading(std::string file)
{
    InitString();
    xmlFileName_ = file;
    m_cStrDoc.Clear();
    if (!m_cStrDoc.LoadFile(xmlFileName_, TIXML_ENCODING_UNKNOWN))
    {
        printf("[CServerXml] Load Fail File : %s\n", FILE_NAME);
    }
    else
    {
        TiXmlNode* pRootNode = m_cStrDoc.FirstChild("xml");
        if (pRootNode == (TiXmlNode*)0)
        {
            puts("[CServerXml] <xml> Tag Error");
        }
        else
        {
            CharsetInit(pRootNode);
            EventLoad(pRootNode);
            ProcessLoad(pRootNode);
        }
    }
}
