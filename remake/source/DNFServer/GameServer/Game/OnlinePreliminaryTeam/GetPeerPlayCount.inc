unsigned int COnlinePreliminaryMemberMatchList::GetPeerPlayCount(
    int serverId, unsigned int uniqueId) const
{
    CharacterInServer key(serverId, uniqueId);
    std::map<CharacterInServer, PlayCountInfo>::const_iterator it =
        m_peerMap.find(key);
    // [b28c-16] P222 正向包裹（同上）。
    if (it != m_peerMap.end())
        return it->second.m_count;
    return 0;
