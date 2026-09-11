int BlueMarbleInfoScript::getDungeonDifficulty(int dungeon)
{
    unsigned int i = 0;
    while (i < m_dungeonDifficultyList.size())
    {
        if (m_dungeonDifficultyList[i].m_dungeon == dungeon)
        {
            return m_dungeonDifficultyList[i].m_difficulty;
        }
        ++i;
    }
    return m_dungeonDifficulty;
}