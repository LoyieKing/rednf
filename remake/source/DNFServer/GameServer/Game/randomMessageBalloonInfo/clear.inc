void randomMessageBalloonInfo::clear()
{
    m_randomMessageProbability = 0;
    m_randomMessageList.clear();
    m_randomMessageList.reserve(10);
