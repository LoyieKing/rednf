int Dispatcher_TraceError::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char errorType = 0;
    if (!packet.get_byte(errorType))
    {
        return LineFunc(0x9c74,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (errorType == 2)
    {
        char errorCode = 0;
        if (!packet.get_byte(errorCode))
        {
            return LineFunc(0x9cb3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x9cb5, 0);
        trace("KILLBOSS2 client - error type(%d)", (int)errorCode);
    }
    else if (errorType == 3)
    {
        return LineFunc(0x9c85,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
