void CBloodDungeonRewardSecondEvent::StartEvent(Word_Param param)
{
    // [N7 封顶][b28c-33 复核 16/16] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    // ORIG 0x0811bea4：movzwl 零扩展 param 低 ushort 写 +0x0c，再经 vptr 槽 2 虚调
    // StartEvent()。
    m_gold = (unsigned short)param.m_duration;
    StartEvent();
}