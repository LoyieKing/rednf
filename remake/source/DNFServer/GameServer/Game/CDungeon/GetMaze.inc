int CDungeon::GetMaze(int idx) const
{
    if ((unsigned int)idx >= m_mazeList.size())
        return 0;
    return (int)&m_mazeList[idx];
