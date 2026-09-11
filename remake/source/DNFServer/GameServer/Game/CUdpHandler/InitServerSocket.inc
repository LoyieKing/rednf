int CUdpHandler::InitServerSocket(int port)
{
    int err;
    sockaddr_in addr;
    timeval rcvtimeo;
    m_serverSocket = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_serverSocket == -1)
    {
        err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    memset((char*)&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons((unsigned short)port);
    if (bind(m_serverSocket, (sockaddr*)&addr, 0x10) != 0)
    {
        err = getErrno();
        switch (err)
        {
        case 0x62:
            printf("Port %d for receiving UDP is in use\n", port);
            break;
        case 99:
            puts("Cannot assign requested address");
            break;
        // b28c-L10：ORIG 0x848362b 决策树尾 `test %eax,%eax; je 0x11f`（跳过 default 体）
        // ⇒ 原码有 case 0（空体，err==0 不打印）。补上后 4.4 复用 %eax 生成同一
        // test/je 尾，且 default 体不再需要 `if (err != 0)` 重载槽判断（消除
        // `cmpl $0x0,-0xc(%ebp); je` 伪差）。case 书写序照 ORIG 树序 0x62→99→0。
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                   err, strerror(err));
            break;
        }
        m_serverSocket = -1;
    }
    rcvtimeo.tv_sec = 10;
    rcvtimeo.tv_usec = 0;
    setsockopt(m_serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcvtimeo,
               sizeof(rcvtimeo));
    return m_serverSocket;
}