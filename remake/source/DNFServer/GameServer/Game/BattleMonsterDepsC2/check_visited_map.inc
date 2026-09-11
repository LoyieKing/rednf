// ORIG 0x08307d7c：按副本是否每图进入选用 goto 位或普通访问位判定。
bool CBattle_Field::check_visited_map(int x, int y, bool& firstVisit)
{
    if ((m_pDungeon != NULL) && m_pDungeon->IsEnterEachMap())
    {
        if (is_visited_map_goto(x, y))
        {
            on_not_visited_map(x, y, firstVisit);
            return true;
        }
    }
    else
    {
        if (is_visited_map(x, y))
        {
            on_not_visited_map(x, y, firstVisit);
            return true;
        }
    }
    return false;
}