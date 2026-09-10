MSG_ITEM_UPGRADE::MSG_ITEM_UPGRADE()
{
    // ORIG C1：先 +0x25 word=0，memset(this+0x28,0,0x81)，末尾 +0x27 byte=0
    m_25 = 0;
    memset(m_28, 0, sizeof(m_28));
    m_27 = 0;
