int Dispatcher_NotifyMessageToGuild::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08207ffe（PacketDispatcher_Impl_1.cpp 0xa2f4 区）
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xa2f4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int guildKey = user->get_charac_guildkey();
    if (guildKey == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9d, 0x22);
        return 0;
    }
    if (!user->IsSubGuildMaster() && !user->IsGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9d, 0x24);
        return 0;
    }
    int len = 0;
    if (!packet.get_int(len))
    {
        return LineFunc(0xa309,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char message[101];
    if (!packet.get_str(message, 0x65, len))
    {
        return LineFunc(0xa30c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    bool invalid = false;
    if (G_CDataManager()->hasPreventString(message))
    {
        invalid = true;
    }
    else if (G_CDataManager()->hasSpecialCharacterExceptSpace(message))
    {
        invalid = true;
    }
    if (invalid)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9d, 0x6c);
        return 0;
    }
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendMessageToGuild((unsigned int)guildKey, len, message);
    PacketGuard guard;
    guard.put_header(1, 0x9d);
    guard.put_byte(1);
    guard.finalize(true);
    user->Send(guard);
    return 0;
