// ---------------------------------------------------------------------------
// CBattle_Field::is_visited_map — ORIG 0x08307c7a
// GetMapInfoFromPos(x,y,false) 后取 MapInfo+0x01 访问标记。
// ---------------------------------------------------------------------------
bool CBattle_Field::is_visited_map(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    if (info->m_visited != 0)
        return true;
    return false;
}