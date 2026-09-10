bool CPacketRespondent::SendAll()
{
    m_autoSend = 0;
    G_GameWorld()->send_all(*this);
    return true;
