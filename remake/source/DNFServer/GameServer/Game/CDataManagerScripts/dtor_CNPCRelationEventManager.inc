CNPCRelationEventManager::~CNPCRelationEventManager()
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.begin();
    while (it != m_npcEventMap.end())
    {
        stNPCRelationEventInfo* node = it->second;
        if (node != 0)
            delete node;
        ++it;
    }
