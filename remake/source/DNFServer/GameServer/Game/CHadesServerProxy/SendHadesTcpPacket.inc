int CHadesServerProxy::SendHadesTcpPacket(char* packet)
{
    if (BaseServerProxy::GetSocketState() == ENUM_SOCKET_STATE_CONNECTED)
    {
        BaseServerProxy::SendTcpPacket(packet, ((PacketHeader*)packet)->packetSize);
    }
    return 1;
