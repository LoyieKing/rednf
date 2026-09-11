int CArtifact::GetCreatureMinimumLevel()
{
    if (m_pItem == 0)
        return 0;
    if (m_pItem->is_stackable())
        return 0;
    return m_pItem->get_creature_minimum_level();
}