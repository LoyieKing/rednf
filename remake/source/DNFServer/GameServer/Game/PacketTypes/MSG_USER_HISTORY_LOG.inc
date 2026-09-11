MSG_USER_HISTORY_LOG::MSG_USER_HISTORY_LOG()
{
    // ORIG C1：+0xd byte、+0x10 int 清零、memset(this+0x14,0,0x100)
    m_0d = 0;
    m_10 = 0;
    memset(m_14, 0, sizeof(m_14));
}