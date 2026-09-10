int Inter_BlackIPMonitor::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BlackIPMonitorPktView* pPkt = reinterpret_cast<Inter_BlackIPMonitorPktView*>(data);
    // [G2C_AuctionItemLimit 负结论] 残余 ndiff=67 / ratio 0.667（ORIG 0x084d86xx）：本 .inc 段
    //（函数头 + 循环头，ORIG[0:20]）已逐条对齐；全部残余落在循环体 —— ORIG 发
    //   `add %edx,%edx; add $0x1,%edx; shl $0x4,%edx; add %edx,%eax; add $0x10|0xc|0x8,%eax; mov (%eax),%e??`
    //（(2i+1)*0x10 + 条目基址 + 字段偏移全额进 add 的纯指针算术），我方产
    //   `add %eax,%eax; add $0x1,%eax; shl $0x4,%eax; lea (%edx,%eax,1),%eax; mov 0x10(%eax),%eax`
    //（末段字段偏移折叠进 disp）。
    // 该循环体及其 `#define pRec ((Inter_BlackIPMonitorRecWinView*)(pData + (i * 2 + 1) * 0x10))`
    // 位于聚合器 Game/InterDispatcher.cpp（3573 行起），**不在本灯写域（R1 禁改聚合器）** ⇒ 留 N。
    // ORIG 0x84d865f/0x84d8681 两次 `movl $0x0,-0x1c` ⇒ 原码 `int i = 0;` + `for (i = 0; …)`
    // 双初始化形（P191 配套）。
    int i = 0;
    if (pPkt->m_c84 != '\0')
        GlobalData::g_blackIPMonitor->reset();

    for (i = 0; i < pPkt->m_0; ++i)
    {
