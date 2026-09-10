int CExchangeServer::OnRecvPacketServerInfo(CSession* session,
                                            PacketGuard& guard)
{
    // [P330 本泳道] ORIG 0x08492860 入口**无** serverId 的零初始化（直接
    // `lea -0x4c(%ebp); get_int`）⇒ 原码是不带初值的局部，旧 `= 0` 多发一条
    // movl 且把后续槽位整体推偏。
    ExchangeServerInfo info;                          // ORIG 槽 -0x5c
    int serverId;                                     // ORIG 槽 -0x4c（无初值）

    if (!guard.get_int(serverId))                     // line 0x29d
    {
        return 0x29d;
    }
    if (!guard.get_int(info.m_field00))               // line 0x29e
    {
        return 0x29e;
    }
    // P0：ORIG 无 port16 槽（0x84928a0 `lea -0x5c(%ebp); add $0x4,%edx` 直传
    // &info.m_word04）⇒ 直接以 m_word04 为 get_short 出参（InterfacePacketBuf
    // 仅有 short& 重载）。
    if (!guard.get_short(*(short*)&info.m_word04))    // line 0x29f
    {
        return 0x29f;
    }
    info.m_type08 = (CONNECT_TYPE)2;                  // ORIG 立即数 2

    // [b28e 真 bug] ORIG 0x849291e/0x8492922/0x8492926 压栈序 = serverId(0x8)、
    // ip(0xc)、port(0x10)，与格式串 "channel no(%d), server ip(%s), port(%d)" 对应
    // ⇒ 实参序应为 (serverId, ip, port)；旧码 (ip, serverId, port) 把 %d 喂指针、
    // %s 喂 int（可变参类型不匹配，日志错乱）。
    // P0：ORIG 0x84928de `mov -0x5c(%ebp),%eax; mov %eax,-0x60(%ebp)`（槽往返）
    // ⇒ 原码有具名 ipv 局部。P227：inet_ntoa 在 cMyTrace ctor 前求值并跨 ctor 保活。
    struct in_addr ipv;
    ipv.s_addr = info.m_field00;
    cMyTrace("int exchange_server::CExchangeServer::"
             "OnRecvPacketServerInfo(exchange_server::CSession*, "
             "PacketGuard&)", 0x2a5, 0)
        ("channel no(%d), server ip(%s), port(%d)", serverId, inet_ntoa(ipv),
         (int)(short)info.m_word04);

    // [b28d-48 P48/P282] ORIG 0x0849293d 与 0x08492ac5 两处独立
    // `mov (%eax),%eax; add $0x10,%eax; … call *%edx`（vptr+0x10 = GetServerType
    // 槽）：O0 无 CSE，双臂各一次虚调用。原具名 self 只发一次（P282 差集
    // ORIG{"16":2} vs OURS{"16":1}）。
    if (this->GetServerType() == EXCHANGE_SERVER_MASTER)
    {
        PacketGuard pkt;
        pkt.clear();
        pkt.put_header(1, 0x83);
        pkt.put_int(GetServerChannelNo());
        pkt.put_int((int)m_serverMap.size());
        for (std::map<int, ExchangeServerInfo>::iterator it =
                 m_serverMap.begin(); it != m_serverMap.end(); ++it)
        {
            pkt.put_int(it->first);
            pkt.put_int(it->second.m_field00);
            pkt.put_short((int)(short)it->second.m_word04);
        }
        pkt.finalize(false);
        m_sessionMgr.TrySend(session, pkt);
    }
    else if (this->GetServerType() == EXCHANGE_SERVER_SLAVE)
    {
        PacketGuard pkt;
        pkt.clear();
        pkt.put_header(1, 0x83);
        pkt.put_int(GetServerChannelNo());
        pkt.put_int(0);
        pkt.finalize(false);
        m_sessionMgr.TrySend(session, pkt);
    }

    if (!RegisterExchangeServer(serverId, info))
    {
        // P227 全表达式临时链：ORIG 0x08492bc1 `mov -0x4c(%ebp),%ebx` 在 ctor 前
        // 把 serverId 载入 callee-saved 并跨 ctor 保活（旧具名形 ctor 后重读槽）。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfo(exchange_server::CSession*, "
                 "PacketGuard&)", 0x2c6, 0)
            ("RegisterExchangeServer! failed (%d)", serverId);
    }
    if (!AttachSessionInExchangeServer(serverId, session))
    {
        // P227 同上：ORIG 0x08492c1e `mov -0x4c(%ebp),%ebx` 跨 ctor 保活。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfo(exchange_server::CSession*, "
                 "PacketGuard&)", 0x2cb, 0)
            ("AttachSessionInExchangeServer! failed (%d)", serverId);
    }
    return 0;
