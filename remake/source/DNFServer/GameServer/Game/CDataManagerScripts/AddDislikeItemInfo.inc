void CNPCRelationEventManager::AddDislikeItemInfo(unsigned short idx,
                                                  unsigned int itemId)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    // ORIG 0x890d0c5 eq + 0x890d0cc jne 跳过 ⇒ 源码 == end 早退形；
    // 0x890d0d9-0x890d0f5：node 物化槽 -0xc 后重读、(int)itemId 物化 -0x10，
    // push_back(EOi) 传 &temp（4.4 将 node 折进 %eax 不复现 ⇒ 余差属 N）。
    if (it == m_npcEventMap.end())
        return;
    stNPCRelationEventInfo* node = it->second;
    node->m_dislikeItems.push_back(itemId);
