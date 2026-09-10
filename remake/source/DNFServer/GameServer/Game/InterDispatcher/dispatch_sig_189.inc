#pragma pack(push, 1)
// P30 索引寻址同型化（ORIG 0x084dbbc2）：
//   ORIG 对数据区发 `mov <S>,%edx; add $0x4,%edx; mov 0x2(%eax,%edx,4),%ebx`
//   ⇒ 视图 +0x02 处是 int 数组（pack(1) 非对齐），每处访问的原码形为 `pPkt->m_2[i + 4]`（%edx=i 保留 `add $0x4`）；
//   我方原写 `data + 0x12 + i * 4` 走指针算术，每处多 `shl $0x2/add $0x12/add data` 三条（地址相同）。
//   +0x0e 计数槽（ORIG `mov 0xe(%eax),%eax`）即 m_2[3]（2 + 3*4 = 0x0e，常量下标折叠）。
//   聚合器 Inter_ItemLimitEdition_Sell_EndPktView 未声明该数组（R1 禁改聚合器），故本件自带同布局视图。
struct Inter_ItemLimitEdition_Sell_EndPktViewI
{
    unsigned char m_0;             // +0x00
    unsigned char m_1;             // +0x01
    int m_2[8];                    // +0x02..+0x21 数据数组；m_2[i + 4] 覆盖 +0x12 起，m_2[3] = +0x0e 计数
};
#pragma pack(pop)

int Inter_ItemLimitEdition_Sell_End::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
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
