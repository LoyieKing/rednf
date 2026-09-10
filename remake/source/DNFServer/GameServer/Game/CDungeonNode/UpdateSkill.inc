bool CDungeonNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<int, CDungeonJobNode>::iterator it = m_jobMap.find(stat->m_job);
    if (it == m_jobMap.end())
        return false;
    // ORIG 0x084855d4：operator-> 后 `add $0x4,%eax; mov %eax,-0xc(%ebp)` ——
    // 源码取具名节点指针再调用（直接 it->second.UpdateSkill 会折成 lea+直调）。
    CDungeonJobNode* node = &it->second;
    node->UpdateSkill(stat);
    return true;
