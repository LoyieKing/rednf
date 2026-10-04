int Inter_AuctionResultAskPrice::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp); jne
        return 0;

    // P324：ORIG 084d5758 +0x018 pPkt=data 落在 user==0 检查之后。
    SIG_AUCTION_RESULT_ASK_PRICE* ask =
        reinterpret_cast<SIG_AUCTION_RESULT_ASK_PRICE*>(data);

    if (ask->m_characNo != user->getCurCharacNo())         // ORIG 084d5761 +0x021 `mov 0x4(%eax),%eax`
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle；ORIG d _ZL6gmList 同 TU gmList）
    for (int i = 0; i <= 4; ++i)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x377c, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultAskPrice",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;                              // ORIG 0x0858dd4c C1
    packet.put_header(1, 0xb9);                      // put_header(1,0xb9)
    packet.put_byte(1);
    packet.put_byte(ask->m_result);                      // ORIG 084d5849 +0x109 `mov (%eax),%eax`
    packet.put_int(ask->m_itemIdx);                       // ORIG 084d585d +0x11d `mov 0x8(%eax),%eax`
    for (int i = 0; i <= 2; ++i)                     // ORIG 084d587e +0x13e `mov 0xc(%eax,%edx,4),%eax`
        packet.put_int(ask->m_priceCur[i]);
    for (int i = 0; i <= 2; ++i)                     // ORIG 084d58b2 +0x172（实访 0x18+4i；ORIG 形为 [i+4] 相对 +0x08）
        packet.put_int(ask->m_priceTop[i]);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}