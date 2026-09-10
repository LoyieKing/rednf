void BingoEvent::StartEvent(Word_Param param)
{
    // b26 N 封顶（N7）：残差仅虚调用 callee 寄存器 edx↔eax 互换
    // （ORIG mov (%eax),%edx; call *%edx vs 我们 mov (%eax),%eax; call *%eax）。
    StartEvent();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x30, 0);
    trace("BingoEvent Event Start!");
