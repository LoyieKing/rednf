// ============================================================================
// IsDislikeItem（ORIG T 0x0890d100）：m_npcEventMap.find(idx) 未命中 → 0；
// node->m_dislikeItems（+0x3c vector<int>）任一元素 == itemId → 1。
// ============================================================================
bool CNPCRelationEventManager::IsDislikeItem(unsigned short idx,
                                             unsigned int itemId)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return false;

    stNPCRelationEventInfo* node = it->second;
    // ORIG 0x0890d100：索引循环 .size()/.at()（非迭代器），每元素一次 at()
    // 返回指针后 (*puVar3==itemId) 用 sete 直判命中即返 1。
    for (unsigned int col = 0; col < node->m_dislikeItems.size(); ++col)
    {
        // [b28c-L15] ORIG 0x0890d15x：`mov (%eax),%eax; cmp 0x10(%ebp),%eax; sete %al`
        //   —— at() 结果留在 %eax、形参槽作内存源操作数（无 (int) cast 中转）。
        if (node->m_dislikeItems.at(col) == itemId)
            return true;
    }
    return false;
}