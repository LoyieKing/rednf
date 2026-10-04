int Inter_AuctionResultRegistCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    // P324：ORIG 084d7434 +0x018 pPkt=data 落在 user==0 检查之后。
    SIG_AUCTION_RESULT_REGIST_CANCEL* res =
        reinterpret_cast<SIG_AUCTION_RESULT_REGIST_CANCEL*>(data);

    if (res->m_characNo != user->getCurCharacNo())         // ORIG 084d743d +0x021 `mov 0x4(%eax),%eax`
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3c88, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultRegistCancel",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xbb);
    packet.put_byte(static_cast<int>(res->m_okFlag));    // ORIG `movzbl 0x8(%eax),%eax` + `movzbl %al,%eax`
    if (static_cast<char>(res->m_okFlag) == '\0')        // 低字节为 0 时补错误码
        packet.put_byte(
            static_cast<int>(res->m_errCode));
    packet.put_byte(res->m_result);                      // ORIG `mov (%eax),%eax` pay 类型
    packet.finalize(true);
    user->Send(packet);
    return 0;
}