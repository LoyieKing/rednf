// ===== Dispatcher_GuildCargoPopItem::process ===== ORIG 0x081d15b2 (0x195)
// 报告 Dispatcher_GuildCargoPopItem/process.md 全文转写。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e2bde orig/ours=185/179 ndiff-blocks=30
//   帧差 sub $fc vs $dc + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0xfc,%esp | mov    %eax,-0x1c(%ebp) | lea    -0x9b(%ebp),%eax
// R28p2p §34.2.1（实参符号性轴，2026-09-13）：trace2/trace4 的 min/max/avg 六处实参补 (short)
//   ⇒ 复现 ORIG 的 movzwl+cwtl / movzwl+movswl %ax,%esi 符号扩展（原先只有单条 movzwl 零扩展）。
int Dispatcher_P2P_Statistics::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;                                                // ORIG 不读写 param
    MSG_P2P_STATISTICS& req = reinterpret_cast<MSG_P2P_STATISTICS&>(msg);

    Packet_P2P_Statistics pkt;
    pkt.m_p2pUser = (int)(signed char)req.m_0d - (int)(signed char)req.m_0e;  // +0x0a
    pkt.m_relayUser = (int)(signed char)req.m_0e;               // +0x0e
    pkt.m_serverGroup = (unsigned char)user->GetServerGroup();  // +0x12（GetServerGroup @0x080cbc90）
    pkt.m_p2pMinPing = req.m_10;                                // +0x13 ← msg+0x10
    pkt.m_p2pMaxPing = req.m_12;                                // +0x15 ← msg+0x12
    pkt.m_p2pAvgPing = req.m_14;                                // +0x17 ← msg+0x14
    pkt.m_p2pOverPing100 = req.m_18;                            // +0x19 ← msg+0x18
    pkt.m_p2pOverPing200 = req.m_1c;                            // +0x1d ← msg+0x1c
    pkt.m_p2pOverPing300 = req.m_20;                            // +0x21 ← msg+0x20
    pkt.m_p2pOverPing400 = req.m_24;                            // +0x25 ← msg+0x24
    pkt.m_relayMinPing = req.m_28;                              // +0x29 ← msg+0x28
    pkt.m_relayMaxPing = req.m_2a;                              // +0x2b ← msg+0x2a
    pkt.m_relayAvgPing = req.m_2c;                              // +0x2d ← msg+0x2c
    pkt.m_relayOverPing100 = req.m_30;                          // +0x2f ← msg+0x30
    pkt.m_relayOverPing200 = req.m_34;                          // +0x33 ← msg+0x34
    pkt.m_relayOverPing300 = req.m_38;                          // +0x37 ← msg+0x38
    pkt.m_relayOverPing400 = req.m_3c;                          // +0x3b ← msg+0x3c

    cMyTrace trace1(__PRETTY_FUNCTION__,
                    0x5953, 0);
    trace1("p2p statistics");

    cMyTrace trace2(__PRETTY_FUNCTION__,
                    0x5954, 0);
    trace2("num(%d) min(%d) max(%d), avg(%d), 100(%u), 200(%u), 300(%u), 400(%u)",
           (int)pkt.m_p2pUser,                                  // num=diff（ORIG ebx=-0x91）
           (int)(short)pkt.m_p2pMinPing,                        // min（ORIG 0x081e2d48 movzwl; 0x081e2d4f movswl %ax,%esi）
           (int)(short)pkt.m_p2pMaxPing,                        // max（ORIG 0x081e2d3a movzwl; 0x081e2d41 cwtl）
           (int)(short)pkt.m_p2pAvgPing,                        // avg（ORIG 0x081e2d2c movzwl; 0x081e2d33 cwtl）
           (unsigned int)pkt.m_p2pOverPing100, (unsigned int)pkt.m_p2pOverPing200,
           (unsigned int)pkt.m_p2pOverPing300, (unsigned int)pkt.m_p2pOverPing400);

    cMyTrace trace3(__PRETTY_FUNCTION__,
                    0x595e, 0);
    trace3("relay statistics");

    cMyTrace trace4(__PRETTY_FUNCTION__,
                    0x595f, 0);
    trace4("num(%d) min(%d) max(%d), avg(%d), 100(%u), 200(%u), 300(%u), 400(%u)",
           (int)pkt.m_relayUser,                                // num（ORIG ebx=-0x8d=(char)msg+0x0e）
           (int)(short)pkt.m_relayMinPing,                      // min（ORIG 0x081e2e36 movzwl; 0x081e2e3a movswl %ax,%esi）
           (int)(short)pkt.m_relayMaxPing,                      // max（ORIG 0x081e2e2b movzwl; 0x081e2e2f cwtl）
           (int)(short)pkt.m_relayAvgPing,                      // avg（ORIG 0x081e2e20 movzwl; 0x081e2e24 cwtl）
           (unsigned int)pkt.m_relayOverPing100, (unsigned int)pkt.m_relayOverPing200,
           (unsigned int)pkt.m_relayOverPing300, (unsigned int)pkt.m_relayOverPing400);

    // [DispA2 P0/P183] ORIG 0x081e2ecd `call GetServerProxy` 回值在 0x081e2ede 直接
    //   `mov %eax,(%esp)` 作 SendPacket receiver，无 proxy(-0x1c) 具名槽。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket((char*)&pkt, 0x3f);                        // CStatisticServerProxy::SendPacket 0x08472044
    return 0;
}