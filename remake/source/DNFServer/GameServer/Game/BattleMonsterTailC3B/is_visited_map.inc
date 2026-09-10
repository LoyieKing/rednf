bool CBattle_Field::is_visited_map(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    if (info->m_visited != 0)
        return true;
    return false;
