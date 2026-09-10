int CKillMonsterInfo::GetDungeonDifficultyFlag(unsigned char diff)
{
    if (diff > 4)
        return 0;
    return 1 << diff;
