int CExchangeServer::OnRecvMessageServerInfo(Stream* stream, int a,
                                             int b)
{
    // [b28d-48] N82 封顶（NDIFF 134，140/142 条）：帧/槽平移 +
    // callee-saved 组差（push edi/esi/ebx vs esi/ebx）。
    (void)a;
    (void)b;
    m_flag39 = true;

    char* buf = stream->get_buffer();
    const ExchangeServerInfoMsgView* sig =
        (const ExchangeServerInfoMsgView*)buf;

    // ORIG 0x8493202/0x849320f：ip 先入具名局部（-0x74，供 inet_ntoa），
    // m_field00 再独立重读一次（P48 O0 无 CSE）。P211 档③：视图字段 unsigned
    // 与 int 目标异型 ⇒ 复现 ORIG `mov 0x10(%eax),%eax; mov %eax,%edx` 中转。
    struct in_addr ipv;
    ipv.s_addr = sig->m_ip10;
    m_localInfo.m_field00 = sig->m_ip10;
    m_localInfo.m_word04 = sig->m_port0e;

    {
        // P227 全表达式临时链：ORIG 0x8493216-0x8493284 在 ctor 前求值
        // channelNo/port/inet_ntoa 并以 %edi/%esi/%ebx 跨 ctor 保活。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvMessageServerInfo(Stream*, int, int)", 0x343, 0)
            ("Exchange Server IP:%s, PORT:%d, Channel No:%d",
             inet_ntoa(ipv),
             (int)(short)sig->m_port0e, sig->m_channel0a);
    }

    if (sig->m_channel0a == GetServerChannelNo())
    {
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvMessageServerInfo(Stream*, int, int)", 0x347, 0)
            ("This is Exchange Master Server");
        m_flag38 = true;

        // [b28d-solo-Exchange 真 bug] ORIG 0x84932ef `je 8493343` 跳过 trace 直达
        // SetServerType ⇒ SetServerType(MASTER) 在内层 if 之外（旧码写在 if 内，
        // 端口相同时漏调 SetServerType）。
        if (sig->m_port0e != (unsigned short)m_port08)
        {
            cMyTrace("int exchange_server::CExchangeServer::"
                     "OnRecvMessageServerInfo(Stream*, int, int)", 0x34b, 0)
                ("different server port (%d,%d) ",
                 (int)(short)m_port08, (int)(short)sig->m_port0e);
        }
        SetServerType(EXCHANGE_SERVER_MASTER);
    }
    else
    {
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvMessageServerInfo(Stream*, int, int)", 0x351, 0)
            ("This is Exchange Slave Server");

        if (!m_sessionMgr.TryConnect(
                inet_ntoa(ipv),
                (int)(short)sig->m_port0e))
        {
            cMyTrace("int exchange_server::CExchangeServer::"
                     "OnRecvMessageServerInfo(Stream*, int, int)", 0x354, 0)
                ("cannot connect to other game server!");
        }
        SetServerType(EXCHANGE_SERVER_SLAVE);
    }
    return 0;
