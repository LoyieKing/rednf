MSG_ENCHANT::MSG_ENCHANT()
{
    // ORIG 0x822dc04：先写 +0x13/+0x15，再拷贝到 +0x0d/+0x0f，末尾 +0x19
    m_13 = 0;
    m_0d = m_13;
    m_15 = 0;
    m_0f = m_15;
    m_19 = 0;
}