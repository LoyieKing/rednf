bool CPacketRespondent::Send()
{
    m_autoSend = 0;
    return m_user->Send(*this);
