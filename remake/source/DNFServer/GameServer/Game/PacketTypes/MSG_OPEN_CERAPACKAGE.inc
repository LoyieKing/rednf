MSG_OPEN_CERAPACKAGE::MSG_OPEN_CERAPACKAGE()
{
    // ORIG C1：+0xe word 清零、memset(this+0x14,0,0x118)
    m_0e = 0;
    memset(m_items, 0, sizeof(m_items));
}