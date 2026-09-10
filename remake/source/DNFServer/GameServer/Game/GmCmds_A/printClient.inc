void CBoosterTest::printClient(CUser* user, bool more, char const* name, char const* contents)
{
    PacketGuard packet;
    packet.put_header(0, 0x13e);
    packet.put_byte(more);
    packet.put_int(static_cast<int>(strlen(name)));
    packet.put_str(name, static_cast<int>(strlen(name)));
    packet.put_int(static_cast<int>(strlen(contents)));
    packet.put_str(contents, static_cast<int>(strlen(contents)));
    packet.finalize(true);
    user->Send(packet);
