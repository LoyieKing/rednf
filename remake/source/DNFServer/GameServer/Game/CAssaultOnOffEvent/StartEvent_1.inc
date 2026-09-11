// ORIG 0x0811bb14（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
void CAssaultOnOffEvent::StartEvent(Word_Param /*param*/)
{
    // [b28c-27] N7 封顶：唯一残差=虚调用目标寄存器（ORIG 用 %edx，4.4 恒 %eax）。
    StartEvent();
}