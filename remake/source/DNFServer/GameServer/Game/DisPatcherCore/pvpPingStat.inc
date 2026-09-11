// ORIG 0x0821afbc W _ZN28DisPatcher_UdpCharacteristic11pvpPingStatEP5CUserRtS2_
// （报告 pvpPingStat.md 全文转写，wave2）：Pvp_Ping 包（0xfa8,0xe）同型。
int DisPatcher_UdpCharacteristic::pvpPingStat(CUser* user, unsigned short& bad, unsigned short& total)

{
    Packet_Pvp_Ping_Statistic pkt;
    pkt.m_pvpBadPing = bad;
    pkt.m_pvpTotal = (unsigned short)(total + bad);
    // ORIG +0x2f：&pkt 物化一次直落 callee-saved %ebx（跨两 call 存活，
    // 无栈槽）→ register 局部实证（c6root 探针 register T* 形态）。
    // ORIG：proxy 结果 %eax 短程直落（无槽）→ register 声明。
    // 修正：ORIG `lea -0x16(%ebp),%ebx` + proxy 返回 %eax 3 参直落（无具名 proxy
    // 局部 → 返回值不被搬离 %eax）。
    register char* pPkt = reinterpret_cast<char*>(&pkt);
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())->SendPacket(pPkt, 0xe);
    return 0;   // ORIG 0x0821afbc 尾 `mov $0x0,%eax` ⇒ int 返回 0
}
