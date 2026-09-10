bool CGuildServerProxy::InsertGuildInfoProxy(unsigned int guildId, STGuildProxy& info)
{
    return m_guildInfoMap.insert(std::make_pair(guildId, info)).second;
