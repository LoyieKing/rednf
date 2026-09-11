// ---------------------------------------------------------------------------
// ORIG 0x08116c04：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-31] N7：转调虚调用 callee ORIG %edx vs 4.4 %eax（12=12，仅寄存器名差）。
void CCollectArchieveEventLog::StartEvent(Word_Param param)
{
    StartEvent();
}