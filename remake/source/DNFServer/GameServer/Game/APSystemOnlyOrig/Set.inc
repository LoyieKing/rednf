void CActionPointManager::Set(_SIG_LOAD_ACTION_POINT& sig)
{
    m_prevApSum = m_sig.m_apSum;
    memcpy(&m_sig, &sig, sizeof(m_sig));
