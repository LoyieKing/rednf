void CBattle_Field::moveMap_4_Log(int x, int y, const GridScript* gs)
{
    if (gs == NULL)
        return;
    // P44/P52：layeredCount/info 无槽——R2L 实参即时求值，值升 callee-saved。
    LogManager::logFormat(
        1, "battle_field.cpp",
        __PRETTY_FUNCTION__, 0x115b,
        "TRY MoveMap [4] Error(G_CDataManager()->find_dungeon( m_Dungeon )) "
        "Dungeon[%d] Pos(%d,%d), gs->mapIndex(%d) [Map Kind : %s], "
        "gs->layeredMapIndex(%d)\n",
        m_dungeonIndex, m_curX, m_curY,
        static_cast<unsigned int>(gs->m_mapCandidates.size()),
        GetMapInfoFromPos(m_curX, m_curY, false)->m_layeredMap ? "layeredMap" : "generalMap",
        static_cast<unsigned int>(gs->m_layeredMapIndexes.size()));       // gs+0x1c
}