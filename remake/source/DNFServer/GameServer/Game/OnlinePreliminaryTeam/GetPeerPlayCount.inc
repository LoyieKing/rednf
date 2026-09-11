// ORIG T 0x08586184（W 内联）：find；miss→0；hit→value.count。
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
}