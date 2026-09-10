int Inter_AddPvPBuddyResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AddPvPBuddyResultPktView* pPkt = reinterpret_cast<Inter_AddPvPBuddyResultPktView*>(data);
    if (user->get_state() <= 2)
        return 0;
    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    if (pPkt->m_12 == '\0')
    {   // 拒绝分支（0x121）
        if (pPkt->m_32 == '\0')
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(1, 0x121);
            packet.put_byte(1);
            packet.put_byte(pPkt->m_13);
            const int local_14 = (int)strlen(pPkt->m_14);
            packet.put_int(local_14);
            packet.put_str(pPkt->m_14, local_14);
            packet.finalize(true);
            user->Send(packet);
        }
        else
        {
            user->SendCmdErrorPacket(
                (ENUM_CMDPACKET)0x121,
                pPkt->m_32);
        }
    }
    else if (pPkt->m_32 == '\0')
    {   // 同意分支（0x122）
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x122);
        packet.put_byte(1);
        packet.put_byte(pPkt->m_13);
        const size_t local_10 = strlen(pPkt->m_14);
        packet.put_int(local_10);
        packet.put_str(pPkt->m_14, local_10);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x122,
            pPkt->m_32);
    }
    return 0;
