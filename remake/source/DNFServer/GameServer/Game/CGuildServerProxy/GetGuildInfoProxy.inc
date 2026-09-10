bool CGuildServerProxy::GetGuildInfoProxy(unsigned int guildId, STGuildProxy& out)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              __PRETTY_FUNCTION__,
                              0x87d, "Find fail from guild info proxy(guild_id:%d)", guildId);
        return false;
    }
    out = it->second;
    return true;
