void IBuyRule::LogCubeStatistic(CUser* user, std::pair<int, int>& item)
{
    // ORIG 0x85fb8b2：
    // GetInstanceCubeStatistic()->collectCubeStatistics(
    //     item.first, item.second, user, (CUBE_STATISTIC_FIELD)0x6a)
    GetInstanceCubeStatistic()->collectCubeStatistics(
        item.first, item.second, user, (CUBE_STATISTIC_FIELD)0x6a);
