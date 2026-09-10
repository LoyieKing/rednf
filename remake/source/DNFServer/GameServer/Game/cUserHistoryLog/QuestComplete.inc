void cUserHistoryLog::QuestComplete(int questIdx)
{
    m_user->LogHistory("QuestComplete,%d", questIdx);
