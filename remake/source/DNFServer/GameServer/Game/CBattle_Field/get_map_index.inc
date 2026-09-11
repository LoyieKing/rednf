int CBattle_Field::get_map_index(int x, int y)
{
    return GetMapInfoFromPos(x, y, false)->m_mapIndex;
}