// [b28d-lane2] verdict=DIFF ndiff=138（ORIG 0x084e27b8）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=135（ORIG 0x84e27b8）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [Lane30-InterDispatcher] 权威头 MSG_MAILBOX_SEND 具名成员直访（原 TU 局部 MailBufSendView
//   cast 反模式）：ORIG 084e27b8 的 mail 缓冲槽 -0x165 的写点即 -0x165+0x0d/0x11/0x2e/0x36/
//   0x37/0x39/0x3d = m_nameLenD/m_name11/m_gold2e/m_space36/m_slot37/m_itemId39/m_extra3d
//   （无中间视图指针槽：原 pMail 局部使帧 +0x10 并多出 lea/mov 与 lea 折址）。
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
        mailBuf.m_nameLenD = pPkt->m_nameLen;
        strncpy(mailBuf.m_name11, pPkt->m_name,
                mailBuf.m_nameLenD);
        mailBuf.m_gold2e = pPkt->m_gold;
        pPkt->m_gold = 0;
        Inter_MultiMailBoxReqSendItemView* item =
            reinterpret_cast<Inter_MultiMailBoxReqSendItemView*>(
                &pPkt->m_items[local_10]);
        mailBuf.m_space36 = pPkt->m_items[local_10].m_space;
        mailBuf.m_slot37 = item->m_slot;
        mailBuf.m_itemId39 = item->m_itemId;
        mailBuf.m_extra3d = item->m_extra;

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
}