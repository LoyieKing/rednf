// ============================================================================
// DB_FatigueResetTimer::dispatch —— ORIG 0x0841abbc（报告 dispatch.md）
// ============================================================================
// ORIG 恒返 1（mov $0x1,%eax; ret，无 DB 访问）。
bool DB_FatigueResetTimer::dispatch(int userIdx, int slot, Stream* stream)
{
    return 1;
}