// [b28d-lane2] verdict=DIFF ndiff=51（ORIG 0x084c8b0c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=44（ORIG 0x84c8b0c）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
// [lane2 2026-09-10] verdict=DIFF→AE：与 Inter_SessionClose 同三形（P0 去 accId/P59 guard 极性/P227 临时链）。
int Inter_MonitorUserGetout::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorUserGetoutPktView* pPkt = reinterpret_cast<Inter_MonitorUserGetoutPktView*>(data);
    // §P0/P48：ORIG 084c8b1d `mov 0xa(%eax),%ebx` 直传（无 accId 槽）、084c8bd8 第二处
    //   独立重读 pPkt->m_a ⇒ 两处各写表达式，不设具名局部。
    CUser* target = G_CGameManager()->GetUserByAccId(pPkt->m_a);
    // P59 guard 极性：ORIG 084c8b38 `jne <if 体>` + 084c8b3a `mov $0,%ebx; jmp <共享尾>`。
    if (target == 0)
        return 0;
    {
        PacketGuard packet;
        packet.put_header(1, 1);
        packet.put_byte(0);
        packet.put_byte(0x41);
        packet.finalize(true);
        target->Send(packet);
        target->DisConnSig((DISCONN_SIG)0xf, true, 0);
        // P227：ORIG 084c8bdb m_a 重读后留 %ebx 跨 ctor（084c8bfc）。
        cMyTrace(__PRETTY_FUNCTION__, 0x226c, 0)(
            // [H2·2026-09-11] §6.5 语义轴：ORIG 实字节为 EUC-KR（同上，ACCID: %d 变体）。
            "\xb0\xad\xc1\xa6 \xc1\xbe\xb7\xe1\xbf\xe4\xc3\xbb ACCID: %d", pPkt->m_a);
        return 0;
    }
}