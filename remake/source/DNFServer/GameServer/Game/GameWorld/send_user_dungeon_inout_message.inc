void GameWorld::send_user_dungeon_inout_message(CUser* user, int dungeonIdx,
                                                bool open)
{
    PacketGuard packet;
    packet.put_header(0, 0x127);
    packet.put_int(dungeonIdx);
    packet.put_byte(open);
    packet.finalize(true);
    user->Send(packet);
