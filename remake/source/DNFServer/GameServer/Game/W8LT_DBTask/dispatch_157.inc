bool DB_TimerStatisticsLog::dispatch(int userIdx, int slot, Stream* stream)
{
    static DB_CrashDownReport crash_down_report;
    crash_down_report.dispatch(userIdx, slot, stream);

    static DB_StatDungeonFail stat_dungeon_fail;
    stat_dungeon_fail.dispatch(userIdx, slot, stream);

    static DB_StatPVPPlay2 stat_pvp_play2;
    stat_pvp_play2.dispatch(userIdx, slot, stream);

    static DB_StatUseShusiaService stat_use_shusia_service;
    stat_use_shusia_service.dispatch(userIdx, slot, stream);

    GetInstanceClientCrashDownLogStatistic()->CarryClientCrashDown();
    return 1;
