void cUserHistoryLog::StartRidable(eRidableType type, int a)
{
    m_user->LogHistory("Transform,%d,%d,%d", 1, a, (int)type);
