// ORIG 0x816be18：(param 低字)*60 → +0xc、(param 高字)*60 → +0x10，
// 然后经 vptr+0x8 虚调 StartEvent()。
void CEventStayTime::StartEvent(Word_Param param)
{
    m_rewardStayTime = static_cast<unsigned int>(param.m_duration) * 60u;
    m_intervalStayTime = static_cast<unsigned int>(param.m_field2) * 60u;
    this->StartEvent();
}