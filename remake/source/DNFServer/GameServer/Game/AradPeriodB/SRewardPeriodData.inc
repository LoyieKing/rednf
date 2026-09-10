ARAD::SCRIPT::SRewardPeriodData::SRewardPeriodData(
    int eventCode, unsigned long f4, short f8,
    unsigned int beginTime, unsigned int endTime)
{
    m_eventCode = eventCode;
    m_field4    = (unsigned int)f4;
    m_field8    = f8;
    m_beginTime = beginTime;
    m_endTime   = endTime;
