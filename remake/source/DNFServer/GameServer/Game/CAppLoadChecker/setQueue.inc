void CAppLoadChecker::setQueue(int dbq, int logdbq, int timerq, int packetq)
{
    m_baseDbq = dbq;
    m_baseLogdbq = logdbq;
    m_baseTimerq = timerq;
    m_basePacketq = packetq;
