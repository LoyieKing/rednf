void CEventStayTime::EndEvent()
{
    LogManager::logFormat(1, "localtaiwan/Event/EventStayTimeCharacter.cpp",
                          __PRETTY_FUNCTION__, 0x1b, "[CEventStayTime] End Event.");
    m_rewardStayTime = 0;
    m_intervalStayTime = 0;
    SetEventFlag(false);
