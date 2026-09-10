void cUserHistoryLog::FPSub(int a, int b, int c, eFPSubReason reason)
{
    m_user->LogHistory("FP-,%d,%d,%d,%d", a, b, c, static_cast<int>(reason));
