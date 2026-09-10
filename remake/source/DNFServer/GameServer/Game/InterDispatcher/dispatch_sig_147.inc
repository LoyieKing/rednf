#pragma pack(push, 1)
// P248 同型化 + P0 去具名局部 + P324 语句序（ORIG 0x084d741c）：
//   +0x018 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` ⇒ pPkt 赋值在 user==0 检查之后；
//   +0x021 `mov 0x4(%eax),%eax`（单段 32 位）⇒ +0x04 int；+0x0f6 `movzbl 0x8(%eax),%eax` ⇒ +0x08 unsigned char；
//   +0x0fa `movzbl %al,%eax`（我方原 `mov %al,<S>; movzbl <S>,%eax` 两跳）⇒ 原码无 `const unsigned char` 具名中转。
//   聚合器 Inter_AuctionResultRegistCancelPktView 的 +0x04 是 char[4] 未消费（R1 禁改聚合器），故本件自带同布局视图。
struct Inter_AuctionResultRegistCancelPktViewI
{
    int m_0;                 // +0x00
    int m_4;                 // +0x04  ORIG 084d743d +0x021 `mov 0x4(%eax),%eax`
    unsigned char m_8;       // +0x08  ORIG 084d74?? `movzbl 0x8(%eax),%eax`
    unsigned char m_9;       // +0x09
};
#pragma pack(pop)

int Inter_AuctionResultRegistCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    // P324：ORIG 084d7434 +0x018 pPkt=data 落在 user==0 检查之后。
    Inter_AuctionResultRegistCancelPktViewI* pPkt =
        reinterpret_cast<Inter_AuctionResultRegistCancelPktViewI*>(data);

    if (pPkt->m_4 != user->getCurCharacNo())         // ORIG 084d743d +0x021 `mov 0x4(%eax),%eax`
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
    packet.put_byte(static_cast<int>(pPkt->m_8));    // ORIG `movzbl 0x8(%eax),%eax` + `movzbl %al,%eax`
    if (static_cast<char>(pPkt->m_8) == '\0')        // 低字节为 0 时补错误码
        packet.put_byte(
            static_cast<int>(pPkt->m_9));
    packet.put_byte(pPkt->m_0);                      // ORIG `mov (%eax),%eax` pay 类型
    packet.finalize(true);
    user->Send(packet);
    return 0;
