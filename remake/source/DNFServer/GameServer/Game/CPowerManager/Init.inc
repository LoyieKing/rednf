bool CPowerManager::Init()
{
    // ORIG 0x0847eabc：清零 +4,+8,+0x14,+0x18,+0xc,+0x10,+0x1c,+0x20，
    // memset (+0x24,4)、memset (+0x28,0x18c)、Reset 两日志，return 1。
    m_side1Point = 0;
    m_side2Point = 0;
    m_side1UserCount = 0;     // +0x14（ORIG 先清 +0x14/+0x18 再清 +0xc/+0x10）
    m_side2UserCount = 0;     // +0x18
    m_side1ChannelPoint = 0;  // +0x0c
    m_side2ChannelPoint = 0;  // +0x10
    m_field1c = 0;
    m_field20 = 0;
    memset(m_powerWarStartTime, 0, sizeof(m_powerWarStartTime));
    memset(m_rankings, 0, sizeof(m_rankings));
    m_powerWarLog.Reset();
    m_powerWarPacketLog.Reset();
    return 1;
