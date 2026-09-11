void CGuildServerProxy::SendPowerWarEnd(unsigned int mid)
{
    if (GlobalData::s_pGMAccounts_->isGM(mid) == false)
    {
        return;
    }
    Packet_GM_PowerWar_End pkt;
    SendPacket((char*)&pkt, pkt.packetSize);
}