int DisPatcher_SetPVPTeam::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 6)
    {
        return 0;
    }
    PacketGuard guard;
    char team;
    if (!packet.get_byte(team))
    {
        return LineFunc(0x82be,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x82c3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (team == 4)
    {
        PvPMapInfo info;
        int mapIdx = room->GetRequestedMapNo();
        CMapList::GetPvpMapInfo(mapIdx, info);
        if (!info.m_playable)
        {
            return LineFunc(0x82ca,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    int result = room->set_pvp_mode(user, (int)(signed char)team);
    if (result < 1)
    {
        room->make_seat_info(reinterpret_cast<char*>(&guard), 0xffff);
        G_GameWorld()->send_all(guard);
        return 0;
    }
    guard.put_header(1, 0x39);
    guard.put_byte(0);
    guard.put_byte(result);
    guard.finalize(true);
    user->Send(guard);
    return 0;
