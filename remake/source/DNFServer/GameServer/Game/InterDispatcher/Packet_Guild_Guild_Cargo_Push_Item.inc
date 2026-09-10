Packet_Guild_Guild_Cargo_Push_Item::Packet_Guild_Guild_Cargo_Push_Item()
    : PacketHeader(0x70d, 0x53),
      m_guildKey(0), m_charNo(0), m_b(0), m_d(0), m_itemType(0), m_item()
{
    m_e = 0;
    m_f = 0;
