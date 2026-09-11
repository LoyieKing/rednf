// ORIG 0x08300ed4：find_map(mapIndex)；失败按当前图种类记日志返回 0；
// 困难 4 且地图 m_b4 非零时改写当前图号后二次查找（再失败同样记日志 0x4c4）。
// [W13E 2026-09-11] ORIG 0x08300ed4 返回 const CMap*（prett 串直证）⇒ 定义侧
// 签名同步；函数体内部仍用非 const 局部再隐式转 const 返回（机器码不变）。
const CMap* CBattle_Field::getMap(int mapIndex)
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
}