void CGuildServerProxy::ChangeGuildNameInProxy(unsigned int guildId, char* name)
{
    STGuildProxy* volatile pGuild;
    // 槽序：find 结果 -0x14（与 ORIG 一致）；end() 临时与 pGuild 互换（N98 残差）。
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp", __PRETTY_FUNCTION__, 0x860,
                              "Find fail from guild info proxy(guild_id:%d)", guildId);
    }
    else
    {
        pGuild = &it->second;
        memset(pGuild, 0, 0x17);
        memcpy(pGuild, name, 0x16);
    }
