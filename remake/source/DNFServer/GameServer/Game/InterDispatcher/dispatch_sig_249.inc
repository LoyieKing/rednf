// [b28d-lane2] verdict=DIFF ndiff=65（ORIG 0x084e2dee）：槽序/帧差(N82/P212/P221)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=53（ORIG 0x84e2dee）：P226 具名 bool/char 落槽+N18a 单段 regmove+N82/P112 帧槽平移。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→0（71=71，TU 121/1331/5/133 不回退）：真修——
//   删 serverKind/channelNo 具名局部（P0 表达式直用），三个 cMyTrace 改全表达式临时链（P221/P227：
//   `cMyTrace(f,l,5)(fmt, G_CEnvironment()->..., (unsigned char)pPkt->m_a)` 使两实参在 ctor 前分别落 %esi/%ebx）。
//   残余=比较宽度/块布局（ORIG 提升 int `test %eax,%eax; cmp $1,%eax` + je 落 then vs 4.4 8 位 test/jne，P47/P222）。
int Inter_MonitorRegistGameServerAck::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorRegistGameServerAckPktView* pPkt = reinterpret_cast<Inter_MonitorRegistGameServerAckPktView*>(data);

    if (pPkt->m_b == '\x01')
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x5308, 5)(
            "Channel NO %d is already registerd(server type : %d). use another channel NO",
            G_CEnvironment()->m_serverEnvir.m_channelNo,
            (unsigned int)(unsigned char)pPkt->m_a);
        GetInstanceShutdowManager()->SendShutdownSignal();
    }
    else
    {
        // [b28d-r5] ORIG 0x084e2e40 段 `movzbl 0xa(%eax),%eax; movzbl %al,%eax;
        //   test %eax,%eax; je <then0>; cmp $0x1,%eax; je <then1>` = 值只读一次、
        //   int 提升后比较链（P251/P261：无栈槽 + cmp 链 ⇒ 源形是 switch，if-else 链
        //   恒退化为逐次重读 + 8 位 test/jne）。
        switch ((int)(unsigned char)pPkt->m_a)
        {
        case 0:
            cMyTrace(__PRETTY_FUNCTION__, 0x5312, 9, true, true)(
                "[!] Monitor Server Connected");
            break;
        case 1:
            cMyTrace(__PRETTY_FUNCTION__, 0x5317, 9, true, true)(
                "[!] Guild Server Connected");
            break;
        }
    }
    return 0;
}