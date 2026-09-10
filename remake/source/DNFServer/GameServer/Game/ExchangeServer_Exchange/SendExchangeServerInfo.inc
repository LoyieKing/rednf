bool CExchangeServer::SendExchangeServerInfo()
{
    // [b28d-solo-Exchange 真 bug] ORIG 两处提前分支都跳 0x8493848 = `mov $0x1,
    // %eax`（返回 true），旧码写 return false（语义反）。另 P52/P0：pProxy 不设
    // 具名局部（ORIG 0x8493816 GetServerProxy 结果直接作 receiver，实参 port/ip
    // 先物化 %edi/%esi 跨 receiver call 保活）。
    // ORIG 正极性守卫 `m_flag38 || !m_flag39`（0x84937cc jne 主流程 / 0x84937da
    // je 共享 return-true 尾 0x8493848），单出口 ⇒ 两处提前分支都跳函数尾 return true。
    if (m_flag38 || !m_flag39)
    {
        const time_t now = time(NULL);
        // ORIG 0x84937fd `jle`（有符号）⇒ time_t 同型相减，勿加 unsigned cast。
        if (now - m_resetTime3c > 9)
        {
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendExchangeServerInfo(GetServerChannelNo(), m_ip04,
                                         (short)m_port08);
            m_resetTime3c = (unsigned int)now;
        }
    }
    return true;
