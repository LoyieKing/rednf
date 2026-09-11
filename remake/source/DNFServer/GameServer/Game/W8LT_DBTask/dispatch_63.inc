// ============================================================================
// DB_UpdatePvPLiveEventData —— ORIG dispatch 0x0843ac48：空处理，恒返 1
// （mov $1,%eax; ret）。报告：docs/class_func_reports/
// DB_UpdatePvPLiveEventData/dispatch.md。
// ============================================================================
bool DB_UpdatePvPLiveEventData::dispatch(int userIdx, int slot, Stream* stream)
{
    return 1;
}