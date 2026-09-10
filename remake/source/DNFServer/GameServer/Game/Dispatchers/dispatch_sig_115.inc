int Dispatcher_PowerWarProcessInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x821ea32 (83 insn)
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xd721,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()));
    if (proxy != 0)
    {
        // ORIG 0x0821ea32 段：[31] 用 %ebx 保存跨调用值、参数经 `mov %ebx,0x4(%esp)` 直传
        //   —— 无 characNo 具名栈槽（我方具名使帧 0x20→0x30）。
        proxy->SendPowerWarProcessInfo(
            static_cast<unsigned int>(user->get_charac_no(-1)));
    }
    PacketGuard response;
    response.put_header(1, 0xec);
    response.put_byte(1);
    response.finalize(true);
    user->Send(response);
    return 0;
