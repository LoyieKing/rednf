bool TCPSocket::connect_nonb(const char* host, unsigned short port,
                             timeval timeout)
{
    // [b23S3C] P48：ORIG connect 错误链对 errno 每判重取 __errno_location()
    // （4 次独立 call，&& 短路）；成功路径单一共享尾（result==0 跳过 select
    // 块直达 memcpy）——旧形 result==0 早返回造成 EXTRA memcpy+1。
    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_addr.s_addr = inet_addr(host);
    adrs.sin_port = htons(port);

    socklen_t len = sizeof(adrs);

    setOptNonBlock();

    int result;
    int local_24 = 0;   // [b23S3C] P31：ORIG +0x6a `movl $0,-0x24` dead 槽
                        // （Ghidra local_24 无消费），缺它使 adrs/len/result
                        // 全体高 4（-0x38 vs -0x3c）。
    result = ::connect(m_socket,
                       reinterpret_cast<struct sockaddr*>(&adrs), len);
    // [b28d-solo-nexon_cash] N 封顶：ORIG 短路首项 `mov; shr $0x1f; test %al,%al;
    // je`（4.1 符号位物化）；4.4 对 result<0 / (unsigned)result>>31 / (result>>31)
    // 均折 `cmpl $0; jns`（N76 族，本次再证）。另 ORIG select 后 if 链的
    // mov$1/jmp/test 汇聚物化（N3 族）与 getsockopt 实参右到左求值序（P75）同为
    // 跨版本结构性差异；已修 FD_ZERO 显式循环/FD_SET unsigned/soError 无预清与
    // 槽序，语义等价。
    if (result < 0 && errno != 0x73 /*EINPROGRESS*/ &&
        errno != EINTR && errno != EAGAIN && errno != EAGAIN)
        return false;
    if (result != 0)
    {
        fd_set writeSet;
        // [b28d-solo-nexon_cash] P212/P30：ORIG 0x081afb74 为显式位循环
        // （i=0..0x1f, pset->fds_bits[i]=0 的 setbe/jne 回边），与 pollReadEvent
        // 已 AE 的形一致；FD_ZERO 宏在 4.4 折 rep stos（N78 反形）。
        unsigned int i;
        fd_set* pset;
        pset = &writeSet;
        for (i = 0; i <= 0x1f; ++i)
            pset->fds_bits[i] = 0;
        // [b28d-solo-nexon_cash] ORIG 0x081afbb0 `shr $0x5` 无符号移位 ⇒ fd
        // 实参须 unsigned（否则 4.4 出 sar+cmovs，N97）。
        FD_SET(static_cast<unsigned int>(m_socket), &writeSet);

        result = ::select(m_socket + 1, 0, &writeSet, 0, &timeout);
        if (result < 0 && errno != EINTR)
            return false;
        if (result <= 0)
            return false;

        // [b28d-solo-nexon_cash] P171：ORIG 0x081afc6a 无 soError 预清
        // （仅 movl $0x4,-0x40 写 optLen），槽序 soError@-0x44/optLen@-0x40。
        // [b28d-solo-nexon_cash] P212：块内 4.4 先声明占浅槽 ⇒ optLen 先声明
        // 得 -0x40、soError 后声明得 -0x44（=ORIG 槽序）。
        socklen_t optLen = 4;
        int soError;
        if (::getsockopt(m_socket, 1 /*SOL_SOCKET*/, 4 /*SO_ERROR*/,
                         &soError, &optLen) < 0)
            return false;
        if (soError != 0)
            return false;
    }

    memcpy(&m_peerAdrs, &adrs.sin_addr, 4);
    m_peerPort = adrs.sin_port;
    return true;
