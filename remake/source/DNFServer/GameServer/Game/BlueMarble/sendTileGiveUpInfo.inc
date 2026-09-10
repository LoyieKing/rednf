int BlueMarble::sendTileGiveUpInfo(int seat) const
{
    PacketGuard packet;
    packet.put_header(0, 0x1b3);
    packet.put_byte(seat);
    packet.finalize(true);
    sendToBlueMarble(packet);
