// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084e6e82）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=77（ORIG 0x84e6e82）：N22 cmp 操作数方向+P30/N4 add-vs-lea+P264 .o 重定位/立即数。
int Inter_NpcLimitBuyItemInfoRenew::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NpcLimitBuyItemInfoRenewPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemInfoRenewPktView*>(data);
    // [K3 2026-09-11] P0：ORIG 循环条件每轮重读 m_a（`mov -0x10(%ebp),%eax; mov 0xa(%eax),%eax;
    //   cmp -0xc(%ebp),%eax; jle`），无 itemCount 具名槽 ⇒ 内联。

    PacketGuard packet;
    packet.put_header(0, 0x193);
    packet.put_int(pPkt->m_count);
    for (int i = 0; i < pPkt->m_count && i < 0x1e; ++i)   // asm 双条件（jle/jg 归并）
    {
        // [R9form4 2026-09-12] 裸偏移 (int*)(data+i*0xc+K) → 视图记录数组具名访问（ORIG 0x084e6ed7
        //   `mov 0xe(%eax,%ecx,1),%eax` 折叠 disp；base 必须用具名局部 pPkt 而非形参 data）。
        packet.put_int(pPkt->m_recs[i].m_f0);
        if (pPkt->m_recs[i].m_f1 == 0)
        {
            packet.put_int(0);
        }
        // [R9form4 2026-09-12 极性/块序] ORIG 0x084e6f47 为 `mov 0x12(%eax,%ecx,1),%eax;
        //   cmp %eax,%ebx; jb <差值段>`（put_int(0) 段在前、差值段 fall-through）；
        //   写 `f2 >= f1` 令 4.4 产 `jb <差值段>` 同形（P222/P354 块序旋钮）。
        else if ((unsigned int)pPkt->m_recs[i].m_f2 >=
                 (unsigned int)pPkt->m_recs[i].m_f1)   // 无符号比较（ORIG 先读 0x16 后读 0x12）
        {
            packet.put_int(0);
        }
        else
        {
            packet.put_int(pPkt->m_recs[i].m_f1 - pPkt->m_recs[i].m_f2);
        }
    }
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}