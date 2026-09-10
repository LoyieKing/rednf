void NewAccountLevelUpEventToJob::StartEvent(Word_Param /*param*/)
{
    // [N7 封顶][b28c-33 复核 22/22] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("NewAccountLevelUpEvent Event Start!");
