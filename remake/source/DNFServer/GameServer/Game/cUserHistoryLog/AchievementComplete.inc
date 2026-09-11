void cUserHistoryLog::AchievementComplete(int idx)
{
    m_user->LogHistory("AchieveComplete,%d", idx);
}