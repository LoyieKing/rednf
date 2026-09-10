bool TCPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    // ORIG has unused locals matching old getsockopt leftovers；
    // optlen 先声明（槽 -0x14）但不初始化，unused 后声明（槽 -0x10），
    // optlen=4 在 unused=0 之后发射（optlen 赋值为体中语句）。
    int optlen;
    int unused = 0;
    (void)unused;
    (void)optlen;
    optlen = 4;
    int ret = setsockopt(sock_, 1, 7, &size, 4);
    if (ret < 0)
    {
        return false;
    }
    return true;
