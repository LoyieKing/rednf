void CBattle_Field::moveMap_1_Log(int x, int y)
{
    // ORIG 无局部变量：三元表达式直接链式调用（cdecl 从右到左求值，
    // 首个调用用于 m_layeredMap、第二个用于 m_mapIndex）。
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1126,
        "TRY MoveMap [1][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y,
        GetMapInfoFromPos(x, y, false)->m_layeredMap ? "layeredMap" : "generalMap");
}