// ---- collectCubeStatistics(CItem*,int,CUser*,CUBE_STATISTIC_FIELD)（ORIG 0x860e07a） ----
void CCubeStatistic::collectCubeStatistics(CItem* item, int count, CUser* user,
                                           CUBE_STATISTIC_FIELD field)
{
    // ORIG：item 非 NULL 且 findCubeStuff(item->getItemGroupName()) 命中 →
    // sendCubeStatistic(item->get_index(), count, user, field)。
    if (item == 0)
        return;
    if (findCubeStuff(item->getItemGroupName()))
        sendCubeStatistic(item->get_index(), count, user, field);
}