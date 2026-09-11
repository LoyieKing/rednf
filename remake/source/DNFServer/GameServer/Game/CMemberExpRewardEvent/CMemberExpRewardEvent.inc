// ORIG 0x081b2990：置标志；比值重置为 10；trace 行号 22。
// 事件进行中 GetMemberExpRewardRatio() 直返 +0xc（默认 10）。
// StartEvent()/EndEvent 均重置 m_ratio = 10（ORIG movl $0xa,0xc）。
// trace 采用 __FUNCTION__ 宏以对齐 ORIG _ZZN..E12__FUNCTION__ 符号。
// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x81b2b01 `_ZN21CMemberExpRewardEventC2Ev`。
CMemberExpRewardEvent::CMemberExpRewardEvent()
{
    m_ratio = 10;
}