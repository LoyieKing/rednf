int Dispatcher_CallGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820792a：无 user==0 防御（直接 get_state；state<=2 失败）。
    if (user->get_state() < 3 || user->getCurCharacR() == 0) {
        return LineFunc(0xa24f, __PRETTY_FUNCTION__, 0, 0);
    }
    int secuRet = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xb);
    if (secuRet != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9a, (unsigned char)(secuRet & 0xff));
        return 0;
    }
    int guildKey;  // ORIG 0x08207a07：不预清零（直接作 get_int 输出缓冲）
    if (!packet.get_int(guildKey)) {
        return LineFunc(0xa25d, __PRETTY_FUNCTION__, 0, 0);
    }
    char guildName[30];
    if (!packet.get_str(guildName, 0x1e, guildKey)) {
        return LineFunc(0xa261, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->get_charac_guildkey() == 0) {
        this->send_packet_guild_invite(user, (unsigned char)0x22);
        return 0;
    }
    if (!user->IsSubGuildMaster() && !user->IsGuildMaster()
        && user->getGuildMemberGrade() != 3) {
        this->send_packet_guild_invite(user, (unsigned char)0x24);
        return 0;
    }
    unsigned int inviteGuildKey = (unsigned int)user->get_charac_guildkey();
    unsigned int characNo = user->getCurCharacNo();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendCallGuildInvite(characNo, inviteGuildKey, guildName);
    return 0;
