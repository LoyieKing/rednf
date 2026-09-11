// ORIG 0x0821b02a W _ZN28DisPatcher_UdpCharacteristic15fairPvpPingStatEP5CUserRtS2_
// （报告 fairPvpPingStat.md 全文转写，wave2）：Fair_Pvp_Ping 包（0x27ed,0xe）同型。
int DisPatcher_UdpCharacteristic::fairPvpPingStat(CUser* user, unsigned short& bad, unsigned short& total)

{
    Packet_Fair_Pvp_Ping_Statistic pkt;
    pkt.m_badPing = bad;
    pkt.m_total = (short)(total + bad);
    // 同 pvpPingStat：链式调用不留 proxy 具名槽（否则挤低 pkt 基址 4B，ORIG
    // `lea -0x16(%ebp)` 帧 0x24）。
    register char* pPkt = reinterpret_cast<char*>(&pkt);
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())->SendPacket(pPkt, 0xe);
    return 0;   // ORIG 0x0821b02a 尾 `mov $0x0,%eax` ⇒ int 返回 0
}
