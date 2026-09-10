bool CBattle_Field::check_end_point()
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    if (m_endX == x && m_endY == y)
    {
        return true;
    }
    return false;
