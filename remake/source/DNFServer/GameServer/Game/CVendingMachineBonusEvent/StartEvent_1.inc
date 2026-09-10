void CVendingMachineBonusEvent::StartEvent(Word_Param param)
{
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
