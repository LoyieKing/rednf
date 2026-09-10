std::map<int, std::vector<RandomList> >* CDungeon::getRandomTowerMapIndexes()
    const
{
    return const_cast<std::map<int, std::vector<RandomList> >*>(&m_map658);
