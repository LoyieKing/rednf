void GameWorld::make_packet_chat_msg(PacketGuard& packet, CUser* user, char* msg, int len,
                                     char type, ENUM_NOTIPACKET notify)
{
    packet.clear();
    packet.put_header(0, (int)notify);
    packet.put_byte((int)type);
    if (user == 0)
    {
        packet.put_short(0);
        packet.put_byte(0);
    }
    else
    {
        packet.put_short(user->get_unique_id());
        packet.put_byte(user->GetServerGroup());
    }
    packet.put_int(len);
    packet.put_str(msg, len);
    packet.finalize(true);
