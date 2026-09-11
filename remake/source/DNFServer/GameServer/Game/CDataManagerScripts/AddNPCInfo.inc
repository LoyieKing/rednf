// ---- CNPCRelationEventManager::AddNPCInfo（ORIG 0x890ceea：
//      make_pair(idx, info) 后 map insert）----
void CNPCRelationEventManager::AddNPCInfo(const unsigned short idx,
                                          stNPCRelationEventInfo* info)
{
    m_npcEventMap.insert(std::make_pair(idx, info));
}