void cUserHistoryLog::MoveArea(int a, int b, int c, int d, int e)
{
    m_user->LogHistory("MoveArea,%d,%d,%d,%d", a, b, c, d);
}