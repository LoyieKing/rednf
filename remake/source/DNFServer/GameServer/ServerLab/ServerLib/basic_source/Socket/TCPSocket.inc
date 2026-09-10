TCPSocket::TCPSocket()
{
    sock_ = -1;
    mSendRetryCount = 0;
    memset(c_adrs_, 0, 4);
    port_ = 0;
