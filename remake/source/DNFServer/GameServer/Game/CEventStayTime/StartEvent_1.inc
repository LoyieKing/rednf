void CEventStayTime::StartEvent(Word_Param param)
{
    m_rewardStayTime = static_cast<unsigned int>(param.m_duration) * 60u;
    m_intervalStayTime = static_cast<unsigned int>(param.m_field2) * 60u;
    this->StartEvent();
