int Inter_SessionClose::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SessionClosePktView* pPkt = reinterpret_cast<Inter_SessionClosePktView*>(data);
    // §P0/P48：ORIG 084c7b01 `mov (%eax),%ebx` 直传（accId 无栈槽），084c7b98 第二处
    //   独立重读 pPkt->m_0 ⇒ 不设具名 accId，两处各写表达式。
    CUser* target = G_CGameManager()->GetUserByAccId(pPkt->m_0);
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
            "강제 종료요청 ACCID: %s", NumberToString(pPkt->m_0, 0));
        target->DisConnSig((DISCONN_SIG)0xf, true, 0);
        // 块内 return：ORIG 084c7c0c `mov $0x0,%ebx` + dtor 后 `jmp <共享尾>`
        //   （探针 /tmp/probe_eh3.cpp f12 命中同形）。
        return 0;
    }
