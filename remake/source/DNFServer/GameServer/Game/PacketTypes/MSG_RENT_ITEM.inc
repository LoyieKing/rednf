MSG_RENT_ITEM::MSG_RENT_ITEM()
{  // ORIG C1：+0xd/+0xe byte 清零、memset(this+0xf,0,2)
    m_0d = 0;
    m_0e = 0;
    memset(m_0f, 0, sizeof(m_0f));
}