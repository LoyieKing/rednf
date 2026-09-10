int Inter_BlackIPMonitor::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BlackIPMonitorPktView* pPkt = reinterpret_cast<Inter_BlackIPMonitorPktView*>(data);
    // ORIG 0x84d865f/0x84d8681 两次 `movl $0x0,-0x1c` ⇒ 原码 `int i = 0;` + `for (i = 0; …)`
    // 双初始化形（P191 配套）。
    int i = 0;
    if (pPkt->m_c84 != '\0')
        GlobalData::g_blackIPMonitor->reset();

    for (i = 0; i < pPkt->m_0; ++i)
    {
