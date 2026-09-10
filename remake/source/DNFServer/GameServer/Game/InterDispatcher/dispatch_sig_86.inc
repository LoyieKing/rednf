int Inter_MonitorForbidChat::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorForbidChatPktView* pPkt = reinterpret_cast<Inter_MonitorForbidChatPktView*>(data);
    cMyTrace(__PRETTY_FUNCTION__, 0x2e96, 0)("Inter_MonitorForbidChat::dispatch_sig recv\n");

    const size_t nameLen = pPkt->m_e;
    char name[30];                                              // ORIG call memset 清零
    memset(name, 0, sizeof(name));
    strncpy(name, pPkt->m_12, nameLen);

    if (!G_GameWorld()->IsForbiddenChat(name))
    {
        G_GameWorld()->InsertUserToIDMap(name, 0);
        const int secs = pPkt->m_a;
        G_GameWorld()->DisableChat(name, secs);
        cMyTrace(__PRETTY_FUNCTION__, 0x2ea2, 0)("Inter_MonitorForbidChat::dispatch_sig %s for %d secs\n", name, secs);
    }
    return 0;
