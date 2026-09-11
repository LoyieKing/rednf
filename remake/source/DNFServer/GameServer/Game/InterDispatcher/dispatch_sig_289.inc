// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084e6e82）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=77（ORIG 0x84e6e82）：N22 cmp 操作数方向+P30/N4 add-vs-lea+P264 .o 重定位/立即数。
int Inter_NpcLimitBuyItemInfoRenew::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NpcLimitBuyItemInfoRenewPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemInfoRenewPktView*>(data);
    // [K3 2026-09-11] P0：ORIG 循环条件每轮重读 m_a（`mov -0x10(%ebp),%eax; mov 0xa(%eax),%eax;
    //   cmp -0xc(%ebp),%eax; jle`），无 itemCount 具名槽 ⇒ 内联。

    PacketGuard packet;
    packet.put_header(0, 0x193);
    packet.put_int(pPkt->m_a);
    for (int i = 0; i < pPkt->m_a && i < 0x1e; ++i)   // asm 双条件（jle/jg 归并）
    {
        packet.put_int(*reinterpret_cast<int*>(data + i * 0xc + 0xe));
        if (*reinterpret_cast<int*>(data + i * 0xc + 0x12) == 0)
        {
            packet.put_int(0);
        }
        else if (*reinterpret_cast<unsigned int*>(data + i * 0xc + 0x16) <
                 *reinterpret_cast<unsigned int*>(data + i * 0xc + 0x12))   // jb 无符号比较
        {
            packet.put_int(
                *reinterpret_cast<int*>(data + i * 0xc + 0x12) -
                *reinterpret_cast<int*>(data + i * 0xc + 0x16));
        }
        else
        {
            packet.put_int(0);
        }
    }
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}