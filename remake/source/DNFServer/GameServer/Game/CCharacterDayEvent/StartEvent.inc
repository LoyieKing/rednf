// [N5C_EventChar 2026-09-11] 旧判词 [b28d-D9]「N18a：两段中转不可复现（四形探针 a-d 全败）」**作废**：
// 两段中转是「字段与 RHS 异型」的产物（P211 档③ / P248 类型旋钮）。m_eventJob 声明改 short
// （源 param.m_duration 为 unsigned short）后复现 ORIG 0x0810a58e 的
// `movzwl 0xc(%ebp),%eax; mov %eax,%edx; mov %dx,0xa`（同型时 4.4 恒折 `movzwl 0xc(%ebp),%edx` 单段）。
// fresh 读数：AE 23/23 gap 0（TU identical=1 ae=7 near=0 diff=1，唯一 DIFF 为 N123 static_init 撞名）。
// 探针：/tmp/r5/n5/N5C_EventChar/probe_hetero.cpp（A 同型 3 条 / B,G 异型 4 条=ORIG 形）、
// probe3.cpp（GetEventJob 返回 unsigned short 仍 movzwl 直返；ctor `= -1` 仍 movw $0xffff）。
void CCharacterDayEvent::StartEvent()
{
    // ORIG 0x0810a53c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x17,0)(...)。
    SetEventFlag(true);
    cMyTrace(__FUNCTION__, 0x17, 0)("Character Day Event Start!");
}
