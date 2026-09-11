// ============================================================================
// DB_LoadOnlinePreliminaryMember —— ORIG dispatch 0x0843ba94：空处理，恒返 1
// （mov $1,%eax; ret）。报告：
// docs/class_func_reports/DB_LoadOnlinePreliminaryMember/dispatch.md。
// ============================================================================
bool DB_LoadOnlinePreliminaryMember::dispatch(int userIdx, int slot,
                                              Stream* stream)
{
    return 1;
}