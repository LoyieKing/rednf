int COnlinePreliminary::_GetOppositeTeam(int team)
{
    /* ORIG @08561f69 比较链 cmp $1/%eax; je; cmp $2; je; jmp def（4.1 switch
     * 下探形【技巧34】） */
    switch (team)
{
    case 1:
        return 2;
    case 2:
        return 1;
    default:
        break;
