bool CUdpHandler::RecvFromClient(char* buf, int* size, unsigned int* addr,
                                unsigned short* port) const
{
    // b28d-solo-CUdpHandler 判词（AE）：ORIG 0x8483732 在 len=0x10 后立即
    // `lea -0x20(%ebp),%ebx` 物化第 6 实参 &len（4.1 右起求值）；4.4 对直传 `&len`
    // 与 P226 语句表达式恒延后到压栈点。复现形 = 纯 register 指针局部
    // `register socklen_t* plen = &len;`（P231/P248：无 asm 绑定、合法存储类提示，
    // 值留 %ebx 免槽且装载点提前）⇒ 逐条同形（clean 口径 AE；p167 调用点 8=8 CLEAR）。
    // 帧 0x44、槽 -0x20/-0x1c/-0xc 与 ORIG 相同。
    int err;
    sockaddr_in from;
    socklen_t len;
    if (m_serverSocket == -1)
    {
        return 0;
    }
    len = 0x10;
    // b28c-L10：ORIG 在 len=0x10 后立即 `lea -0x20(%ebp),%ebx` 物化 &len 跨 recvfrom
    // 调用（P226 裸语句表达式升格 callee-saved）；直传实参被 4.4 延后到压栈点。
    register socklen_t* plen = &len;
    *size = recvfrom(m_serverSocket, buf, *size, 0, (sockaddr*)&from,
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