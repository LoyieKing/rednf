// ============================================================================
// DB_UpdatePvPBuddy —— ORIG dispatch 0x0843a9b6：空处理，恒返 1
// （mov $1,%eax; ret）。报告：docs/class_func_reports/DB_UpdatePvPBuddy/
// dispatch.md。
// ============================================================================
bool DB_UpdatePvPBuddy::dispatch(int userIdx, int slot, Stream* stream)
{
    return 1;
}