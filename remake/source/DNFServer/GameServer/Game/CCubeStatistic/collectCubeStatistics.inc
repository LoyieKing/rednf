// ---- collectCubeStatistics(int,int,CUser*,CUBE_STATISTIC_FIELD)（ORIG 0x860dfb8） ----
void CCubeStatistic::collectCubeStatistics(int itemIndex, int count, CUser* user,
                                           CUBE_STATISTIC_FIELD field)
{
    // ORIG：item=G_CDataManager()->find_item(itemIndex) 直调（无 manager NULL 防御）。
    // 两处 findCubeStuff 各自内联 getItemGroupName()（ORIG 重算两次，无 group 局部；
    // P0 槽-局部对应律禁缓存局部）。GetInstanceCubeStatistic() 也用两次。
    CItem* item = G_CDataManager()->find_item(itemIndex);
    if (item == 0)
        return;
    if (findCubeStuff(item->getItemGroupName()))
        sendCubeStatistic(itemIndex, count, user, field);
    if (GetInstanceCubeStatistic()->findCubeStuff(item->getItemGroupName()))
        GetInstanceCubeStatistic()->sendCubeStatistic(itemIndex, count, user, field);
}