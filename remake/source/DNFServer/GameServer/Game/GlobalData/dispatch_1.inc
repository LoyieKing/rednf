void UdpThread::dispatch(void* arg)
{
    (void)arg;
    char buf[0x1800];
    fd_set readfds;
    int maxfd;
    struct timeval timeout;
    int sockets[1];
    int recvLen;
    int nfds;

    sockets[0] = _bindUDPSocket((unsigned short)G_CEnvironment()->get_udp_port());
    g_bUdp_TH_start = true;
    while (!GlobalData::s_server_stop)
    {
        memset(buf, 0, sizeof(buf));
        // FD_ZERO(&readfds) 的 ORIG 4.1-glibc 展开形（逐元素循环；见函数头判词）。
        // [L8] 槽序：ORIG p(-0x24)/i(-0x28)；且 ORIG 尾测为 `cmpl $0x1f,-0x28;
        // setbe`（**无符号**比较）⇒ 计数变量须 unsigned（我方 int 版产 setle）。
        unsigned int fi;
        int* fdBits = (int*)&readfds;
        for (fi = 0; fi <= 0x1f; ++fi)
            fdBits[fi] = 0;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        maxfd = 0;
        for (int i = 0; i <= 0; ++i)
        {
            // [L8] ORIG 0x0857ca90 段 `mov -0x3c(%ebp,%eax,4),%eax; shr $0x5,%eax`
            // 为**逻辑**右移（无 `lea 0x1f/test/cmovs` 修正）⇒ FD_SET 的实参须按
            // unsigned 参与 `__FDELT(d)=d/32` 折叠；而 max 侧按 int 左值传址
            // （0x0857cac6 `lea -0x3c(%ebp)`）⇒ 各处分别 cast。
            FD_SET((unsigned int)sockets[i], &readfds);
            maxfd = std::max(maxfd, sockets[i]);
        }
        maxfd += 1;
        nfds = select(maxfd, &readfds, NULL, NULL, &timeout);
        if (nfds < 0)
        {
            continue;
        }
        for (int i = 0; i <= 0; ++i)
        {
            if (FD_ISSET((unsigned int)sockets[i], &readfds))
            {
                recvLen = recv(sockets[i], buf, sizeof(buf), 0);
                if (recvLen > 0)
                {
                    _processUDPData(buf, recvLen);
                }
            }
        }
    }
