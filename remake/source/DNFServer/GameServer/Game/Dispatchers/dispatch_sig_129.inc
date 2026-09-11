// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08207ffe orig/ours=181/183 ndiff-blocks=25
//   帧差 sub $90 vs $a0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x90,%esp | movl   $&_ZZN31Dispatcher_NotifyMessageToGuild12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,-0xc
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
    // [G1Csig 负结论] 曾试「去具名 bool invalid + 两判就地报错」：实测 ndiff 42→48（倒退）
    // ⇒ 回滚原形（if/else-if + 具名 bool；ORIG 侧确有该物化形）。
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
}