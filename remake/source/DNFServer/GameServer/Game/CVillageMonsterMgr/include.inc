bool MapArea::include(int x, int y, int& outX, int& outY, int margin)
{
    if (m_x + margin > x)
        outX = m_x + margin;
    else if (m_x + m_w - margin < x)
        outX = m_x + m_w - margin;
    else
        outX = x;
    if (m_y + margin > y)
        outY = m_y + margin;
    else if (m_y + m_h - margin < y)
        outY = m_y + m_h - margin;
    else
        outY = y;
    if (outX == x && outY == y)
        return 1;
    return 0;
