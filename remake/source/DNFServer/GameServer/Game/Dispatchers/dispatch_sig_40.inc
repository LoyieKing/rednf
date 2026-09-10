int Dispatcher_CallGuildInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08200448（docs/class_func_reports/Dispatcher_CallGuildInfo/dispatch_sig.md）
    // [Lane1Dispatchers 2026-09-10] N78/P214③：ORIG +0xdd 段为内联 rep stos
    //   （rep-stos 计 0/1，`memset` 在 4.4 退化为 call）⇒ 聚合初始化。
    unsigned int charIds[0x5f] = {0};         // local_1ac
    unsigned char count = 0;                  // local_21
    int i;                                    // local_20
    bool fail;                                // bVar1

    // +0x00：state<3 或 charac 空 → fail
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        fail = true;
    else
        fail = false;
    if (fail)
        return LineFunc(0x94e7,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x66：读数量
    if (!packet.get_byte(count))
        return LineFunc(0x94ec,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0xaa：count > 0x5f → 拒绝
    if (count >= 0x60)
        return LineFunc(0x94f0,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0xfa：逐个读取；遇到 0 → 发送 0x4d 错误包
    for (i = 0; i < (int)(unsigned int)count; i = i + 1)
    {
        if (!packet.get_int((unsigned int&)charIds[i]))
            return LineFunc(0x94f7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (charIds[i] == 0)
        {
            PacketGuard guard;                                        // 0858dd4c
            guard.put_header(1, 0x4d);
            guard.put_byte(0);
            guard.put_byte(0x22);
            guard.finalize(true);
            user->Send(guard);
            return 0;                                                 // guard 析构 0858de80
        }
    }

    // +0x21a：委托公会服务器
    {
        unsigned int uid = user->GetUID();                            // 080c8c96
        unsigned int characNo = user->getCurCharacNo();               // 080cbc4e
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendCallGuildInfo(characNo, uid, count, charIds);
    }
    return 0;
