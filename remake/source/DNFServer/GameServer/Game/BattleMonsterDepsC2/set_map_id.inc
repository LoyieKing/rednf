int CBattle_Field::set_map_id(const GridScript* gs, int& mapId,
                              std::vector<int>& candidator)
{
    if (gs == NULL)
        return 0x15;

    if (mapId < 0)
    {
        if (candidator.size() != 0)
        {
            const int idx =
                get_rand_int(static_cast<int>(candidator.size()));
            mapId = candidator[idx];
        }
        if (GetCurrentMapInfo()->m_layeredMap)
            mapId = gs->m_layeredMapIndexes[getLayeredMapIndex()];
    }

    GetCurrentMapInfo()->m_mapIndex = mapId;
    if (mapId < 1)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__,
            0x1363,
            "[CBattle_Field::MoveMap] Error Dungeon[%d] Pos(%d,%d), candidator(%d), "
            "gs->mapIndex(%d) [Map Kind : %s], gs->layeredMapIndex(%d)\n",
            m_dungeonIndex, m_curX, m_curY,
            static_cast<unsigned int>(candidator.size()),
            static_cast<unsigned int>(gs->m_mapCandidates.size()),
            GetMapInfoFromPos(m_curX, m_curY, false)->m_layeredMap
                ? "layeredMap" : "generalMap",
            static_cast<unsigned int>(gs->m_layeredMapIndexes.size()));
        return 0x15;
    }
    return 0;
