// ORIG T 0x0834c9f0。
// b28d-solo-W7_UserOnlyC 判词：第一循环补具名 MazeScript* 局部（ORIG -0x14 槽，
// P0 槽往返）⇒ 113=113，TU 层 AE。
bool CDungeon::VerifyMaze() const
{
    if (!(m_index <= 0x7cf))
        return true;

    for (unsigned int i = 0; i < m_mazeList.size(); ++i)
    {
        // P0：ORIG -0x14 槽保存 operator[] 返回的指针（具名局部，非内联实参）。
        const MazeScript* maze = &m_mazeList[i];
        if (!VerifyMaze(*maze))
            return false;
    }
    for (int g = 0; g < 2; ++g)
    {
        for (unsigned int i = 0; i < m_mazeList0[g].size(); ++i)
        {
            MazeScript copy(m_mazeList0[g][i]);
            if (!VerifyMaze(copy))
                return false;
        }
    }
    return true;
}