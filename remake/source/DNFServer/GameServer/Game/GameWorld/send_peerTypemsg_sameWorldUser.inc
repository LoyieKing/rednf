void GameWorld::send_peerTypemsg_sameWorldUser(CUser* from, CUser* to, char type,
                                               int a, int b, const char* msg, int len)
{
    PacketGuard packet;
    packet.put_header(0, 0x41);
    packet.put_byte((int)type);
    packet.put_byte(a);
    int nameLen = (int)strlen(from->getCurCharacName());
    packet.put_int(nameLen);
    packet.put_str(from->getCurCharacName(), nameLen);
    packet.put_byte(0);
    packet.put_int(len);
    packet.put_str(msg, len);
    packet.finalize(true);
    to->Send(packet);
