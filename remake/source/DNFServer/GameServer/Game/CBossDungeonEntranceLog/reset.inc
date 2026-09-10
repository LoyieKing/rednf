void CBossDungeonEntranceLog::reset()
{
    // ORIG 0x08145746：tail-call map::clear。
    m_logMap.clear();
