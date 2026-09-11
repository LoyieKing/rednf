// ============================================================================
// DB_TimerStatisticsLog::dispatch —— ORIG 0x08427c06（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8427c06：四个函数局部 static 实例（crash_down_report/
// stat_dungeon_fail/stat_pvp_play2/stat_use_shusia_service，各带 guard+
// atexit，P20 范式）依次 dispatch(userIdx, slot, stream)（返回值丢弃）；
// 最后 GetInstanceClientCrashDownLogStatistic()->CarryClientCrashDown()
// → 返 1。
// ============================================================================
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

    // [C-3/2026-09-11] ORIG 实有函数局部 static
    // `_ZZN21DB_TimerStatisticsLog8dispatchEiiP6StreamE5count`
    // （.bss 0x0943c228 size 1；全 ELF objdump 无任何引用点=死变量，原码遗留）
    // ⇒ 恢复声明以对齐符号集（未使用，零指令/零语义影响）。
    static char count;

    GetInstanceClientCrashDownLogStatistic()->CarryClientCrashDown();
    return 1;
}