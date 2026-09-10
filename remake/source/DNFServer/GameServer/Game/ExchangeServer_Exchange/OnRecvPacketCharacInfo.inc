int CExchangeServer::OnRecvPacketCharacInfo(CSession* session,
                                            PacketGuard& guard)
{
    // [b28d-48] N82 封顶（NDIFF 155，194/199 条）：巨件帧/槽平移 +
    // PacketGuard 临时槽方向。
    int errBook = 0;                                  // -0x44 (类型 a)
    int idx = 0;                                      // -0x48
    int chNoArg = 0;                                  // -0x4c
    int characNo = 0;                                 // -0x50
    bool localHit = false;                            // -0x1d

    if (!guard.get_int(errBook))                      // fail line 0x235
    {
        return 0x235;
    }
    if (!guard.get_int(idx))
    {
        return 0x236;
    }
    if (!guard.get_int(chNoArg))
    {
        return 0x237;
    }
    if (!guard.get_int(characNo))
    {
        return 0x238;
    }

    // ORIG：大对象置于栈帧 -0x3f8b5（sizeof(CACHE_CHARACTER_TYPE)=0x3f85c）
    CACHE_CHARACTER_TYPE cc;
    int pos = 0;                                      // -0x1c
    int mi1 = 0;                                      // -0x54
    int mi2 = 0;                                      // -0x58
    bool found = false;                               // -0x59
    pos = cc.GetMemberPosition((ENUM_INTERNALPACKET)errBook, &mi1, found);

    // P225：ORIG 0x84923c9 `cmpl $0,-0x1c; setne %al; test %al,%al; je` ⇒
    // while(v!=0){body;break;} 精确复现 4.1 bool 四段物化（4.4 对 if 恒直跳）。
    while (pos != 0)
    {
        if (!guard.get_int(mi2))                      // line 0x241
        {
            return 0x241;
        }
        if (!guard.get_binary((char*)pos, mi2))       // line 0x24d
        {
            return 0x24d;
        }
        break;
    }

    // P0：ORIG 0x084923bb `call GetChannelNo; mov %eax,%ebx` 无槽往返 ⇒ 原码
    // 无具名 fromChannel 局部（旧码多发一槽并把帧撑大 0x20）。
    // P227：ORIG 0x084923bd..0x084923e4 在 ctor 前右起求值 7 个实参
    // （characNo/chNoArg/mi1/mi2 落深槽，idx/errBook 落 %edi/%esi，
    // fromChannel 落 %ebx）⇒ 全表达式临时链复现。
    cMyTrace("int exchange_server::CExchangeServer::"
             "OnRecvPacketCharacInfo(exchange_server::CSession*, "
             "PacketGuard&)", 0x253, 0)
        ("OnRecvPacketCharacInfo() type(%d), idx(%d), msg len(%d), "
         "member_size(%d), channel_no(%d), charac_no(%d), "
         "from_channel_no",
         errBook, idx, mi2, mi1, chNoArg, characNo,
         session->GetChannelNo());

    if (errBook == 0x20)
    {
        localHit = true;
    }

    if (!ReturnCacheCharacter(errBook, idx, chNoArg, characNo, localHit,
                              session))
    {
        if (!SendResponseCharacInfo(errBook, idx, (void*)pos, mi1,
                                    session, 0x15))
        {
            cMyTrace trace("int exchange_server::CExchangeServer::"
                           "OnRecvPacketCharacInfo(exchange_server::"
                           "CSession*, PacketGuard&)", 0x25d, 0);
            trace("SendResponseCharacInfo() error");
        }
    }
    return 0;                                         // 全部正常路径 ret0
