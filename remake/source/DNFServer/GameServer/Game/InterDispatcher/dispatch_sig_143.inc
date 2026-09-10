#pragma pack(push, 1)
// P248 同型化 + P0 去具名局部 + P324 语句序（ORIG 0x084d5740）：
//   +0x018 `mov 0x10(%ebp),%eax; mov %eax,-0x18(%ebp)` ⇒ pPkt 赋值在 user==0 检查之后；
//   +0x021 `mov 0x4(%eax),%eax`（单段 32 位）⇒ +0x04 int；+0x109 `mov (%eax),%eax` ⇒ +0x00 int；
//   +0x11d `mov 0x8(%eax),%eax`（32 位送 put_int）⇒ +0x08 int；
//   +0x13e `mov 0xc(%eax,%edx,4),%eax`（edx=循环变量 i 无加法）⇒ 原码 `pPkt->m_c[i]`，无 `const int* msg` 具名指针；
//   +0x172 `mov 0x8(%eax,%edx,4),%eax`（edx=i+4，ORIG +0x16f `add $0x4,%edx`）⇒ 第二循环实访 +0x18..+0x20。
//   聚合器 Inter_AuctionResultAskPricePktView 只声明了 m_0（R1 禁改聚合器），故本件自带同布局视图。
struct Inter_AuctionResultAskPricePktViewI
{
    int m_0;                       // +0x00  ORIG 084d5849 +0x109 `mov (%eax),%eax`
    int m_4;                       // +0x04  ORIG 084d5761 +0x021 `mov 0x4(%eax),%eax`
    int m_8;                       // +0x08  ORIG 084d585d +0x11d `mov 0x8(%eax),%eax`（送 put_int）
    int m_c[3];                    // +0x0c  ORIG 084d587e +0x13e `mov 0xc(%eax,%edx,4),%eax`
    int m_18[3];                   // +0x18  ORIG 084d58b2 +0x172 `mov 0x8(%eax,%edx,4),%eax`（edx=i+4）
};
#pragma pack(pop)

int Inter_AuctionResultAskPrice::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp); jne
        return 0;

    // P324：ORIG 084d5758 +0x018 pPkt=data 落在 user==0 检查之后。
    Inter_AuctionResultAskPricePktViewI* pPkt =
        reinterpret_cast<Inter_AuctionResultAskPricePktViewI*>(data);

    if (pPkt->m_4 != user->getCurCharacNo())         // ORIG 084d5761 +0x021 `mov 0x4(%eax),%eax`
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
    packet.put_byte(pPkt->m_0);                      // ORIG 084d5849 +0x109 `mov (%eax),%eax`
    packet.put_int(pPkt->m_8);                       // ORIG 084d585d +0x11d `mov 0x8(%eax),%eax`
    for (int i = 0; i <= 2; ++i)                     // ORIG 084d587e +0x13e `mov 0xc(%eax,%edx,4),%eax`
        packet.put_int(pPkt->m_c[i]);
    for (int i = 0; i <= 2; ++i)                     // ORIG 084d58b2 +0x172（实访 0x18+4i；ORIG 形为 [i+4] 相对 +0x08）
        packet.put_int(pPkt->m_18[i]);
    packet.finalize(true);
    user->Send(packet);
    return 0;
