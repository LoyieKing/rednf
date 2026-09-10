bool CUdpHandler::SendToServer(char* buf, int len, unsigned short port,
                              const char* ip) const
{
    sockaddr_in to;
    int sent;
    int err;
    if (m_clientSocket == -1)
    {
        // b28c-L10：P221 全表达式临时链——ORIG 槽序 0x194→-0x70 最深，4.4 具名对象
        // 反装为 -0x30 最浅；临时链复现 ORIG 槽深方向。
        cMyTrace(__FUNCTION__, 0x194, 5)("[%s][%d]", __FUNCTION__, 0x194);
        return 0;
    }
    else if (m_clientSocket == -1)
    {
        return 0;
    }
    if (port == 0)
    {
        if (ip == 0)
        {
            sent = send(m_clientSocket, buf, len, 0);
        }
    }
    else
    {
        // b28c-L10：P68 memset 首参 cast 触发 lea 先行（ORIG `lea -0x80(%ebp),%eax`
        // 在 `movl $0x10,0x8(%esp)` 之前；无 cast 时 4.4 右到左先压常量）。
        memset((char*)&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = inet_addr(ip);
        sent = sendto(m_clientSocket, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
        {
            cMyTrace(__FUNCTION__, 0x1ca, 5)
                ("CUdpHandler::SendToServer Error: EHOSTUNREACH");
            break;
        }
        case 0x61:
        {
            cMyTrace(__FUNCTION__, 0x1d1, 5)
                ("CUdpHandler::SendToServer Error: EAFNOSUPPORT");
            break;
        }
        default:
        {
            cMyTrace(__FUNCTION__, 0x1d8, 5)
                ("CUdpHandler::SendToServer Error : other err");
            break;
        }
        }
        return 0;
    }
    if (sent == 0)
    {
        cMyTrace(__FUNCTION__, 0x1e2, 5)("CUdpHandler::SendToServer Error: 0 == s");
        return 0;
    }
    if (len != sent)
    {
        cMyTrace(__FUNCTION__, 0x1ea, 5)("CUdpHandler::SendToServer Error : len != s");
        return 0;
    }
    else
    {
        return 1;
    }
