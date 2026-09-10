void MakeNotiSetDurability(PacketGuard& packet, int value)
{
    packet.put_header(0, 0xb3);
    packet.put_int(value);
    packet.finalize(true);
