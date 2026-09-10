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
