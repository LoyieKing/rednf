int Inter_MultiMailBoxReqSend::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    Inter_MultiMailBoxReqSendPktView* pPkt =
        reinterpret_cast<Inter_MultiMailBoxReqSendPktView*>(data);
    unsigned int local_14 = 0;
    for (int local_10 = 0;
         local_10 < (int)(unsigned int)pPkt->m_68;
         ++local_10)
    {
        MSG_MAILBOX_SEND mailBuf;
        memset(&mailBuf, 0, 0x145);
        mailBuf.m_nameLenD = pPkt->m_42;
        strncpy(mailBuf.m_name11, reinterpret_cast<char*>(pPkt) + 0x46,
                mailBuf.m_nameLenD);
        mailBuf.m_gold2e = pPkt->m_64;
        pPkt->m_64 = 0;
        mailBuf.m_space36 = *reinterpret_cast<unsigned char*>(
            reinterpret_cast<char*>(pPkt) + local_10 * 0xb + 0x69);
        mailBuf.m_slot37 = reinterpret_cast<Inter_MultiMailBoxReqSendItemView*>(
            reinterpret_cast<char*>(pPkt) + local_10 * 0xb + 0x60)->m_slot;
        mailBuf.m_itemId39 = reinterpret_cast<Inter_MultiMailBoxReqSendItemView*>(
            reinterpret_cast<char*>(pPkt) + local_10 * 0xb + 0x60)->m_itemId;
        mailBuf.m_extra3d = reinterpret_cast<Inter_MultiMailBoxReqSendItemView*>(
            reinterpret_cast<char*>(pPkt) + local_10 * 0xb + 0x60)->m_extra;

        local_14 =
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user,
                &mailBuf,
                pPkt->m_1d,
                false,
                pPkt->m_29,
                pPkt->m_3fbd);
        if (local_14 != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c,
                                     (unsigned char)(local_14 & 0xff));
            return 0;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0x13c);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
