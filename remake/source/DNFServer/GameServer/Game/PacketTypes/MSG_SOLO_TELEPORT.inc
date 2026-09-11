MSG_SOLO_TELEPORT::MSG_SOLO_TELEPORT()
{  // ORIG 0x0822d764 调用链实证
    memset(m_0d, 0, sizeof(m_0d));  // +0xd..+0x2a
    m_2c = 0;
    m_30 = 0;
    m_34 = -1;      // +0x34 dword = 0xffffffff
    m_tp.reset();   // stPartyTelePort::reset @ +0x38（ORIG 出线调用）
    m_44 = 0;
}