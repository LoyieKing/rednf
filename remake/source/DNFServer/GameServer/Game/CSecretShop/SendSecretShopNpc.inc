void CSecretShop::SendSecretShopNpc(CUser* user, int npcIdx)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x114);
    ((InterfacePacketBuf*)&packet)->put_int(npcIdx);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
