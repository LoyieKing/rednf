stNPCRewardItemInfo CNPCRelationEventManager::GetRewardItem(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind)
{
    stNPCRewardItemInfo result;
    result.m_rewardA = 0;
    result.m_rewardB = 0;
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return result;
    stNPCRelationEventInfo* node = it->second;
    // g6 探针：`return T{a,b}`（列表初始化临时）令 4.4 走 NRVO 直写 sret，
    // 复现 ORIG 命中分支的 `mov %eax,(%ebx); mov %edx,0x4(%ebx)`。
    return stNPCRewardItemInfo{node->m_favor[kind].m_rewardA,
                               node->m_favor[kind].m_rewardB};
