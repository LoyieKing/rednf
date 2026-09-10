CPacketRespondent::CPacketRespondent(CUser* user, int header, int subHeader)
    : PacketGuard()
{
    m_user = user;
    m_header = header;
    m_subHeader = subHeader;
    m_field18 = 0;
    m_autoSend = 0;
