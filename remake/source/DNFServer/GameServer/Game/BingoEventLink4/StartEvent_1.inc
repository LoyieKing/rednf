// StartEvent(Word_Param) @ 0x080c9b68：经 vptr+0x8（slot 2）调无参 StartEvent()，
// 打日志。param 未参与计算（ORIG 如此）。
void BingoEvent::StartEvent(Word_Param param)
{
    // b26 N 封顶（N7）：残差仅虚调用 callee 寄存器 edx↔eax 互换
    // （ORIG mov (%eax),%edx; call *%edx vs 我们 mov (%eax),%eax; call *%eax）。
    StartEvent();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x30, 0);
    trace("BingoEvent Event Start!");
}