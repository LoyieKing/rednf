int TCPSocket::pollReadEvent() const
{
    // ORIG DWARF 局部：set(255)/waitTimeStr(260)/result(265)；
    // result 先于 p 声明（P2 序）→ result@-0x24、p@-0x1c。
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
    result = select(2, &set, NULL, NULL, &waitTimeStr);
    if (result == 0)
    {
        return 0;
    }
    return 1;
