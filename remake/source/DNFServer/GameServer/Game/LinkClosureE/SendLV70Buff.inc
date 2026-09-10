void SendLV70Buff(CUser* user, unsigned long buff)
{
    CBuffGroup group(user);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x100);
    packet.put_byte(1);
    packet.put_int((int)buff);
    packet.put_int(0xe10);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet, &group);
    packet.clear();
    packet.put_header(0, 0x103);
    packet.put_short((int)(unsigned short)user->get_unique_id());
    packet.put_int((int)buff);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet);
