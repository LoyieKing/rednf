bool CBattle_Field::is_visited_map_goto(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    return info->m_visited != 0 || info->m_visitedGoto != 0;
