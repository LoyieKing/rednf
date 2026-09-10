void cUserHistoryLog::QPAdd(int before, int questIdx, int add)
{
    m_user->LogHistory("QP+,%d,%d,%d", before, questIdx, add);
