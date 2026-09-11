// ORIG 0x0821b098 W _ZN28DisPatcher_UdpCharacteristic13partyPingStatEP5CUserRtS2_
// （报告 DisPatcher_UdpCharacteristic/partyPingStat.md 汇编+C 全文转写，wave2）：
// Party_Ping 统计包（0xfa7,0xe）：+0xa=bad、+0xc=bad+total，经统计代理 SendPacket。
int DisPatcher_UdpCharacteristic::partyPingStat(CUser* user, unsigned short& bad, unsigned short& total)

{
    Packet_Party_Ping_Statistic pkt;
    pkt.m_dungeonBadPing = bad;                 // ORIG movzwl (%eax)
    pkt.m_dungeonTotal = (unsigned short)(bad + total);   // ORIG `mov 0x10(bad),%edx; mov 0x14(total),%eax; lea` bad 先
    // 同 pvpPingStat：链式调用不留 proxy 槽（ORIG `lea -0x16(%ebp)` 帧 0x24）。
    register char* pPkt = reinterpret_cast<char*>(&pkt);
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())->SendPacket(pPkt, 0xe);
    return 0;   // ORIG 0x0821b098 尾 `mov $0x0,%eax` ⇒ int 返回 0
}
