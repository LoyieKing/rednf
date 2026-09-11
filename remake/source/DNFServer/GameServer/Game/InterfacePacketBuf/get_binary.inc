bool InterfacePacketBuf::get_binary(char* out, int size)
{
    return m_packetBuf->get_binary(out, size);
}