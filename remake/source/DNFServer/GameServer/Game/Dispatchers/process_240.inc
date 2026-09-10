int Dispatcher_Delete_GuildBoard_Text::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    unsigned int guildkey = (unsigned int)user->get_charac_guildkey();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    unsigned int textIdx = *(unsigned int*)((char*)&param + 0x89);
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendGuildBoardDelete(textIdx, characNo, guildkey);
    return 0;
