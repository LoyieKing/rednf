int Inter_MonitorReplyBuddyList::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorReplyBuddyListPktView* pPkt = reinterpret_cast<Inter_MonitorReplyBuddyListPktView*>(data);
    G_GameWorld()->send_buddy_conn_list(
        pPkt->m_accId,
        &pPkt->m_list,
        (int)(unsigned char)pPkt->m_count);
    return 0;
}