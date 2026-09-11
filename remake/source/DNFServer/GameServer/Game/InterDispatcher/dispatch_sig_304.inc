int Inter_GetPremiumSelectedCubeInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GetPremiumSelectedCubeInfoPktView* pPkt = reinterpret_cast<Inter_GetPremiumSelectedCubeInfoPktView*>(data);
    if (user->getCurCharacR() == 0)
        return 0;

    user->initContractOfCubePremiumInfo(
        pPkt->m_4 != '\0',
        pPkt->m_5);

    PacketGuard packet;
    packet.put_header(0, 0x217);
    packet.put_byte((unsigned int)(unsigned char)pPkt->m_4);
    packet.put_byte((unsigned int)pPkt->m_5);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}