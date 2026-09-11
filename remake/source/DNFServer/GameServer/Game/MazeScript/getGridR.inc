const GridScript& MazeScript::getGridR(int x, int y) const
{
    return m_grids[m_width * y + x];
}