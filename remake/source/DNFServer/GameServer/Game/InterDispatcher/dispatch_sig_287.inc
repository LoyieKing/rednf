// [b28d-lane2] verdict=DIFF ndiff=135（ORIG 0x084e6258）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=87（ORIG 0x84e6258）：P30/N4 add-vs-lea+P264 .o 重定位/立即数。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→6（156 vs 150，TU 121/1331/5/133 不回退）：真修两处——
//   ① `user==0 || get_state()==0` 拆两条独立守卫（P133，复现 ORIG 两段 `jne`/`sete;je` 早退）；
//   ② pPkt 声明移到 get_state 检查后（P0，复现 ORIG `mov 0x10(%ebp),%eax; mov %eax,-0x10` 位置）。
//   残余=data 形参 vs ORIG -0x10 槽基址（`lea (%ecx,%eax,1)` vs `add $0x1a; add 0x10(%ebp)`，P30/N4 族）。
int Inter_NpcLimitBuyItemInfoAll::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                     // ORIG 两独立守卫（P133）
        return 0;
    if (user->get_state() == 0)
        return 0;

    Inter_NpcLimitBuyItemInfoAllPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemInfoAllPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x193);
    packet.put_int(pPkt->m_16);   // msg+0x16 count
    // ORIG 0x84e62de..0x84e63a9 每处 `lea (%ecx,%eax,1)` 的 ecx = -0x10 具名槽（pPkt），
    // 非形参 0x10(%ebp)（P30/N4 族可消）。
#define pData (reinterpret_cast<char*>(pPkt))
    for (int i = 0;
         i < pPkt->m_16 && i < 0x1e; ++i)
    {
        packet.put_int(
            *reinterpret_cast<int*>(pData + i * 0xc + 0x1a));
        if (*reinterpret_cast<int*>(pData + i * 0xc + 0x1e) == 0)
        {
            packet.put_int(0);
        }
        else if ((unsigned int)*reinterpret_cast<unsigned int*>(pData + i * 0xc + 0x22) <
                 (unsigned int)*reinterpret_cast<unsigned int*>(pData + i * 0xc + 0x1e))
        {
            packet.put_int(
                *reinterpret_cast<int*>(pData + i * 0xc + 0x1e) -
                *reinterpret_cast<int*>(pData + i * 0xc + 0x22));
        }
        else
        {
            packet.put_int(0);
        }
    }
#undef pData
    packet.finalize(true);
    user->Send(packet);
    return 0;
}