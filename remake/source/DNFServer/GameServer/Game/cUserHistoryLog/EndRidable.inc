void cUserHistoryLog::EndRidable(eRidableType type, int a)
{
    m_user->LogHistory("Transform,%d,%d,%d", 0, a, (int)type);
