// ---------------------------------------------------------------------------
// PreparePassiveSocket                     ORIG 0x8493856
// 监听本服端口、把自身注册进 serverMap（即 ORIG 的三级“size over”告警链，
// 其比较立即数为编译期常量——忠实保留判断形态）。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug 日志第二实参 sizeProbe→0x3f869/0x3f86b（P97）+
// P221 五臂临时链 + m_ip04 int 化（P211 store 中转）。
bool CExchangeServer::PreparePassiveSocket()
{
    // [b28d-solo-Exchange] P221 临时链翻五臂 trace 槽方向（ORIG 0x40a=-0x60、
    // 0x416=-0x50、0x420=-0x40、0x426=-0x30、0x42c=-0x20 深→浅；4.4 具名形相反）。
    // P97：ORIG 0x8493a0f/0x8493a5e 的 logFormat 第二实参是立即数 0x3f869/0x3f86b
    // （非 sizeProbe），按来源保留。
    m_ip04 = (int)inet_addr(GetServerIP());
    m_port08 = (unsigned short)GetServerPORT();

    if (!m_sessionMgr.TryListen((int)(short)m_port08, GetServerIP()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x40a, 0)
            ("TryListen failed!");
        return false;
    }

    {
        ExchangeServerInfo info;
        info.m_field00 = (int)inet_addr(GetServerIP());
        info.m_word04 = m_port08;
        info.m_type08 = EXCHANGE_CONNECT_UNKNOWN;
        if (!RegisterExchangeServer(GetServerChannelNo(), info))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x416, 0)
                ("RegisterExchangeServer! failed (%d)",
                 GetServerChannelNo());
        }
    }

    // ORIG 局部序列：-0x10=0x10（未参与比较）、-0xc=0x15469 后接三级常量门限
    // 注意：不能声明 const（GCC O0 会常量折叠删除比较链，ORIG 保留运行时判断）。
    int warnBase = 0x10;                              // -0x10（写入未读取）
    (void)warnBase;
    int sizeProbe = 0x15469;                          // -0xc
    if (sizeProbe > 0x17318)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x420, 0)
            ("RegisterExchangeServer! failed packet size over! (%d/%d)",
             sizeProbe, 0x17318);
    }
    if (sizeProbe > 0x17318)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x426, 0)
            ("RegisterExchangeServer! failed recv buffer size over! "
             "(%d/%d)", 0x3f869, 0x17318);
    }
    if (sizeProbe > 0x6ddd0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x42c, 0)
            ("RegisterExchangeServer! failed send buffer size over! "
             "(%d/%d)", 0x3f86b, 0x6ddd0);
    }
    return true;
}