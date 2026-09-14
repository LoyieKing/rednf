void CServerXml::InitString()
{
    m_eCharSet = ENUM_CHARSET_KOR;
    memset(m_cCharSet, 0, sizeof(m_cCharSet));
    m_mapServerStr.clear();
    m_mapEventStartStr.clear();
    m_mapEventEndStr.clear();
    m_map_EventRGBA.clear();
#if defined(DNF_SVC_AUCTION) || defined(DNF_SVC_POINT)
    // ORIG df_auction_r InitString@0x0808f104 实测第 5 次 map clear（this+0xb8）
    auctionString_.clear();
#endif
}
