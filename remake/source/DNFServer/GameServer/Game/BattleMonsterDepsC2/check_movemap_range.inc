int CBattle_Field::check_movemap_range(int x, int y)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    // [b28f-L27] ORIG 0x08307b5a..0x08307b83 为 abs() 内建内联展开
    //   （`mov %edx,%eax; sar $31,%eax; xor %eax,%edx; mov %edx,slot;
    //     sub %eax,slot`）⇒ 原码写 abs() 而非手写位运算（手写两句会先落槽
    //     再重读，+7 条/处）。
    int dx = abs(x - curX);
    int dy = abs(y - curY);
    // [b28f-L27] ORIG 块序 = 主流程(logFormat)+`mov $0x15`+`jmp epi` 在前、
    //   共享 `mov $0,%eax` 尾块在最后 ⇒ 源码是**正向包裹**（P222/P257）：
    //   条件取反包住主流程，最后的裸 return 0 自然落尾并被两条 `jne` 共享。
    if (dx > 1 || dy > 1 || (dx == 1 && dy == 1))
    {
        LogManager::logFormat(
            1, "battle_field.cpp", __PRETTY_FUNCTION__,
            0x119f,
            "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
            "[Map Kind : %s]",
            m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
            x, y,
            GetMapInfoFromPos(x, y, false)->m_layeredMap ? "layeredMap" : "generalMap");
        return 0x15;
    }
    return 0;
