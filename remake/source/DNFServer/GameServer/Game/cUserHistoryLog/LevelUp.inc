void cUserHistoryLog::LevelUp(int level, eLevelUpReason reason)
{
    m_user->LogHistory("Level+,%d,%d", level, static_cast<int>(reason));
