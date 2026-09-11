// ============================================================================
// DB_LoadPowerWarStatueRanker::dispatch —— ORIG 0x08434878（mov $1; ret）
// ============================================================================
bool DB_LoadPowerWarStatueRanker::dispatch(int userIdx, int slot,
                                           Stream* stream)
{
    (void)userIdx;
    (void)slot;
    (void)stream;
    // ORIG 0x8434878：sub $0x40 空帧 + mov $1 + leave/ret（P31，
    // 实测 char[0x34..0x40] → sub $0x40）。
    char buf[0x38];
    (void)buf;
    // [N8W8LT P139] ORIG nm 有 prett 局部串（0x08c5bf40, sz 0x46：`virtual bool
    // DB_LoadPowerWarStatueRanker::dispatch(int, int, Stream*)`）而体无引用 ⇒ 原码有被
    // 丢弃的引用；sizeof 形只发射该串、零指令变化（探针实证）。
    (void)sizeof(__PRETTY_FUNCTION__);
    return 1;
}