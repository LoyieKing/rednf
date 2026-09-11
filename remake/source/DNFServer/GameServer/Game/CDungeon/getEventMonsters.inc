std::vector<stEventMonster_t>* CDungeon::getEventMonsters() const
{
    return const_cast<std::vector<stEventMonster_t>*>(&m_eventMonsters);
}