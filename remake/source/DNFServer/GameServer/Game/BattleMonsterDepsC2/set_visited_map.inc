void CBattle_Field::set_visited_map(int x, int y, bool visited)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    if (curX != x || curY != y)
    {
        GetCurrentMapInfo()->m_visited = visited;
    }
    else
    {
        if (getLayeredMapIndex() == 0)
            GetMapInfoFromPos(curX, curY, true)->m_visited = visited;
        else
            getLayeredMapInfoIndex(getLayeredMapIndex() - 1)->m_visited = visited;
    }
