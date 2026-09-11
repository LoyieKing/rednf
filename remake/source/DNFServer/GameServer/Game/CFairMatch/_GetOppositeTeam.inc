// ORIG _GetOppositeTeam 0x8564f8c：1↔2，其余原样返回。
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
}