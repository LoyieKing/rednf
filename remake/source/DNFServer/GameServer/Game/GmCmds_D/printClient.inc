void CIndependentDrop::printClient(CUser* user, const char* fileName,
                                   const char* contents)
{
    PacketGuard packet;
    packet.put_header(0, 0x13e);
    packet.put_byte(1);
    packet.put_int(static_cast<int>(strlen(fileName)));
    packet.put_str(fileName, static_cast<int>(strlen(fileName)));
    packet.put_int(static_cast<int>(strlen(contents)));
    packet.put_str(contents, static_cast<int>(strlen(contents)));
    packet.finalize(true);
    user->Send(packet);
