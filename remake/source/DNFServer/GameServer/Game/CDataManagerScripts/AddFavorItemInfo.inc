// ---- CNPCRelationEventManager::AddFavorItemInfo（ORIG 0x890cf44：
//      find(idx)，非 end 时将 {itemId,count} push_back 到 node+kind*0x14
//      即 m_favor[kind].m_items；步长 0x14 为 stNPCFavorAndRewardInfo）----
void CNPCRelationEventManager::AddFavorItemInfo(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind,
    unsigned int itemId, unsigned int count)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return;

    // ORIG 0x890cf7x：node 物化局部槽后重读（operator-> 一次），item 栈构 push。
    stNPCRelationEventInfo* node = it->second;
    stNPCEventItemInfo item;
    item.m_itemId = static_cast<int>(itemId);
    item.m_count = static_cast<int>(count);
    node->m_favor[kind].m_items.push_back(item);
}