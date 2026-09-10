void CDimensionActivationEvent::StartEvent(Word_Param param)
{
    // ORIG 0x080eeb4a：忽略参数，经 vtable+0x08 转调 StartEvent()。
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    (void)param;
    StartEvent();
