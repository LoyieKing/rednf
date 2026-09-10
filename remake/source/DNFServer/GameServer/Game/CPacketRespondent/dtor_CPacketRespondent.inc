CPacketRespondent::~CPacketRespondent()
{
    if (m_autoSend != 0)
        Send();
