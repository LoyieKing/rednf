// [b28d-lane2] verdict=DIFF ndiff=100（ORIG 0x084d8650）：槽序/帧差(N82/P212/P221)+混合残差；gap+22。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=126（ORIG 0x84d8650）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-solo2] P0 重判：ORIG 4 个字段值仅 endTime 落 spill 槽(-0x3c)、startTime/idCnt/punishType
//   走 %edi/%esi/%ebx callee-saved ⇒ 原码无 4 个具名局部（全内联实参，右到左求值序
//   m_20→m_1c→m_18→m_14 与 ORIG 逐槽一致）；循环后取值用循环变量 `i-1`（ORIG 0x84d8739
//   `lea -0x1(%eax),%edx` 直证，非重读 m_0-1）；trace 用全表达式临时链使全局值跨 ctor 保活
//   （ORIG 0x84d8753 `mov &g_...,%ebx` 在 ctor 之前，P328⑩/P227）。
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

    for (i = 0; i < pPkt->m_count; ++i)
    {
        Inter_BlackIPMonitorRec* recs =
            reinterpret_cast<Inter_BlackIPMonitorRec*>(pPkt);
        GlobalData::g_blackIPMonitor->insertNewIP(
            inet_addr(recs[i].m_ip),
            (WongWork::ENUM_BLACKIPMONITOR_PUNISH_TYPE)recs[i].m_punishType,
            recs[i].m_start, recs[i].m_c,
            *reinterpret_cast<long*>(&recs[i + 1]));
    }

    GlobalData::g_IPMonitorLastDBAccessTime =
        reinterpret_cast<Inter_BlackIPMonitorRec*>(pPkt)[i - 1].m_c;
    cMyTrace(__PRETTY_FUNCTION__, 0x3f5b, 0)(
        "IPMonitorPunish LAST LOAD TIME : %d",
        GlobalData::g_IPMonitorLastDBAccessTime);
    return 0;
}