int Inter_BlackIPMonitorDeleteIP::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BlackIPMonitorDeleteIPPktView* pPkt = reinterpret_cast<Inter_BlackIPMonitorDeleteIPPktView*>(data);
    unsigned int i = 0;                                // ORIG 0x084d87f6 预清 -0xc + 循环前再清零（P191 双初始化）
    cMyTrace(__PRETTY_FUNCTION__, 0x3f6d, 0)("Delete IP Cnt : %d", pPkt->m_a);

    for (i = 0; i < pPkt->m_a; ++i)
    {
        // 逐处内联（P0：ORIG 除 pPkt 外无条目/取值具名槽往返）。
        if (GlobalData::g_blackIPMonitor->deleteIP(
                reinterpret_cast<char*>(pPkt) + i * 0x14 + 0xe,
                reinterpret_cast<Inter_BlackIPMonitorDeleteIPRecLeafView*>(
                    reinterpret_cast<char*>(pPkt) + i * 0x14 + 0x10)->m_val) != '\0')
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x3f76, 0)(
                "Delete IP: %s, Type: %d",
                reinterpret_cast<char*>(pPkt) + i * 0x14 + 0xe,
                reinterpret_cast<Inter_BlackIPMonitorDeleteIPRecLeafView*>(
                    reinterpret_cast<char*>(pPkt) + i * 0x14 + 0x10)->m_val);
        }
    }
    return 0;
}