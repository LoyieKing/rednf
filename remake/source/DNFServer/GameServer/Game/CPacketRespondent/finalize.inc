void CPacketRespondent::finalize(bool encryptRequired)
{
    m_autoSend = 1;
    InterfacePacketBuf::finalize(encryptRequired);
