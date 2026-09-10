bool isInArea(int x, int y, MapArea* area)
{
    if (x < area->m_x)
        return 0;
    if (x >= area->m_x + area->m_w)
        return 0;
    if (y < area->m_y)
        return 0;
    if (y >= area->m_y + area->m_h)
        return 0;
    return 1;
