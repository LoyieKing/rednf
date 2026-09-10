void COnlinePreliminaryMemberMatchList::IncreasePeerPlayCount(
    int serverId, unsigned int uniqueId)
{
    CharacterInServer key(serverId, uniqueId);
    std::map<CharacterInServer, PlayCountInfo>::iterator it =
        m_peerMap.find(key);
    if (it != m_peerMap.end())
    {
        it->second.m_flag = 1;
        it->second.m_count += 1;
    }
    else
    {
        if (m_peerMap.size() > 999)
        {
            // ORIG __PRETTY_FUNCTION__ 行号 0x65（101）
            cMyTrace(__PRETTY_FUNCTION__, 0x65, 0)(
                "Online Preliminary peer play count map size over! %d",
                static_cast<unsigned int>(m_peerMap.size()));
            return;
        }
        m_peerMap.insert(std::make_pair(CharacterInServer(serverId, uniqueId),
                                        PlayCountInfo(1, true)));
    }
    alter();
