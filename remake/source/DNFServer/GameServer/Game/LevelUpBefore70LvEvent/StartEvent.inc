// ORIG 0x0811ffca：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x12,0)。
// 日志名用 __FUNCTION__（ORIG .rodata 符号 _ZZN22...E12__FUNCTION__ 实证）。
void LevelUpBefore70LvEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x12, 0);
    trace("LevelUpBefore70Lv Event start!");
}