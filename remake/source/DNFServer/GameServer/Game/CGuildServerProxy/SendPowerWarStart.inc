void CGuildServerProxy::SendPowerWarStart(unsigned int mid)
{
    if (GlobalData::s_pGMAccounts_->isGM(mid) == false)
    {
        return;
    }
    Packet_GM_PowerWar_Start pkt;
    SendPacket((char*)&pkt, pkt.packetSize);
}