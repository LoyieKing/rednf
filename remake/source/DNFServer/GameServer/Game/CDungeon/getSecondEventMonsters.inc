std::vector<stSecondEventMonster_t>* CDungeon::getSecondEventMonsters() const
{
    return const_cast<std::vector<stSecondEventMonster_t>*>(&m_secondEventMonsters);
}