void COnlinePreliminary::OnReward(PvP_Room* /*room*/, int loseTeamMark,
                                  bool isDrawWin)
{
    if (isDrawWin)
    {
        _UpdateRating(1, (ENUM_ELO_RATING_SYSTEM)1);
        _UpdateRating(2, (ENUM_ELO_RATING_SYSTEM)1);
        _IncreaseWinCount(_GetTeam((ENUM_SEAT_STATE)1));
        _IncreaseWinCount(_GetTeam((ENUM_SEAT_STATE)2));
    }
    else if (loseTeamMark == 1)
    {
        _UpdateRating(1, (ENUM_ELO_RATING_SYSTEM)2);
        _UpdateRating(2, (ENUM_ELO_RATING_SYSTEM)0);
        _IncreaseWinCount(_GetTeam((ENUM_SEAT_STATE)1));
        _IncreaseLoseCount(_GetTeam((ENUM_SEAT_STATE)2));
    }
    else
    {
        _UpdateRating(2, (ENUM_ELO_RATING_SYSTEM)2);
        _UpdateRating(1, (ENUM_ELO_RATING_SYSTEM)0);
        _IncreaseLoseCount(_GetTeam((ENUM_SEAT_STATE)1));
        _IncreaseWinCount(_GetTeam((ENUM_SEAT_STATE)2));
    }
}