void Statistics::UseShusiaService(int value)
{
    G_GameWorld()->getStatisticsShusiaService()->incServiceUsage(value);
}