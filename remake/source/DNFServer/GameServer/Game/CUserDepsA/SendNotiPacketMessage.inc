void CUser::SendNotiPacketMessage(const char* msg, ENUM_MESSAGE_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(static_cast<int>(type));
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(static_cast<int>(strlen(msg)));
    packet.put_str(msg, static_cast<int>(strlen(msg)));
    packet.finalize(true);
    Send(packet);
