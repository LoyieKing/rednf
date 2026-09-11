// Set —— ORIG T 0x08121b0c objdump：prev ← m_sig.m_apSum 后复制完整 packed 包。
void CActionPointManager::Set(_SIG_LOAD_ACTION_POINT& sig)
{
    m_prevApSum = m_sig.m_apSum;
    memcpy(&m_sig, &sig, sizeof(m_sig));
}