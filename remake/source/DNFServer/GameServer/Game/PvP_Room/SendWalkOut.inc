int PvP_Room::SendWalkOut(int seat, ENUM_WALKOUT_TYPE walkoutType)
{
    PacketGuard local_18;
    local_18.put_header(0, 10);
    local_18.put_byte(seat);
    local_18.put_byte((int)walkoutType);
    local_18.finalize(true);
    send_to_pvp(local_18);
    return 1;
}