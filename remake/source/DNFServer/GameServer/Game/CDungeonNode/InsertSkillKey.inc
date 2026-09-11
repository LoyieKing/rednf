// ============================================================================
// CDungeonNode::InsertSkillKey(int, int, int, int) —— ORIG T 0x08485404
// 参数 1 = skillIndex（转 CDungeonJobNode::InsertSkillKey）、参数 2 = job（m_jobMap 键）；
// 参数 3/4 未用。恒返回 1。
// ============================================================================
bool CDungeonNode::InsertSkillKey(int skillIndex, int job, int type, int flag)
{
    // [b28d-solo-CDungeonNode · 具名 it + 临时 end] ORIG 0x08485404：find sret -0x54（具名 it）
    // 先、end 临时 sret -0x50 后（ORIG 无具名 e 槽）⇒ 源码只具名 it，比较用 `m_jobMap.end()`。
    std::map<int, CDungeonJobNode>::iterator it = m_jobMap.find(job);
    if (it != m_jobMap.end())
    {
        CDungeonJobNode* node = &it->second;
        node->InsertSkillKey(skillIndex);
    }
    else
    {
        CDungeonJobNode node;
        node.InsertSkillKey(skillIndex);
        m_jobMap.insert(std::make_pair(job, node));
    }
    return true;
}