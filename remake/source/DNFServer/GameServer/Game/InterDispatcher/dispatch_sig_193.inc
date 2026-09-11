int Inter_GuildExpLimit::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))->SendGuildExpLimit();
    return 0;
}