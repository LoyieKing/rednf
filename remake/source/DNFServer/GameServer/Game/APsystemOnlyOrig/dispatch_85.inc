// ============================================================================
// DB_UpdateLimitItemUsage::dispatch —— ORIG 0x0843f022
// ============================================================================
// ORIG 0x843f022：sub $0x20 空帧 + mov $1,%eax + leave/ret —— 恒返 1。
bool DB_UpdateLimitItemUsage::dispatch(int userIdx, int slot, Stream* stream)
{
    // 未用局部压出 ORIG 帧宽（P31；实测 char[0x14..0x20] → sub $0x20 + leave）。
    char buf[0x18];
    (void)buf;
    return 1;
}