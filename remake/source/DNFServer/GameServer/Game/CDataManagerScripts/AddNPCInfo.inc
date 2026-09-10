void CNPCRelationEventManager::AddNPCInfo(const unsigned short idx,
                                          stNPCRelationEventInfo* info)
{
    m_npcEventMap.insert(std::make_pair(idx, info));
