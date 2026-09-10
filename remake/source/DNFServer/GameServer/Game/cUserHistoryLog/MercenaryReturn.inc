void cUserHistoryLog::MercenaryReturn(unsigned int mercNo, int a, int b, int c, int d, int e)
{
    m_user->LogHistory("MecenaryReturn,%u,%d,%d,%d,%d,%d", mercNo, a, b, c, d, e);
