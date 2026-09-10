void cUserHistoryLog::ClearUsedQP(int before, int after, int initCount, int unused)
{
    m_user->LogHistory("ClearQP,%d,%d,%d,%d", before, after, initCount, unused);
