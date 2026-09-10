int Inter_MonitorEventItemUpdate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorEventItemUpdatePktView* pPkt = reinterpret_cast<Inter_MonitorEventItemUpdatePktView*>(data);
    // ORIG 0x84cefd2：m_1a/m_16 落槽（-0x40/-0x3c），m_e/m_12/NumberToString
    // 经 block-scope register 直落 %edi/%esi/%ebx 跨 cMyTrace ctor 缓存（P36）。
    unsigned int v1a = pPkt->m_1a;
    unsigned int v16 = pPkt->m_16;
    register unsigned int v0e = pPkt->m_e;
    register unsigned int v12 = pPkt->m_12;
    register char* mid = NumberToString(pPkt->m_a, 0);
    cMyTrace(__PRETTY_FUNCTION__,
        0x2bff, 0)("[%s][%d],이벤트 유저 DB ID : %s [EventType: %d][CharacNo: %d][ItemID: %d][Stack: %d]\n",
          __PRETTY_FUNCTION__,
          0x2bff,
          mid,
          v12,
          v0e,
          v16,
          v1a);
    WongWork::CWebEvent::RecvInfoFromWeb(
        pPkt->m_a,
        pPkt->m_e,
        pPkt->m_16,
        pPkt->m_1a,
        pPkt->m_12,
        pPkt->m_1e);
    return 0;
