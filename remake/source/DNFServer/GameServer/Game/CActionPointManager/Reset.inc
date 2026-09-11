// ORIG 0x08122af4（W）：m_flag00=0；m_prevApSum=0；
// memset(&m_sig, 0, sizeof(_SIG_LOAD_ACTION_POINT))。无负标记扫描循环。
void CActionPointManager::Reset()
{
    m_flag00 = 0;
    m_prevApSum = 0;
    memset(&m_sig, 0, sizeof(m_sig));
}