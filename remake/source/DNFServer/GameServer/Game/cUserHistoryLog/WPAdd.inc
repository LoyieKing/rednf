void cUserHistoryLog::WPAdd(int winPoint, int add, eWPAddReason reason)
{
    m_user->LogHistory("WP+,%d,%d,%d", winPoint, add, static_cast<int>(reason));
