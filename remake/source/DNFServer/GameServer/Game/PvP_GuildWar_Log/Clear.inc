void PvP_GuildWar_Log::Clear()
{
    m_guildCnt = 0;
    m_startTime = 0;
    memset(m_guilds, 0, sizeof(m_guilds));
}