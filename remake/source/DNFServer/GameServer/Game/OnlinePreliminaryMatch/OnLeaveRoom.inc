void COnlinePreliminary::OnLeaveRoom(CUser* user, PvP_Room* room,
                                     int teamGroup, int /*b*/)
{
    if (m_field04 == 4)
    {
        _SendMatchStatePacket(4, 2);
        m_field04 = 0;
        _ReturnTeam(room, room->get_manager_team(), user);
    }
    else if (m_field04 == 5)
    {
        if (room->GetTeamCount(teamGroup) == 1)
        {
            _UpdateRating(teamGroup, (ENUM_ELO_RATING_SYSTEM)0);
            _IncreaseLoseCount(_GetTeam((ENUM_SEAT_STATE)teamGroup));
        }
    }
    else
    {
        m_field04 = 0;
    }

    if (teamGroup == 1)
        m_teams[0].del(user);
    else
        m_teams[1].del(user);

    pWaitingRoomList_->DelRoom(this);
