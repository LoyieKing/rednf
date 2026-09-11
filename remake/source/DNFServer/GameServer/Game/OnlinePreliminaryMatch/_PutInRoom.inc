int COnlinePreliminary::_PutInRoom(CUser* user, PvP_Room* room, bool observer)
{
    PacketGuard pkt;
    int seat = 0;
    int err = room->join_room(user, seat, observer);       // T 0x85d6cf2
    if (err >= 1)
    {
        pkt.put_header(1, 0x36);
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
        return seat;
    }
    room->make_seat_info((char*)(PacketGuard*)&pkt, seat);  // T 0x85db9e2
    G_GameWorld()->send_all(pkt);
    G_GameWorld()->goto_pvp(user);
    return seat;
}