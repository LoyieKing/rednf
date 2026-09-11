// ORIG C1 @ 0x0834032a：PacketGuard 基类构造后依次写
// +0x0c=user、+0x10=header、+0x14=subHeader、+0x18=0、+0x1c(byte)=0。
CPacketRespondent::CPacketRespondent(CUser* user, int header, int subHeader)
    : PacketGuard()
{
    m_user = user;
    m_header = header;
    m_subHeader = subHeader;
    m_field18 = 0;
    m_autoSend = 0;
}