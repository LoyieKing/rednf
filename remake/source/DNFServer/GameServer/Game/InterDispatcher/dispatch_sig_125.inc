int Inter_SecurityCardActivate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SecurityCardActivatePktView* pPkt = reinterpret_cast<Inter_SecurityCardActivatePktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    if (pPkt->m_a == '\0')
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa8, 1);
        return 0;
    }

    user->getSecurityCard()->setActivate(true);
    user->getSecurityCard()->setCertFlag('\x01');

    PacketGuard packet;
    packet.put_header(1, 0xa8);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
