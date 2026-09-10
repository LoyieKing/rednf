void cUserHistoryLog::TradeItemAddFail(int a, int b)
{
    m_user->LogHistory("Item!,%d,%d", a, b);
