void CClearRewardTest::printClient(CUser* user, char const* fileName, char const* content)
{
    PacketGuard packet;
    packet.put_header(0, 0x13e);
    packet.put_byte(1);
    packet.put_int(strlen(fileName));
    packet.put_str(fileName, strlen(fileName));
    packet.put_int(strlen(content));
    packet.put_str(content, strlen(content));
    packet.finalize(true);
    user->Send(packet);
