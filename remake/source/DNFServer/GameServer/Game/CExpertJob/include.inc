bool Rectangle::include(Rectangle const& other) const
{
    // 块A：other 左上点落在本矩形内
    if (other.m_pos.m_x >= m_pos.m_x &&
        other.m_pos.m_x <= m_pos.m_x + m_width &&
        other.m_pos.m_y >= m_pos.m_y &&
        other.m_pos.m_y <= m_pos.m_y + m_height)
        return true;

    // 块B：other 右边界 x 落在本矩形内且上边 y 在 y 区间
    if (other.m_pos.m_x + other.m_width >= m_pos.m_x &&
        other.m_pos.m_x + other.m_width <= m_pos.m_x + m_width &&
        other.m_pos.m_y >= m_pos.m_y &&
        other.m_pos.m_y <= m_pos.m_y + m_height)
        return true;

    // 块C：other 左边 x 在 x 区间且下边 y 落在本矩形内
    if (other.m_pos.m_x >= m_pos.m_x &&
        other.m_pos.m_x <= m_pos.m_x + m_width &&
        other.m_pos.m_y + other.m_height >= m_pos.m_y &&
        other.m_pos.m_y + other.m_height <= m_pos.m_y + m_height)
        return true;

    // 块D：other 右边界 x 与下边 y 均落在本矩形内
    if (other.m_pos.m_x + other.m_width >= m_pos.m_x &&
        other.m_pos.m_x + other.m_width <= m_pos.m_x + m_width &&
        other.m_pos.m_y + other.m_height >= m_pos.m_y &&
        other.m_pos.m_y + other.m_height <= m_pos.m_y + m_height)
        return true;

    return false;
