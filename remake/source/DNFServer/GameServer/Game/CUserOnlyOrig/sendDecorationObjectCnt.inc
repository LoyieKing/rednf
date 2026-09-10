void CUser::sendDecorationObjectCnt(short count)
{
    PacketGuard packet;
    packet.put_header(0, 0x1ac);
    packet.put_short(static_cast<int>(count));
    packet.finalize(true);
    Send(packet);
