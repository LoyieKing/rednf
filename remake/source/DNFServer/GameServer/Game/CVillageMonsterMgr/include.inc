// ============================================================================
// MapArea::include(x,y,outX,outY,margin) —— ORIG T 0x82f02e6
//   把 (x,y) 按 margin 裁剪到本矩形内（结果写 outX/outY）；
//   未发生移动（裁剪后仍等于原坐标）返回 1，否则返回 0。
// ============================================================================
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
}