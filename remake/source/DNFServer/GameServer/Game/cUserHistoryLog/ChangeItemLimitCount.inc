void cUserHistoryLog::ChangeItemLimitCount(int a, int b, int itemIdx, int d, int e)
{
    if (m_user != 0)
        m_user->LogHistory("ChangeItemTradeLimitCount,%d,%d,%d,%d,%d", a, b, itemIdx, d, e);
    return;
