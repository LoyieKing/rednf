// LevelDown(int, eLevelDownReason)：ORIG 0x86845de，"Level-,%d,%d"
void cUserHistoryLog::LevelDown(int level, eLevelDownReason reason)
{
    m_user->LogHistory("Level-,%d,%d", level, static_cast<int>(reason));
}