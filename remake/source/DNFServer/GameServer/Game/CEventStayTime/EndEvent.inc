// ORIG 0x816be60：logFormat(1, file, __PRETTY_FUNCTION__, 0x1b, msg) 先行，
// 再清零 0xc/0x10，最后 SetEventFlag(false)。
void CEventStayTime::EndEvent()
{
    LogManager::logFormat(1, "localtaiwan/Event/EventStayTimeCharacter.cpp",
                          __PRETTY_FUNCTION__, 0x1b, "[CEventStayTime] End Event.");
    m_rewardStayTime = 0;
    m_intervalStayTime = 0;
    SetEventFlag(false);
}