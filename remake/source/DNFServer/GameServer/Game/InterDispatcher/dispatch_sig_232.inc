// [b28d-solo-InterDispatcher 收口] verdict=AE（P231 register int 局部直落 %ebx → 归零）。
int Inter_RemovePvPBuddyResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_RemovePvPBuddyResultPktView* pPkt = reinterpret_cast<Inter_RemovePvPBuddyResultPktView*>(data);
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    if (pPkt->m_31 != '\0')
    {   // 结果码非零 → 错误包
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x123,
            (unsigned int)pPkt->m_31);
    }
    // ORIG：PacketGuard 只在 else 支，两支汇合后统一 `mov $0x0,%eax`。
    else
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x123);
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_12);
        const int nameLen = strlen(pPkt->m_13);
        packet.put_int(nameLen);
        packet.put_str(pPkt->m_13, nameLen);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}