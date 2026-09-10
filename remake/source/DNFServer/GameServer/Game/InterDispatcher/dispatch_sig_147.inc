int Inter_AuctionResultRegistCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultRegistCancelPktView* pPkt = reinterpret_cast<Inter_AuctionResultRegistCancelPktView*>(data);
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    const int* msg =&pPkt->m_0;
    if (msg[1] != user->getCurCharacNo())            // msg+0x04 角色号校验
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3c88, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultRegistCancel",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xbb);
    const unsigned char result =                     // msg+0x08 结果码
        pPkt->m_8;
    packet.put_byte(static_cast<int>(result));
    if (static_cast<char>(msg[2]) == '\0')           // 低字节为 0 时补错误码
        packet.put_byte(
            static_cast<int>(pPkt->m_9));
    packet.put_byte(msg[0]);                         // msg+0x00 pay 类型
    packet.finalize(true);
    user->Send(packet);
    return 0;
