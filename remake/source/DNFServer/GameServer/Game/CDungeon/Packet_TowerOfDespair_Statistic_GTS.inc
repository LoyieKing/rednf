Packet_TowerOfDespair_Statistic_GTS::Packet_TowerOfDespair_Statistic_GTS()
    : m_header(0x271c, 0x17)   // 成员序：header → key(默认构造) → flag/accId
{
    m_flag = 0;
    m_accId = 0;
