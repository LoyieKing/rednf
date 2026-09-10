int Inter_AuctionResultAskPrice::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultAskPricePktView* pPkt = reinterpret_cast<Inter_AuctionResultAskPricePktView*>(data);
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp); jne
        return 0;

    const int* msg =&pPkt->m_0;
    if (msg[1] != user->getCurCharacNo())            // msg+0x04 vs 当前角色号
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle；ORIG d _ZL6gmList 同 TU g_auctionResultGmList）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
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
    packet.put_byte(msg[0]);                         // msg+0x00（movzbl 截断）
    packet.put_int(msg[2]);                          // msg+0x08
    for (int i = 0; i <= 2; ++i)                     // msg+0x0c..+0x14（cmpl $0x2, setle）
        packet.put_int(msg[i + 3]);
    for (int i = 0; i <= 2; ++i)                     // msg+0x18..+0x20（同界）
        packet.put_int(msg[i + 6]);
    packet.finalize(true);
    user->Send(packet);
    return 0;
