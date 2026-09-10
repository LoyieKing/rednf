int CExchangeServer::OnRecvPacketServerInfoRet(CSession* session,
                                               PacketGuard& guard)
{
    // [P330 本泳道 2026-09-09] REAL-BUG 修复（ORIG 0x08492c7f..0x08492c88）：
    // ORIG 在**函数入口**（三个 0 初始化之后、首个 get_int 之前）只调一次
    // `ExchangeServerInfo::ExchangeServerInfo()`（槽 -0x8c），循环体内不再构造
    // （ORIG 全函数该 ctor 出现 1 次，-0x8c 其余 5 处均为取址/读字段）。
    // 我方旧码 `for (...) { ExchangeServerInfo info; ... }` 每轮重新构造 ⇒
    // 未被本轮 get_int/get_short 覆盖的字段被清零；ORIG 则**沿用上一轮残值**
    // 传给 RegisterExchangeServer。p167 抓不到（循环体机器码只有一份，ctor
    // 调用计数两边都是 1）——只有 ctor 调用**位置**（入口 vs 循环内）能发现。
    // 三个 int 的声明序按 ORIG 初始化序 -0x74 / -0x78 / -0x7c 排定。
    // 残余仍为 N82（巨件帧/槽平移）+ N93（cMyTrace 块槽方向）。
    int entryCount = 0;                               // -0x74
    int remoteId = 0;                                 // -0x78
    int senderId = 0;                                 // -0x7c
    ExchangeServerInfo info;                          // -0x8c（ORIG：入口构造一次）

    if (!guard.get_int(senderId))                     // line 0x2d9
    {
        return 0x2d9;
    }
    if (!guard.get_int(entryCount))                   // line 0x2da
    {
        return 0x2da;
    }

    {
        // [b28e 真 bug] ORIG 0x08492d0a/d0x08492d13 压栈序 = senderId(0x8)、
        // entryCount(0xc) ⇒ 实参序 (senderId, entryCount)；旧码两值互换
        // （"channel no(%d), server count(%d)" 打印反）。
        // P227：ORIG 在 ctor 前把两值载入 %esi/%ebx 跨 ctor 保活。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfoRet(exchange_server::"
                 "CSession*, PacketGuard&)", 0x2dc, 0)
            ("channel no(%d), server count(%d)", senderId, entryCount);
    }

    for (int i = 0; i < entryCount; ++i)
    {
        if (!guard.get_int(remoteId))                 // line 0x2e0
        {
            return 0x2e0;
        }
        if (!guard.get_int(info.m_field00))           // line 0x2e1
        {
            return 0x2e1;
        }
        // P0：同 OnRecvPacketServerInfo——ORIG 直传 &info.m_word04，无 port16 槽。
        if (!guard.get_short(*(short*)&info.m_word04))  // line 0x2e2
        {
            return 0x2e2;
        }
        info.m_type08 = (CONNECT_TYPE)1;              // ORIG 立即数 1

        // [b28e 真 bug] ORIG 0x08492e60/0x8492e68/0x8492e6e 压栈序 =
        // remoteId(0x8)、ip(0xc)、port(0x10) ⇒ 实参序 (remoteId, ip, port)；
        // 旧码 (ip, remoteId, port) 把 %d 喂指针、%s 喂 int。
        // P0：ORIG 0x08492e4c `mov -0x8c(%ebp),%eax; mov %eax,-0x90(%ebp)` ⇒ 具名 ipv。
        // P227：ip/port/remoteId 在 ctor 前求值（%ebx/%edi/%esi 保活）。
        struct in_addr ipv;
        ipv.s_addr = info.m_field00;
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfoRet(exchange_server::"
                 "CSession*, PacketGuard&)", 0x2e8, 0)
            ("channel no(%d), server ip(%s), port(%d)", remoteId,
             inet_ntoa(ipv), (int)(short)info.m_word04);

        // [b28d-solo-Exchange] ORIG 0x8492e2b（自身表项）/0x8492e82（find==end，
        // eqERKS5_）/0x8492ebb（TryConnect 成功）三处 `je 8492f15` 与 TryConnect
        // 失败路径全部汇入循环内**无条件** RegisterExchangeServer（0x8492f15）；
        // 旧码写成三处 `continue` 跳过注册 ⇒ p167 eq/ne 差集 + 控制流真差异。
        if (m_localInfo.m_field00 == info.m_field00 &&
            m_localInfo.m_word04 == info.m_word04)
        {
            // 自身表项：不重连，直接注册
        }
        else if (m_serverMap.find(remoteId) == m_serverMap.end())
        {
            // 未登记：不重连，直接注册
        }
        else if (!m_sessionMgr.TryConnect(
                     inet_ntoa(ipv),
                     (int)(short)info.m_word04))
        {
            // P227：ORIG 0x08492ebd/0x8492ec7 在 ctor 前求值 port(%esi)/ip(%ebx)。
            cMyTrace("int exchange_server::CExchangeServer::"
                     "OnRecvPacketServerInfoRet(exchange_server::"
                     "CSession*, PacketGuard&)", 0x2f2, 0)
                ("cannot connect other game server! (%s,%d)",
                 inet_ntoa(ipv), (int)(short)info.m_word04);
        }
        if (!RegisterExchangeServer(remoteId, info))
        {
            // P227：ORIG 0x08492f38 `mov -0x78(%ebp),%ebx` 跨 ctor 保活。
            cMyTrace("int exchange_server::CExchangeServer::"
                     "OnRecvPacketServerInfoRet(exchange_server::"
                     "CSession*, PacketGuard&)", 0x301, 0)
                ("RegisterExchangeServer! failed (%d)", remoteId);
        }
    }

    if (!AttachSessionInExchangeServer(senderId, session))
    {
        // P227：ORIG 0x08492faa `mov -0x7c(%ebp),%ebx` 跨 ctor 保活。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfoRet(exchange_server::"
                 "CSession*, PacketGuard&)", 0x307, 0)
            ("AttachSessionInExchangeServer! failed (%d)", senderId);
    }
    SetSlave();
    return 0;
