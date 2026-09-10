void CBattle_Field::moveMap_3_Log(int x, int y)
{
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x114a,
        "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y,
        GetMapInfoFromPos(x, y, false)->m_layeredMap ? "layeredMap" : "generalMap");
