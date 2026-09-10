void Statistics::DungeonFail(int dungeonIdx)
{
    G_GameWorld()->getStatisticsShusiaService()->incDungeonFailure(dungeonIdx);
