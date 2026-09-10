void CBattle_Field::set_visited_map_goto(int x, int y)
{
    int lastX = -1;
    int lastY = -1;
    getLastPosXY(lastX, lastY);
    if (!(lastX == x && lastY == y))
    {
        GetMapInfoFromPos(x, y, false)->m_visitedGoto = 1;
    }
    else
    {
        if (getLayeredMapIndex() == 0)
            GetMapInfoFromPos(x, y, true)->m_visitedGoto = 1;
        else
            getLayeredMapInfoIndex(getLayeredMapIndex() - 1)->m_visitedGoto = 1;
    }
