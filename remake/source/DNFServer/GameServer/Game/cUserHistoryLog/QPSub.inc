void cUserHistoryLog::QPSub(int before, int questIdx, int sub)
{
    m_user->LogHistory("QP-,%d,%d,%d", before, questIdx, sub);
