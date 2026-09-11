int WarRoom::SendWalkOut(CUser* user, ENUM_WALKOUT_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 10);
    packet.put_byte(0);
    packet.put_byte((int)type);
    packet.finalize(true);
    user->Send(packet);
    return 1;
}