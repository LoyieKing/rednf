// ============================================================================
// DB_LoadPvPLiveEventData —— ORIG dispatch 0x0843ac3e：空处理，恒返 1
// （mov $1,%eax; ret）。报告：docs/class_func_reports/
// DB_LoadPvPLiveEventData/dispatch.md。
// ============================================================================
bool DB_LoadPvPLiveEventData::dispatch(int userIdx, int slot, Stream* stream)
{
    return 1;
}