CMap* CBattle_Field::getMap(int mapIndex)
{
    CMap* map = static_cast<CMap*>(G_CDataManager()->find_map(mapIndex));
    if (map == NULL)
    {
        LogManager::logFormat(
            1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x4a7,
            "Find Map[%d] Error(In Map Maker)(%d/%d) [Map Kind : %s]",
            GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY,
            GetCurrentMapInfo()->m_layeredMap ? "layeredMap" : "generalMap");
        return NULL;
    }
    if (get_dungeon_diff() == 4 && map->m_b4 != 0)
    {
        GetCurrentMapInfo()->m_mapIndex = map->m_b4;
        map = static_cast<CMap*>(G_CDataManager()->find_map(map->m_b4));
        if (map == NULL)
        {
            LogManager::logFormat(
                1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x4c4,
                "Find Map[%d] Error(In Map Maker)(%d/%d) [Map Kind : %s]",
                GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY,
                GetCurrentMapInfo()->m_layeredMap ? "layeredMap" : "generalMap");
            return NULL;
        }
    }
    return map;
