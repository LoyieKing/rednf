Packet_Monitor_Char_Info::Packet_Monitor_Char_Info()
    : PacketHeader(0x3ef, 0x3f), m_dbid(0), m_channel(255), m_charNo(0),
      m_guildKey(0), m_job(255), m_grade(255), m_level(65535), m_memberKey(0),
      m_returnUser(0)
{
    memset(m_name, 0, sizeof(m_name));
