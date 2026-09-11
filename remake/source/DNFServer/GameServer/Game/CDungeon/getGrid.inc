GridScript* MazeScript::getGrid(int x, int y)
{
    // ORIG 0x88f2c2a：返回 &m_grids[m_width*y+x]（vector::operator[] 结果）
    return &m_grids[m_width * y + x];
}