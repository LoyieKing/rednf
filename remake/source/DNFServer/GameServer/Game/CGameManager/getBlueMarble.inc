BlueMarble* CGameManager::getBlueMarble()
{
    BlueMarble* marble = m_blueMarblePool.Acquire();
    if (marble == 0)
        return 0;
    marble->setMemoryPoolIndex(m_blueMarblePool.GetIndex(marble));
    int marbleIdx = marble->getMemoryPoolIndex();
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(marbleIdx);
    if (it == m_blueMarbleMap.end())
    {
        m_blueMarbleMap[marble->getMemoryPoolIndex()] = marble;
        return marble;
    }
    else
    {
        return 0;
    }
}