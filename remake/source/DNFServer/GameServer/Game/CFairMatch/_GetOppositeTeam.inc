int CFairMatch::_GetOppositeTeam(int team)
{
    switch (team)
    {
    case 1:
        return 2;
    case 2:
        return 1;
    default:
        return team;
    }
