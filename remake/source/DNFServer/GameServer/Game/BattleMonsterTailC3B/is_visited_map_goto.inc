// ---------------------------------------------------------------------------
// CBattle_Field::is_visited_map_goto — ORIG 0x08307cbe
// 访问标记(+0x01)或 goto 标记(+0x02)任一置位即为真。
// ---------------------------------------------------------------------------
bool CBattle_Field::is_visited_map_goto(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    return info->m_visited != 0 || info->m_visitedGoto != 0;
}