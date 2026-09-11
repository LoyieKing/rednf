// ORIG 0x890ce34：遍历 delete 节点后直接析构 map（无显式 clear()）。
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
}