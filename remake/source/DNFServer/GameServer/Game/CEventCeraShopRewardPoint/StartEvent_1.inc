void CEventCeraShopRewardPoint::StartEvent(Word_Param param)
{
    // [N7 封顶] 末尾转调 StartEvent() 的虚调用 callee 寄存器 %edx↔%eax 互换。
    m_field10 = param.m_duration;
    m_fieldC = param.m_field2;
    StartEvent();
