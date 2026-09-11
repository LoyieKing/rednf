// ---------------------------------------------------------------------------
// ORIG 0x0811c29e：thunk——经 vtable slot 2（StartEvent()）虚调用。
void CDnFLeaguePromoteSecondEvent::StartEvent(Word_Param param)
{
    // [N7 封顶][b28c-33 复核 12/12] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
}