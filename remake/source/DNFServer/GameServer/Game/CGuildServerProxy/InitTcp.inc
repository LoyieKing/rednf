bool CGuildServerProxy::InitTcp()
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (m_tcp.m_sock < 0)
    {
        int ret = m_tcp.CreateConnectionSocket((int&)m_tcp, m_ip.c_str(),
                                               m_tcpPort);
        // [b28d-solo-CGuildServerProxy P225] ORIG @0x0846d9c1 `cmpl $0x0,-0x1c(%ebp);
        // setne %al; test %al,%al; je` —— 4.1 的槽比零物化形，4.4 对 if 恒折
        // `cmpl;je` 直跳；P225 实证 while+break 形可复现（CVillageAttacked/CDropItem）。
        while (ret != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4ea, 0)
                ("CGuildServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x4ef, 9, true, true)
        ("[!] Connect To Guild Server ...");
    return 1;
