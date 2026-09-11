int Inter_ItemLimitEdition_Sell_End::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [N 判词·2026-09-11 复核] 残差 = ORIG pPkt@-0x10 / packet@-0x2c vs 我方 -0x14/-0x30（整体深 4）。
    //   试形（Main「指针局部→整型承载地址」旋钮：`unsigned int pPktAddr` + 宏视图）实测
    //   **零变化**（仍 95/94 条、our 94 不变）⇒ 本件深 4 不由指针分区产生 ⇒ 落 N82/P212 槽方向。
    Inter_ItemLimitEdition_Sell_EndPktViewI* pPkt =
        reinterpret_cast<Inter_ItemLimitEdition_Sell_EndPktViewI*>(data);
    // ORIG 0x084dbbc2 仅一枚具名包指针（-0x10）；`char* recv`/`const unsigned int count`
    // 两个具名局部在 ORIG 无槽往返（循环界与下标基址均直读）⇒ P0/P329 内联删除。
    PacketGuard packet;
    packet.put_header(0, 0xe1);
    packet.put_byte(1);
    // ORIG 084dbbfc：0xe 按 32 位读（mov 0xe(%eax),%eax）直传 put_byte 形参。
    packet.put_byte(pPkt->m_2[3]);

    // 残（已留痕）：ORIG pPkt 槽 -0x10（浅）/packet -0x2c；4.4 先声明深 ⇒ 我方 -0x14/-0x30
    //（把 i 提前到 packet 前实测 -0x10 被 i 占用、pPkt 仍 -0x14）⇒ N82/P212 跨版本槽方向封顶。
    unsigned int i = 0;
    while (i < (unsigned int)pPkt->m_2[3])              // cmp/seta 无符号（界直读无具名槽）
    {
        const unsigned int ipg =
            (unsigned int)pPkt->m_2[i + 4];             // ORIG `add $0x4,%edx; mov 0x2(%eax,%edx,4),%ecx`
        G_CDataManager()->getItemLimitEditionMgr()->removeItem(ipg);
        cMyTrace(__PRETTY_FUNCTION__,
            0x465a, 0)("ipg: %d", ipg);
        packet.put_int(pPkt->m_2[i + 4]);               // ORIG 重读同址
        i = i + 1;                                     // addl $0x1,mem 形态
    }
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}