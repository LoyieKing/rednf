bool CDungeonNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    m_success[stat->m_successType].UpdateCharacter(stat);
    return true;
