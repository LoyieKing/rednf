void cUserHistoryLog::FPAdd(int a, int b, int c, eFPAddReason reason)
{
    m_user->LogHistory("FP+,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
}