void cUserHistoryLog::WPSub(int cur, int sub, eWPSubReason reason)
{
    m_user->LogHistory("WP-,%d,%d,%d", cur, sub, static_cast<int>(reason));
