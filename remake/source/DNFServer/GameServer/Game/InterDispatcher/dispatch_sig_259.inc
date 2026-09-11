int Inter_CheckJoinGuild::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_CheckJoinGuildPktView* pPkt = reinterpret_cast<Inter_CheckJoinGuildPktView*>(data);


    if (pPkt->m_0 != 0)
    {   // 结果码非零 → 错误包
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x15c,
            (unsigned char)pPkt->m_0);
    }
    // ORIG：PacketGuard 只在 else 支（两支汇合后 `mov $0x0,%eax`）。
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x15c);
        packet.put_byte(1);
        int strLen = strlen(pPkt->m_22);
        packet.put_int(strLen);
        packet.put_str(pPkt->m_22, strLen);
        strLen = strlen(pPkt->m_4);
        packet.put_int(strLen);
        packet.put_str(pPkt->m_4, strLen);
        packet.put_int(pPkt->m_3c);
        packet.put_int(pPkt->m_40);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}