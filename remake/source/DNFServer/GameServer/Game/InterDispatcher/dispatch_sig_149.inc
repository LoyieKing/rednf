int Inter_AuctionResultMyRegistedItems::dispatch_sig(CUser* user, char* data, int len)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;
    if (static_cast<unsigned int>(len) <= 3)         // asm cmp $0x3; jbe
        return 0;

    // P183：ORIG 084d777c +0x024 起直接 memcpy，无 `movl $0x0,-0x20(%ebp)` 预清零 ⇒ 原码无 `= 0` 初值。
    int characNoMsg;
    memcpy(&characNoMsg, data, 4);                   // msg+0x00（本地 4 字节拷贝）
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3ced, 0)("Trace Auction Delay, %s(%d)",
                  "ACK AuctionResultMyRegistedItems", user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xbf);
    packet.put_byte(1);
    packet.put_binary(data + 4, len - 4);            // 余包原样转发
    packet.finalize(true);
    user->Send(packet);
    return 0;
