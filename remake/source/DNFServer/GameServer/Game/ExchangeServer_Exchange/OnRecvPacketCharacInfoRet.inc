int CExchangeServer::OnRecvPacketCharacInfoRet(CSession* session,
                                               PacketGuard& guard)
{
    // [P31/P330 本泳道 2026-09-09] ORIG 入口零初始化**序列**为
    // `-0x38, -0x3c, -0x24, -0x20, -0x40`（5 条 movl，见 0x084925f4..0x08492610），
    // 旧码只声明 3 个（⇒ 我方 3 条 movl，帧 0x3f8bc vs ORIG 0x3f8cc）。
    // 其中 -0x24 / -0x20 两条写**在整个函数里再无读取**（已核：ORIG 切片内
    // grep `-0x24(%ebp)`/`-0x20(%ebp)` 各只命中这条 movl）⇒ 原码确有两个
    // 之后未被使用的局部（P31 死写先例，同 SetPeriodCheckTimer 的 pad24）。
    // 声明顺序照 ORIG 初始化顺序排，两个死局部按 ORIG 位置夹在中间。
    int idx = 0;                                      // ORIG -0x38
    int fromCh = 0;                                   // ORIG -0x3c
    int pad24 = 0;                                    // ORIG -0x24（死写，P31）
    int pad20 = 0;                                    // ORIG -0x20（死写，P31）
    int ret = 0;                                      // ORIG -0x40

    if (!guard.get_int(ret))                          // line 0x26c
    {
        return 0x26c;
    }
    if (!guard.get_int(idx))                          // line 0x26d
    {
        return 0x26d;
    }
    if (!guard.get_int(fromCh))                       // line 0x26e
    {
        return 0x26e;
    }

    CACHE_CHARACTER_TYPE cc;
    int pos = 0;
    int mi1 = 0;
    int mi2 = 0;
    bool found = false;
    pos = cc.GetMemberPosition((ENUM_INTERNALPACKET)idx, &mi1, found);

    // P225：ORIG 0x0849271a `cmpl $0x0,-0x1c(%ebp); setne %al; test %al,%al;
    // je` ⇒ while(v!=0){body;break;} 精确复现 4.1 bool 四段物化（4.4 对 if 直跳）。
    while (pos != 0)
    {
        if (!guard.get_int(mi2))                      // line 0x27a
        {
            return 0x27a;
        }
        if (!guard.get_binary((char*)pos, mi2))       // line 0x282
        {
            return 0x282;
        }
        break;
    }

    // P0：ORIG 0x08492778 `call GetChannelNo; mov %eax,%ebx` 无槽往返 ⇒ 原码
    // 无具名 channel 局部（旧码 `const int channel` 多发一槽）。
    // P227：ORIG 在 ctor 前求值 mi1/mi2/fromCh 到深槽、idx/ret 到 %edi/%esi、
    // channel 到 %ebx（0x0849277d..0x084927a5）⇒ 全表达式临时链复现。
    cMyTrace("int exchange_server::CExchangeServer::"
             "OnRecvPacketCharacInfoRet(exchange_server::"
             "CSession*, PacketGuard&)", 0x28a, 0)
        ("OnRecvPacketCharacInfoRet() ret(%d) type(%d), idx(%d), "
         "msg len(%d), member_size(%d), from_channel(%d)",
         ret, idx, fromCh, mi2, mi1, session->GetChannelNo());

    if (ret == 0)
    {
        // P276：ORIG 0x84927e3 `je 84927ec` 与 ret!=0 汇入**同一** ForwardRequestCharac
        // 调用点（p167 旧形 EXTRA +1）⇒ 成功即 return 0，失败落共享尾。
        if (SendResponseCharacInfo(idx, fromCh, (void*)pos, mi1))
        {
            return 0;
        }
    }
    ForwardRequestCharac(idx, fromCh, (void*)pos, mi1);
    return 0;
