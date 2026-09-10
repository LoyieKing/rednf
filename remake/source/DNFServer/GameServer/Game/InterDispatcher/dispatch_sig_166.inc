int Inter_BlackIPMonitorDeleteIP::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BlackIPMonitorDeleteIPPktView* pPkt = reinterpret_cast<Inter_BlackIPMonitorDeleteIPPktView*>(data);
    unsigned int i = 0;                                // ORIG 0x084d87f6 预清 -0xc + 循环前再清零（P191 双初始化）
    cMyTrace(__PRETTY_FUNCTION__, 0x3f6d, 0)("Delete IP Cnt : %d", pPkt->m_a);

    for (i = 0; i < pPkt->m_a; ++i)
    {
        if (GlobalData::g_blackIPMonitor->deleteIP(data + i * 0x14 + 0xe,
                                                   ((Inter_BlackIPMonitorDeleteIPRecWinView*)(data + i * 0x14))->m_1e) != '\0')
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x3f76, 0)(
                "Delete IP: %s, Type: %d", data + i * 0x14 + 0xe,
                ((Inter_BlackIPMonitorDeleteIPRecWinView*)(data + i * 0x14))->m_1e);
        }
    }
    return 0;
