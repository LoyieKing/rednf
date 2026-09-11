// ---- CNPCRelationEventManager::AddDislikeItemInfo（ORIG 0x890d07a：
//      find(idx)，非 end 时向 node+0x3c 的 vector<int> push_back）----
// [b28d-solo-CDataManagerScripts] AE：删具名 `int item`（P0 右值直传 push_back(itemId) 的 const& 临时由编译器生成）⇒ it/end 槽 -0x14/-0x10 与 ORIG 对齐；ORIG 0x0890d07a。
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
}