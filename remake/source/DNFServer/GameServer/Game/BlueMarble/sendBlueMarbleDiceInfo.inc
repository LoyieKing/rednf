int BlueMarble::sendBlueMarbleDiceInfo(int seat) const
{
    PacketGuard packet;
    packet.put_header(0, 0x1af);
    packet.put_byte(seat);
    packet.put_byte((int)(char)getDiceNumber());
    packet.finalize(true);
    sendToBlueMarble(packet);
}