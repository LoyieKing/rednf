// ORIG finalize @ 0x083403b8：+0x1c=1 后转发 InterfacePacketBuf::finalize(bool)。
void CPacketRespondent::finalize(bool encryptRequired)
{
    m_autoSend = 1;
    InterfacePacketBuf::finalize(encryptRequired);
}