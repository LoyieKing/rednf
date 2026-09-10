void CActionPointManager::Reset()
{
    m_flag00 = 0;
    m_prevApSum = 0;
    memset(&m_sig, 0, sizeof(m_sig));
