void cUserHistoryLog::RequestCleanPad(unsigned int a, char b)
{
    m_user->LogHistory("CleanPad,%d,%d", a, static_cast<int>(b));
