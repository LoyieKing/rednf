// LevelUp(int, eLevelUpReason)：ORIG 0x86845b2，"Level+,%d,%d"
void cUserHistoryLog::LevelUp(int level, eLevelUpReason reason)
{
    m_user->LogHistory("Level+,%d,%d", level, static_cast<int>(reason));
}