void CGuildServerProxy::ChangeGuildAgitFlagInProxy(unsigned int guildId,
                                                   unsigned char agitFlag)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp",
                              __PRETTY_FUNCTION__,
                              0x86e, "Find fail from guild info proxy(guild_id:%d)", guildId);
        return;
    }
    STGuildProxy* pGuild = &it->second;
    // ORIG @846ebb4 写入 +0x18（本结构 m_powerSide 槽；DB 装载器把列 3
    // power_side 也读入 +0x18 —— ORIG 两处同址，原样复刻）
    pGuild->m_powerSide = agitFlag;
