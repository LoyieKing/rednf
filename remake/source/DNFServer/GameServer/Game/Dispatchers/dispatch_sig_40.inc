// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08200448 orig/ours=161/168 ndiff-blocks=16
//   帧差 sub $1bc vs $1c0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x1bc,%esp | movl   $&_ZZN24Dispatcher_CallGuildInfo12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
int Dispatcher_CallGuildInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08200448（docs/class_func_reports/Dispatcher_CallGuildInfo/dispatch_sig.md）
    // [Lane1Dispatchers 2026-09-10] N78/P214③：ORIG +0xdd 段为内联 rep stos
    //   （rep-stos 计 0/1，`memset` 在 4.4 退化为 call）⇒ 聚合初始化。
    // ORIG 全函数无任何 `movb $0x0,<slot>`（P328⑤：初值 store 不存在）⇒ 去 `= 0`。
    unsigned char count;                      // local_21
    int i;                                    // local_20

    // +0x00：state<3 或 charac 空 → 早退
    // ORIG 段直证：无 bool 物化槽（无 `movb $1/$0,-0x25` 序列）⇒ 删 fail 具名量；
    // 原具名 bool 令我方帧 0x1bc→0x1cc（+0x10）并整段槽位下移。
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
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

    // ORIG 段直证：`lea -0x1a8(%ebp),%ebx; rep stos $0x5f` 位于读 count/上限校验**之后**
    // （函数入口无清零）⇒ 声明随使用点下移。
    unsigned int charIds[0x5f] = {0};         // local_1a8

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
}