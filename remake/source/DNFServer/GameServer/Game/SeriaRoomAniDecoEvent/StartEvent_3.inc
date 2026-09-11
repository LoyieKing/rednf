// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081ba03a | P167 CLEAR（2/2）。
// 修法：P227 全表达式临时链（具名 trace 改 `cMyTrace(...)(fmt, a, b)`）⇒
// ORIG 的 `push esi/ebx` + ctor 前两条 movswl 跨 ctor 保活形逐条复现（35=35）。
void OneADayItemShopEvent::StartEvent(Word_Param)       // 0x081ba03a
{
    // ORIG：先虚调 vptr+0x10 槽（即 this->StartEvent()），再取 this+0x04/+0x06
    // （stADDINFO 两 ushort）符号扩展打印 AddInfo:%d,%d（0xffff → -1）。
    this->StartEvent();
    // P227 全表达式临时链：ORIG 在 ctor 前把两 vararg 求值到 %esi/%ebx 跨 ctor 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0x20, 0)(
        "OneADayItemShopEvent(AddInfo:%d,%d) Event Start!",
        static_cast<short>(m_addInfo.m_info0),
        static_cast<short>(m_addInfo.m_info1));
}
