// ORIG 0x083086d2：写目标格/当前层 visited 标记。
// [b28d-solo-BattleMonsterDepsC2] P272/P136：ORIG 短路链 `jne<else>`+`je<then>`（else 直落、
// then 在后）⇒ 源写德摩根 `if (curX != x || curY != y) {else} else {then}`；字段直赋 bool 值
// 复现 `movzbl -0x1c(%ebp),%edx; mov %dl,0x1(%eax)`（原三目 `?1:0` 每处 +5 条物化）。
void CBattle_Field::set_visited_map(int x, int y, bool visited)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    if (curX != x || curY != y)
    {
        GetCurrentMapInfo()->m_visited = visited;
    }
    else
    {
        if (getLayeredMapIndex() == 0)
            GetMapInfoFromPos(curX, curY, true)->m_visited = visited;
        else
            getLayeredMapInfoIndex(getLayeredMapIndex() - 1)->m_visited = visited;
    }
}