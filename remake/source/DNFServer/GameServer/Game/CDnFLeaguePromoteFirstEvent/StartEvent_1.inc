// ---------------------------------------------------------------------------
// ORIG 0x0811c176：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（12=12）。
void CDnFLeaguePromoteFirstEvent::StartEvent(Word_Param param)
{
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
}