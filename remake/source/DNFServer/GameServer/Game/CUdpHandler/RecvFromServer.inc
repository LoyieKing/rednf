bool CUdpHandler::RecvFromServer(char* buf, int* size, unsigned int* addr,
                                unsigned short* port) const
{
    // b28d-solo-CUdpHandler 判词（AE）：同 RecvFromClient —— ORIG 0x8483a52 先把
    // &len 物化到 %ebx（P231 纯 register 指针局部形复现，无 asm 绑定），
    // 4.4 直传 `&len`/语句表达式恒延后；现逐条同形（clean 口径 AE，p167 8=8 CLEAR）。
    int err;
    sockaddr_in from;
    socklen_t len;
    if (m_clientSocket == -1)
    {
        return 0;
    }
    len = 0x10;
    register socklen_t* plen = &len;
    *size = recvfrom(m_clientSocket, buf, *size, 0, (sockaddr*)&from,
                     plen);
    if (*size == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", err);
            break;
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    buf[*size] = '\0';
    return 1;
}