int Inter_MouseRegister::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_MouseRegisterPktView* pPkt = reinterpret_cast<Inter_MouseRegisterPktView*>(data);
    if (pPkt->m_16 != 0)   // msg+0x16 错误码
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74,
                                 static_cast<unsigned char>(
                                     pPkt->m_16));
        return 0;
    }

    const int action = pPkt->m_4;   // msg+0x04 动作码
    if (action == 1 || action == 2)
    {
        Sanicova::CPad* pad = user->getPad();
        pad->setActivate(true);
        Sanicova::CPad* pad2 = user->getPad();
        pad2->setCertified(true);
        Sanicova::CPad* pad3 = user->getPad();
        pad3->setPassword(data + 0x23);              // msg+0x23 密码
        Sanicova::CPad* pad4 = user->getPad();
        pad4->setCancelCnt(pPkt->m_1b);
        Sanicova::CPad* pad5 = user->getPad();
        pad5->setFailCnt(pPkt->m_1f);
    }
    else if (action == 3)
    {
        Sanicova::CPad* pad = user->getPad();
        pad->setActivate(false);
        Sanicova::CPad* pad2 = user->getPad();
        pad2->setCertified(false);
        Sanicova::CPad* pad3 = user->getPad();
        pad3->setPassword(data + 0x23);
    }

    PacketGuard packet;
    packet.put_header(1, 0x74);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
