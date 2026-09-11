// [b28d-solo-InterDispatcher 收口] verdict=AE（P231 register int 局部直落 %ebx → 归零）。
int Inter_GuildAttendanceInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildAttendanceInfoPktView* pPkt = reinterpret_cast<Inter_GuildAttendanceInfoPktView*>(data);
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x164);
    packet.put_byte(1);
    packet.put_byte((int)pPkt->m_12);  // movzbl+movsbl
    packet.put_int(pPkt->m_13);
    packet.put_int(pPkt->m_17);
    packet.put_int(pPkt->m_2b);
    packet.put_int(pPkt->m_23);
    packet.put_int(pPkt->m_27);
    packet.put_int(pPkt->m_1b);
    packet.put_int(pPkt->m_1f);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}