void COnlinePreliminary::_PullOutRoom(CUser* user, PvP_Room* room, int seat)
{
    if (!room->OnSetPVPSeatState(user, seat,
                                 ENUM_SEAT_STATE_0xfe,
                                 (ENUM_WALKOUT_TYPE)2))     // T 0x85dd070
    {
        room->reset();                                      // T 0x85d62c8
        G_CGameManager()->PutPvp(room);
    }
