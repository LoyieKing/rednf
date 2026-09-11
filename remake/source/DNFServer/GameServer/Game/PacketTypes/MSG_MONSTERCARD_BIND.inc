MSG_MONSTERCARD_BIND::MSG_MONSTERCARD_BIND()
{
    // ORIG C1：拷贝链 m_11←0 → m_0f ← m_11 → m_0d ← m_0f
    m_11 = 0;
    m_0f = m_11;
    m_0d = m_0f;
}