int TCPSocket::pollErrorEvent() const
{
    fd_set set;
    timeval waitTimeStr;
    int result;
    unsigned int i;
    fd_set* p = &set;
    for (i = 0; i <= 0x1f; i = i + 1)
    {
        p->fds_bits[i] = 0;
    }
    FD_SET((unsigned int)sock_, &set);
    waitTimeStr.tv_sec = 0;
    waitTimeStr.tv_usec = 0;
    result = select(2, NULL, NULL, &set, &waitTimeStr);
    if (result == 0)
    {
        return 0;
    }
    return 1;
