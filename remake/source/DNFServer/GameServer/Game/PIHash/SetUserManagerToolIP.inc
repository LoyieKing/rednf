void SetUserManagerToolIP(CUser* user, SIG_UPDATE_LOGOUT* sig)
{
    if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        sig->m_pcbangFlag = 1;
    else
        sig->m_pcbangFlag = 0;

    user->m_network.GetPeerIP2(sig->m_ipBuf, 0x10);

    // peer IP 为空时回退用户 m_mcap.m_pcRoomIp（+0x8e418）记录的 IP。
    if (sig->m_ipBuf[0] == '\0' && user->m_mcap.m_pcRoomIp[0] != '\0')
    {
        std::memset(sig->m_ipBuf, 0, 0x10);
        std::strncpy(sig->m_ipBuf, user->m_mcap.m_pcRoomIp, 0x10);
    }

    // [SweepPIHash] ORIG 0x08655327 尾部为 4 个独立 nop+jmp 岛（落点 9d/a0/a3/a6，
    // 均 jmp 出口 a7）⇒ 按 P223 多岛形写 4 条独立 goto + 尾标签串（标签序=守卫序）。
    // 变量先声明（无初始化）以合法跨越 goto（P223 BM8）。
    char* dot;
    int prefixLen;
    int suffixLen;
    if (std::strlen(sig->m_ipBuf) > 0x10)
        goto tool_ip_end1;
    // 以最后一个 '.' 把 a.b.c.d 拆为前缀(+0x37)/后缀(+0x43)。
    dot = std::strrchr(sig->m_ipBuf, '.');
    if (dot == 0)
        goto tool_ip_end2;
    prefixLen = static_cast<int>(dot - sig->m_ipBuf);
    if (static_cast<unsigned int>(prefixLen) > 0xc)
        goto tool_ip_end3;
    // [b23S3C] P48：后缀起点用 ipBuf+prefixLen+1 表达式（ORIG 两使用点各自
    // `mov 0xc;lea 0x27;mov -0x10;add$1;lea(,%eax,1)` 重取 5 条；dot+1 缓存形
    // 少 3 条/处，非 ORIG 表达式）。
    suffixLen = static_cast<int>(std::strlen(sig->m_ipBuf + prefixLen + 1));
    if (static_cast<unsigned int>(suffixLen) > 4)
        goto tool_ip_end4;
    std::strncpy(sig->m_ipPrefix, sig->m_ipBuf, prefixLen);
    std::strncpy(sig->m_ipSuffix, sig->m_ipBuf + prefixLen + 1, suffixLen);
    sig->m_ipDword = user->m_mcap.m_field24;  // +0x8e414
tool_ip_end1:;
tool_ip_end2:;
tool_ip_end3:;
tool_ip_end4:;
