// [b28d-lane2] verdict=DIFF ndiff=59（ORIG 0x084c7af0）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=51（ORIG 0x84c7af0）：P264 .o 重定位/立即数。
// [lane2 2026-09-10] verdict=DIFF→AE（identical=135/ae=1348→1352）：三处源形修复 ——
//   ① accId 去具名（P0/P48 两处各自重读 pPkt->m_accId）；② P59 guard 极性（if(target==0)return 0;
//   + 块内 return 0）复现 ORIG `jne <then>`+`mov $0,%ebx;jmp <共享尾>`；③ P227 全表达式临时链。
int Inter_SessionClose::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SessionClosePktView* pPkt = reinterpret_cast<Inter_SessionClosePktView*>(data);
    // §P0/P48：ORIG 084c7b01 `mov (%eax),%ebx` 直传（accId 无栈槽），084c7b98 第二处
    //   独立重读 pPkt->m_accId ⇒ 不设具名 accId，两处各写表达式。
    CUser* target = G_CGameManager()->GetUserByAccId(pPkt->m_accId);
    // P59 guard 极性：ORIG 084c7b1b `jne <if 体>` + 084c7b1d `mov $0,%ebx; jmp <共享尾>`
    //   ⇒ 写 `if (target == 0) return 0;`（fail 块直落 jcc），then 体（guard 块）后置。
    if (target == 0)
        return 0;
    {
        PacketGuard packet;
        packet.put_header(1, 1);
        packet.put_byte(0);
        packet.put_byte(0x12);
        packet.finalize(true);
        target->Send(packet);
        // P227：ORIG 084c7bad NumberToString 结果留 %ebx 跨 ctor（084c7bcd）⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x2004, 0)(
            // [H2·2026-09-11] §6.5 语义轴：ORIG .rodata 实字节为 EUC-KR
            //   （0x08ae5840 段 `\xb0\xad\xc1\xa6 \xc1\xbe\xb7\xe1\xbf\xe4\xc3\xbb ACCID: %s`），
            //   旧源码写 UTF-8 韩文 ⇒ 发射字节不同（日志/客户端显示乱码）。改 \xNN 字节转义。
            "\xb0\xad\xc1\xa6 \xc1\xbe\xb7\xe1\xbf\xe4\xc3\xbb ACCID: %s", NumberToString(pPkt->m_accId, 0));
        target->DisConnSig((DISCONN_SIG)0xf, true, 0);
        // 块内 return：ORIG 084c7c0c `mov $0x0,%ebx` + dtor 后 `jmp <共享尾>`
        //   （探针 /tmp/probe_eh3.cpp f12 命中同形）。
        return 0;
    }
}