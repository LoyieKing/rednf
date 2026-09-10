void Phase::reset()
{
    // ORIG T 0x08899cf8：仅 m_field0 清零 + m_assigns.clear()（+0x04 不清）。
    m_field0 = 0;
    m_assigns.clear();
