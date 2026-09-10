Packet_Stat_Compatibility_Index::Packet_Stat_Compatibility_Index()
    // ORIG W 0x0822c61e：PacketHeader(0x27e0,0x1e) 后逐字段 movl/movb/movw 置 0，
    // 置位顺序 = 成员声明顺序（+0xa accId → +0x1d osBit），故不得合并为 memset。
    : PacketHeader(0x27e0, 0x1e)   // ORIG _ZN12PacketHeaderC1Ett
{
    m_accId = 0;        // +0x0a movl
    m_cpuVendor = 0;    // +0x0e movb
    m_cpuNum = 0;       // +0x0f movb
    m_cpuClock = 0;     // +0x10 movl
    m_ram = 0;          // +0x14 movw
    m_videoVendor = 0;  // +0x16 movw
    m_videoDevice = 0;  // +0x18 movw
    m_videoRam = 0;     // +0x1a movw
    m_os = 0;           // +0x1c movb
    m_osBit = 0;        // +0x1d movb
