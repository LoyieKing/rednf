bool CHadesServerProxy::IsConnect()
{
    if (BaseServerProxy::GetSocketState() == ENUM_SOCKET_STATE_CONNECTED)
    {
        return 1;
    }
    return 0;
