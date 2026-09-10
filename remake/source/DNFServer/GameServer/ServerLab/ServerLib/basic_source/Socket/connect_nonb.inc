bool TCPSocket::connect_nonb(const char* ip, unsigned short port, timeval tval)
{
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_addr.s_addr = inet_addr(ip);
    svradr.sin_port = htons(port);
    int len = 0x10;
    setOptNonBlock();
    int n;
    int error = 0;
    (void)error;
    // ORIG: 赋值在条件内（mov n; shr $0x1f; test; je）
    if ((n = ::connect(sock_, (sockaddr*)&svradr, len)) < 0)
    {
        if ((errno != EINPROGRESS) && (errno != EINTR) && (errno != EAGAIN) && (errno != EAGAIN))
        {
            return false;
        }
    }
    if (n != 0)
    {
        fd_set wset;
        unsigned int i;
        fd_set* p = &wset;
        for (i = 0; i <= 0x1f; i = i + 1)
        {
            p->fds_bits[i] = 0;
        }
        FD_SET((unsigned int)sock_, &wset);
        // ORIG: 赋值在条件内（n<0 && errno!=EINTR 物化为 0/1：mov $1; jmp; mov $0; test; je）
        if ((n = select(sock_ + 1, NULL, &wset, NULL, &tval)) < 0 && errno != EINTR)
        {
            return false;
        }
        if (n >= 1)
        {
            socklen_t lon = 4;
            int valopt;
            // ORIG（二进制实测）：&valopt 先入 edx，再载 sock_，&lon 最后（lea ecx）。
            // (char*)&valopt 强转让 GCC 提前物化地址，复现 ORIG 的求值顺序。
            if (getsockopt(sock_, 1, 4, (char*)&valopt, &lon) < 0)
            {
                return false;
            }
            if (valopt != 0)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    memcpy(c_adrs_, &svradr.sin_addr, 4);
    port_ = svradr.sin_port;
    return true;
