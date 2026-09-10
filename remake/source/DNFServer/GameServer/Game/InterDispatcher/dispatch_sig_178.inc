int Inter_NoticeServerMessageInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticeServerMessageInfoPktView* pPkt = reinterpret_cast<Inter_NoticeServerMessageInfoPktView*>(data);
    const char* const local_24 = data;
    CSharedServerMessageManager* local_20 =
        G_CGameManager()->GetSharedServerMessageManager();
    if (local_20 != 0)
    {
        // P183：ORIG 无 sourceChannel 具名 byte 槽（`movzbl %al,%ebx` 直取）⇒ 内联表达式。
        if (pPkt->m_a != G_CEnvironment()->get_channel_no())
        {
            local_20->UpdateServerMessageByBroadcast(
                (int)pPkt->m_c,
                const_cast<char*>(local_24 + 0x11),
                const_cast<char*>(local_24 + 0x47),
                pPkt->m_5b);
        }
        cMyTrace(__PRETTY_FUNCTION__,
            0x42be, 0)("SERVER_MESSAGE : Recv ServerMessage from monitor server(source_channel:%d, kind:%d, index:%d, msg_len:%d, msg:%s, name_len:%d, name:%s, time:%d",
              (unsigned int)pPkt->m_a,
              (unsigned int)pPkt->m_b,
              (unsigned int)pPkt->m_c,
              pPkt->m_d,
              local_24 + 0x11,
              pPkt->m_43,
              local_24 + 0x47,
              pPkt->m_5b);
    }

    PacketGuard packet;
    packet.put_header(0, 0xc0);
    packet.put_byte(1);
    packet.put_byte(1);
    packet.put_byte(
        (unsigned int)pPkt->m_c);
    packet.put_int(pPkt->m_d);
    packet.put_str(pPkt->m_11, pPkt->m_d);
    packet.put_int(pPkt->m_43);
    packet.put_str(pPkt->m_47, pPkt->m_43);
    packet.finalize(true);
    G_GameWorld()->send_all_with_state(packet, 3);
    return 0;
