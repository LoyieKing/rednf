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
#if defined(DNF_SVC_AUCTION) || defined(DNF_SVC_POINT)
            // ORIG df_auction_r StrLoading(std::string)@0x0808f20e 实测在
            // ProcessLoad 之后 call auctionStringLoad@0x0808fb50
            auctionStringLoad(pRootNode);
#endif
        }
    }
}
