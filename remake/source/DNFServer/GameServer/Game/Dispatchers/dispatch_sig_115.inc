int Dispatcher_PowerWarProcessInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x821ea32 (83 insn)
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xd721,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    const unsigned int characNo = static_cast<unsigned int>(user->get_charac_no(-1));
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()));
    if (proxy != 0)
    {
        proxy->SendPowerWarProcessInfo(characNo);
    }
    PacketGuard response;
    response.put_header(1, 0xec);
    response.put_byte(1);
    response.finalize(true);
    user->Send(response);
    return 0;
