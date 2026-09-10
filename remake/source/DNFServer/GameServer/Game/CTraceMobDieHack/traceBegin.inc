void CTraceMobDieHack::traceBegin(long t, bool b)
{
    m_traceBeginTime = t;
    m_bTracing = 1;
    m_bInspectUser = b;
