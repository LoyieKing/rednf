// ORIG D1 @ 0x0834036a：m_autoSend 非零则补发一次 Send，随后 ~PacketGuard()。
CPacketRespondent::~CPacketRespondent()
{
    if (m_autoSend != 0)
        Send();
}