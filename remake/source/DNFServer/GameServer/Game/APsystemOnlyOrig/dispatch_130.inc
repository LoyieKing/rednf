// ============================================================================
// DB_SuicideWhenQueueIsEmpty::dispatch —— ORIG 0x0841b206（报告 dispatch.md）
// ============================================================================
// ORIG 恒返 1（mov $0x1,%eax; ret，无 DB 访问）。
bool DB_SuicideWhenQueueIsEmpty::dispatch(int userIdx, int slot, Stream* stream)
{
    return 1;
}