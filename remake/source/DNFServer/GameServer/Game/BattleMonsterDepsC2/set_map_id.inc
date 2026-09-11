// ORIG 0x08308882：确定最终图号并写回当前图信息。
// [b28d-solo-BattleMonsterDepsC2] P197④/P0：ORIG `call size; test %eax; setne %al; test %al;
// je` ⇒ `candidator.size() != 0`（empty() 生 xor $1 两段）；get_rand_int 结果具名存 -0x1c
// （ORIG 有槽往返）；logFormat 内联 GetMapInfoFromPos（具名 info 使调用提前，破坏右起求值序）。
int CBattle_Field::set_map_id(const GridScript* gs, int& mapId,
                              INT_VECTOR& candidator)
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
}