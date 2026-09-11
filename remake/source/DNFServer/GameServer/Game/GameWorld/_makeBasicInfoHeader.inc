int _makeBasicInfoHeader(PacketGuard& packet)
{
    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    int index = packet.get_index();
    packet.put_short(0);
    return index;
}