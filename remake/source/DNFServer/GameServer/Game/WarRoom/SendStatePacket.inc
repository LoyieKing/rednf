void WarRoom::SendStatePacket()
{
    PacketGuard packet;
    packet.put_header(0, 0x58);
    packet.put_short(1);
    MakeStateInfo((char*)&packet);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
