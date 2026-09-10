bool TCPSocket::open()
{
    // ORIG: socket → fail path first (errno+printf) → success opts
    sock_ = socket(2, 1, 6);
    if (sock_ == -1)
    {
        int err = errno;
        printf("Could not create a TDP socket : %d\n", err);
        return false;
    }
    setOptResizeSendBuf(G_Script()->findIntValue(0, 0xb));
    setOptResizeRecvBuf(G_Script()->findIntValue(0, 10));
    setOptLinger(false);
    setOptNagle(false);
    return true;
