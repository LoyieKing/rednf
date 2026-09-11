void CGameMasterCharacterItemDropRate::sendServerState(
    CUser* user, char const* msg)                            // 0x084b4330
{
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int((int)strlen(msg));
    packet.put_str(msg, (int)strlen(msg));
    packet.finalize(true);
    user->Send(packet);
}