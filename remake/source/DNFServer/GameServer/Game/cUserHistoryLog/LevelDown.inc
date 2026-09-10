void cUserHistoryLog::LevelDown(int level, eLevelDownReason reason)
{
    m_user->LogHistory("Level-,%d,%d", level, static_cast<int>(reason));
