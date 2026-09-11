void cUserHistoryLog::RedeemItemAdd(int a, int b)
{
    m_user->LogHistory("RedeemItem+,%d,%d", a, b);
}