bool BaseServerProxy::OnTcpError()
{
    if (*__errno_location() != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x19f, 0)
            ("%s OnError() Socket[%d] Error(%s)", GetServerName().c_str(),
             GetTcpHandler()->get_socket(), strerror(*__errno_location()));
        return 0;
    }
    return 1;
