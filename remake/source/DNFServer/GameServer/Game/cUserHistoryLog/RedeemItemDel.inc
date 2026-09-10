void cUserHistoryLog::RedeemItemDel(int a, int b)
{
    m_user->LogHistory("RedeemItem-,%d,%d", a, b);
