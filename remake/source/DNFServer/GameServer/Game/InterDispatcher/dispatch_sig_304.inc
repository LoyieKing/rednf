int Inter_GetPremiumSelectedCubeInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES* res = reinterpret_cast<SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES*>(data);
    if (user->getCurCharacR() == 0)
        return 0;

    user->initContractOfCubePremiumInfo(
        res->m_selected != '\0',
        res->m_cubeType);

    PacketGuard packet;
    packet.put_header(0, 0x217);
    packet.put_byte((unsigned int)res->m_selected);
    packet.put_byte((unsigned int)res->m_cubeType);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}