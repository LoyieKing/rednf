// ORIG 0x08437756：空处理，恒返回 1（mov $1,%eax; ret）。ORIG 帧 sub $0x40
// 为编译器伪影（函数体无局部访问），保留一个未用局部对齐帧（P31）。
bool DB_LoadPowerWarInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    char buf[0x40];
    (void)buf;
    // [N8W8LT P139] ORIG 0x08437756 体仅 13B（`sub $0x40 / mov $1 / leave / ret`），
    // 但 ORIG nm 有本函数 prett 局部串 `_ZZN19DB_LoadPowerWarInfo8dispatchEiiP6Stream
    // E19__PRETTY_FUNCTION__`（0x08c5bd80, sz 0x3e，内容 `virtual bool
    // DB_LoadPowerWarInfo::dispatch(int, int, Stream*)`）⇒ 原码有被丢弃的引用。
    // 探针实证：`(void)sizeof(__PRETTY_FUNCTION__)` 只发射该串、零指令变化。
    (void)sizeof(__PRETTY_FUNCTION__);
    return 1;
}