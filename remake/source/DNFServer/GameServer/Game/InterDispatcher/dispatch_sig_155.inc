// [b28d-lane2] verdict=DIFF ndiff=130（ORIG 0x084d7c8e）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=77（ORIG 0x84d7c8e）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_AuctionResultBuyItemApiece::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    // P324：ORIG 084d7ca6 +0x018 `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` 落在 user==0 检查之后。
    Inter_AuctionResultBuyItemApiecePktView* pPkt =
        reinterpret_cast<Inter_AuctionResultBuyItemApiecePktView*>(data);

    if (pPkt->m_characNo !=                                  // ORIG 084d7caf +0x021 `mov (%eax),%eax`
        user->getCurCharacNo())
        return 0;

    for (int i = 0; i <= 4; ++i)                       // cmpl $4; setle
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3e12, 0)("Trace Auction Delay, %s(%d)",
                  "ACK AuctionResultBuyItemApiece",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0x14f);
    packet.put_byte(
        (int)pPkt->m_okFlag);
    if (pPkt->m_okFlag == 0)                               // ORIG 084d7d9c +0x10e `movzbl 0x8(%eax),%eax`
    {
        // P0：ORIG 084d7dab +0x11d `mov 0x4(%eax),%ebx`（跨 getCurCharacInvenW 调用保存在寄存器，
        //   无具名 money/inven 栈槽）⇒ 原码为链式单表达式，实参先算、receiver 后算。
        const int gained =
            static_cast<CInventory*>(user->getCurCharacInvenW())
                ->gain_money(pPkt->m_money, (eMoneyAddReason)0x14, true, 0);
        // ORIG 084d7de3 +0x155 `mov 0x4(%eax),%eax; cmp -0xc(%ebp),%eax; jle` ⇒ 原码 `pPkt->m_money > gained`
        if (pPkt->m_money > gained)
        {
            user->SendMoneyFullReason(
                (ENUM_MONEY_FULL_REASON)0,
                (unsigned long)pPkt->m_money, (unsigned long)gained);
        }
        packet.put_byte(
            (int)pPkt->m_errCode);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}