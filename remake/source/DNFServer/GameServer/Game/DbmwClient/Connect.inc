unsigned int DbmwClient::Connect()
{
    // ORIG @0x081209fa：Init 失败 -98；已连接 -96；
    // connect() -1 且 errno!=EINPROGRESS(0x73) → -97（否则继续 Regist）；
    // Regist 失败 → -95；成功置 connected、返回 0。
    if (Init() != 0)
        return (unsigned int)-98;

    if (m_connected)
        return (unsigned int)-96;

    if (::connect(m_sock, reinterpret_cast<sockaddr*>(&m_addr),
                  sizeof(sockaddr_in)) == -1)
    {
        if (errno == EINPROGRESS)              // ORIG 0x8120a63：cmp $0x73（EINPROGRESS=115）jne -97 块
        {
            if (!Regist())
                return (unsigned int)-95;      // 0xffffffa1
        }
        else
            return (unsigned int)-97;          // 0xffffff9f
    }
    else
    {
        if (!Regist())
            return (unsigned int)-95;          // 0xffffffa1
    }

    m_connected = true;
    return 0;
}