// ============================================================================
// CDungeonNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08485564
// m_success[stat->m_successType].UpdateCharacter(stat)；返回 1。
// ============================================================================
bool CDungeonNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    m_success[stat->m_successType].UpdateCharacter(stat);
    return true;
}