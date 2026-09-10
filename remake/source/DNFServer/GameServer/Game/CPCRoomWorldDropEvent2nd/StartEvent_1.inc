void CPCRoomWorldDropEvent2nd::StartEvent(Word_Param param)
{
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换，探针不可翻。
    StartEvent();
