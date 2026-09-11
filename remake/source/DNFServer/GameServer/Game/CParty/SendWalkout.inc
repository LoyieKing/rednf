// ---- CParty::SendWalkout（ORIG T 0x859cb00）----
int CParty::SendWalkout(int param, ENUM_WALKOUT_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 10);
    packet.put_byte(param);
    packet.put_byte((int)type);
    packet.finalize(true);
    send_to_party(packet);
    return 1;
}