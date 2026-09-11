// ORIG 0x08164684：m_field10 = param.m_duration（低 ushort），
// m_fieldC = param.m_field2（高 ushort），再经 vptr slot2 虚调 StartEvent()。
// [b28c-31] N7 复核：末尾虚调用 callee ORIG %edx vs 4.4 %eax（20=20）。
void CEventCeraShopRewardPoint::StartEvent(Word_Param param)
{
    // [N7 封顶] 末尾转调 StartEvent() 的虚调用 callee 寄存器 %edx↔%eax 互换。
    m_field10 = param.m_duration;
    m_fieldC = param.m_field2;
    StartEvent();
}