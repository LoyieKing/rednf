int Inter_NotifyAuctionMail::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NotifyAuctionMailPktView* pPkt = reinterpret_cast<Inter_NotifyAuctionMailPktView*>(data);
    if (user->getCurCharacR() == 0 ||                // asm 双失败归并 true
        user->getCurCharacNo() !=
            pPkt->m_a)     // msg+0x0a 角色号
        return 0x3748;

    // 频道门：GetChannelType()==7 或 IsIntegratedPvPBaseChannel() 时跳过发包
    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;

    reinterpret_cast<WongWork::CMailBox*>(user->GetMailBox())
        ->IncNotLoadedMailCount();                   // ORIG W 0x084ed330

    PacketGuard packet;
    packet.put_header(0, 0xa9);
    packet.put_byte(                                 // msg+0x17（movzbl）
        static_cast<int>(pPkt->m_17));
    packet.put_byte(                                 // msg+0x12（movzbl）
        static_cast<int>(pPkt->m_12));
    packet.put_int(pPkt->m_13);
    // ORIG 0x084d56e2：call 后 `cwtl` → 实参以 short 形态再提升 int（探针 s1 实证）。
    packet.put_short(                                // 未读邮件计数
        (int)(short)reinterpret_cast<WongWork::CMailBox*>(user->GetMailBox())
            ->GetNotLoadedMailCount());
    // ORIG 0x084d56f5：经 -0xc(%ebp) 包基址槽取随机选项区
    reinterpret_cast<const RandomOption*>(
        // [b28d-solo-InterDispatcher] RandomOption::put_packet_random_option<PacketGuard>
        // verdict=DIFF ndiff=27：N107 成员常量偏移取址 add-vs-lea（4.4 恒 add；六形探针全败）⇒ N 封顶。
        reinterpret_cast<char*>(pPkt) + 0x18)
        ->put_packet_random_option(packet);          // msg+0x18 随机选项
    packet.finalize(true);
    user->Send(packet);
    return 0;
}