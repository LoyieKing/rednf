int CFairMatch::OnWalkOut(PvP_Room* room, int seat)
{
    const int team = room->get_team(seat);
    const int userSeat = room->get_user_seat(seat);
    OnLeaveRoom(reinterpret_cast<CUser*>(userSeat), room, team, seat);
    return 1;
