int BlueMarble::sendBoardZoneActionInfo(int seat, int zone,
                                        unsigned int pos) const
{
    PacketGuard packet;
    packet.put_header(0, 0x1b2);
    packet.put_short(zone);
    packet.put_int((int)pos);
    packet.finalize(true);
    m_users[seat].getUser()->Send(packet);
