// ---------------------------------------------------------------------------
// ORIG 0x0811c04e：vtable+0x8 直调 StartEvent()（转发无参版本）。
// [b28c-31] N7 复核：转调虚调用 callee ORIG %edx vs 4.4 %eax（12=12）。
void CPCRoomWorldDropEvent2nd::StartEvent(Word_Param param)
{
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换，探针不可翻。
    StartEvent();
}