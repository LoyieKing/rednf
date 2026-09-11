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
        if (gmList[i] == user->getCurCharacNo())
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
}