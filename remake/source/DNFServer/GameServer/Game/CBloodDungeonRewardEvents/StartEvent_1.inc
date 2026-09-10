void CBloodDungeonRewardFirstEvent::StartEvent(Word_Param param)
{
    // [N7 封顶][b28c-33 复核 16/16] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    // ORIG 0x0811bd64：movzwl 零扩展 param 低 ushort 写 +0x0c，再经 vptr 槽 2 虚调
    // StartEvent()。成员内未限定调用即为虚分派，与 ORIG call *vptr+8 一致。
    m_gold = (unsigned short)param.m_duration;
    StartEvent();
