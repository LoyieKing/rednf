int Inter_MonitorReplyBuddyList::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorReplyBuddyListPktView* pPkt = reinterpret_cast<Inter_MonitorReplyBuddyListPktView*>(data);
    G_GameWorld()->send_buddy_conn_list(
        pPkt->m_a,
        &pPkt->m_list,
        (int)(unsigned char)pPkt->m_e);
    return 0;
}