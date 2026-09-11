void cUserHistoryLog::ChangeGrowType(int a, int b, int c, int d, int e, eChangeGrowTypeReason reason)
{
    m_user->LogHistory("CP,%d,%d,%d,%d,%d,%d", a, b, c, d, e, static_cast<int>(reason));
}