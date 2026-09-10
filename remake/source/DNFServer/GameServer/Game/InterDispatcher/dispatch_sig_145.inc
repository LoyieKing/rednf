#pragma pack(push, 1)
// P248 同型化 + P0 去具名局部（ORIG 0x084d6ad6）：
//   +0x021 `mov 0x4(%eax),%eax` 单段 32 位 ⇒ 视图 +0x04 为 int（聚合器该槽是 char[4] 未消费，R1 禁改聚合器，本件自带同布局视图）；
//   +0x0f6/+0x10f/+0x16c `movzbl 0x8(%eax),%eax` ⇒ +0x08 unsigned char；+0x11a `movzbl 0x9(%eax),%eax` ⇒ +0x09 unsigned char；
//   +0x0fa `movzbl %al,%eax`（我方原 `mov %al,<S>; movzbl <S>,%eax` 两跳）= 原码无 `const unsigned char` 具名中转。
struct Inter_AuctionResultRegistPktViewI
{
    int m_0;                 // +0x00  ORIG 084d6c09 +0x133 `mov (%eax),%eax`
    int m_4;                 // +0x04  ORIG 084d6af7 +0x021 `mov 0x4(%eax),%eax`
    unsigned char m_8;       // +0x08  ORIG 084d6bcc +0x0f6 `movzbl 0x8(%eax),%eax`
    unsigned char m_9;       // +0x09  ORIG 084d6bf0 +0x11a `movzbl 0x9(%eax),%eax`
};
#pragma pack(pop)

int Inter_AuctionResultRegist::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    // P324：ORIG 084d6aee +0x018 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 落在 user==0 检查之后。
    Inter_AuctionResultRegistPktViewI* pPkt =
        reinterpret_cast<Inter_AuctionResultRegistPktViewI*>(data);

    if (pPkt->m_4 != user->getCurCharacNo())         // ORIG 084d6af7 +0x021 `mov 0x4(%eax),%eax`
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3ab4, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultRegist",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xba);
    // ORIG 084d6bcc +0x0f6 `movzbl 0x8(%eax),%eax` + 084d6bd0 +0x0fa `movzbl %al,%eax`
    packet.put_byte(static_cast<int>(pPkt->m_8));
    if (static_cast<char>(pPkt->m_8) == '\0')        // ORIG 084d6be5 +0x10f 低字节为 0 时补错误码
        packet.put_byte(
            static_cast<int>(pPkt->m_9));
    packet.put_byte(pPkt->m_0);                      // ORIG 084d6c09 +0x133 `mov (%eax),%eax` pay 类型
    packet.finalize(true);
    user->Send(packet);

    if (static_cast<char>(pPkt->m_8) == '\x01')      // 成功：清理 AP 9 号动作组
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)9, 0, (ENUM_PACKETCLASS)0);
    }
    return 0;
