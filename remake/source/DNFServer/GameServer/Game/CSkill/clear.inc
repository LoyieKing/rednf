// ---- STSecondGrowTypeFitness（ORIG 0x083748da / 0x083748ee）----
void STSecondGrowTypeFitness::clear()
{
    memset(m_fitness, 0, sizeof(m_fitness));
    memset(m_maxLevel, 0, sizeof(m_maxLevel));
}