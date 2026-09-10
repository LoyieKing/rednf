void CCharacterDayEvent::StartEvent(Word_Param param)
{
    // ORIG 0x0810a58e：SetEventFlag(true)；movzwl 0xc(%ebp),%eax → mov %eax,%edx
    // → mov %dx,0xa 写 m_eventJob；cMyTrace(__FUNCTION__,0x1f,0)(...)。
    // [N5C_EventChar 2026-09-11] 旧判词「N18a 两段中转不可复现」**已推翻**：
    // 两段中转 = 字段与 RHS 异型的产物（P211 档③ / P248 类型旋钮）。字段在
    // CCharacterDayEvent.h 声明为 short（源 param.m_duration 为 unsigned short）
    // ⇒ 本件 AE 23/23 gap 0。探针：probe_hetero.cpp（同型 A=3 条 / 异型 B=4 条
    // 与 ORIG 同形）、probe3.cpp（GetEventJob 返回 unsigned short 仍 movzwl 直返、
    // ctor `= -1` 仍 movw $0xffff）。
    SetEventFlag(true);
    m_eventJob = param.m_duration;
    cMyTrace(__FUNCTION__, 0x1f, 0)("Character Day Event Start!");
}
