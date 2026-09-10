Packet_Stat_Lag_Statistics::Packet_Stat_Lag_Statistics()
    : PacketHeader(0x17b9, 0x14c)   // ORIG PacketHeader(0x17b9, 0x14c)（arg1=0x17b9）
    , m_flagA(0)                    // movb $0x0,0xa(%eax)
    , m_flagB(0)                    // movb $0x0,0x5b(%eax)
{
    // m_modules[7]/m_dungeons[9] 子对象由成员声明自动构造
    //（ORIG 循环调 stModuleLagInfo/stDunGeonLagInfo ctor）。
