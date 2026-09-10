bool CPCRoomServerProxy::OnTcpError()
{
    if (errno != 0)
    {
        cMyTrace(__FUNCTION__, 0x17f, 0)
            ("PCRoomServer OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
