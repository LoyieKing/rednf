CPowerManager::CPowerManager()
    : m_winnerSide(0), m_side1Point(0), m_side2Point(0),
      m_side1ChannelPoint(0), m_side2ChannelPoint(0),
      m_side1UserCount(0), m_side2UserCount(0), m_field1c(0)
{
    // ORIG 0x0847e998：初始化列表按声明序清 +0..+1c（不写 +0x20）；编译器按
    // 声明序前置成员构造——m_rankings[3] 循环调 STPowerWarStatueRankerInfo::C1、
    // CPowerWarLog::C1、CPowerWarPacketLog::C1（与 ORIG 顺序一致）；
    // 函数体再 memset (+0x24,4)、memset (+0x28,0x18c)，最后 Reset 两日志子对象。
    memset(m_powerWarStartTime, 0, sizeof(m_powerWarStartTime));
    memset(m_rankings, 0, sizeof(m_rankings));
    m_powerWarLog.Reset();
    m_powerWarPacketLog.Reset();
