int Inter_NoticeGuildNameOnGuildCreate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticeGuildNameOnGuildCreatePktView* pPkt = reinterpret_cast<Inter_NoticeGuildNameOnGuildCreatePktView*>(data);
    STGuildProxy proxy;
    memcpy(&proxy, pPkt->m_pad16, 0x16);   // §4.3 具名数组基址替代裸偏移 (char*)pPkt+0x16
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)pPkt->m_2d
        )->InsertGuildInfoProxy(
            pPkt->m_12, proxy);
    return 0;
