bool CMonitorServerProxy::OnTcpError()
{
    if (errno != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6b6, 0)
            ("Monitor Server OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}