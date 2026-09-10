int CBattle_Field::get_candidator_map(const CDungeon* dungeon, const GridScript* gs,
                                      int& mapId, std::vector<int>& candidator,
                                      int x, int y, int dir)
{
    if (dungeon == NULL || gs == NULL)
        return 0x17;
    // [b28d-Lane27] ORIG 0x08308238 段 `cmp $0xffffffff,%eax; jne 0x083086c4`（直跳尾部
    // `mov $0,%eax`）⇒ 写「`if (mapId == -1) { 主体 }` + 尾部共享 `return 0`」；负向早退形
    // 4.4 会把 `return 0` 就地内联（`je 主体; mov $0; jmp`，+3 条）。主体缩进从简。
    if (mapId == -1)
    {
    if (m_endX == x && m_endY == y)                                       // +0xc/+0x10
    {
        if (gs->m_secondCandidates.size() == 0)                           // gs+0x10
        {
            mapId = dungeon->ChooseRandomMap(1, gs->m_gridValue, dir);
        }
        else
        {
            // [b28d-Lane27] ORIG 槽位：map=-0x2c / i=-0x28（map **先声明**更深）⇒ 把
            // map 提到 for 之前声明（P212/P331 同尺寸标量声明序）。
            // [L27-r2·判据⑩②/widthscan] ORIG 该槽无零初始化（我方 `movl $0x0,-0x2c` 为多出）
            //   ⇒ 先声明、循环内赋值（P198 孪生）。
            CMap* map;
            for (unsigned int i = 0; i < gs->m_secondCandidates.size(); ++i)
            {
                map = static_cast<CMap*>(
                    G_CDataManager()->find_map(gs->m_secondCandidates[i]));
                if (map == NULL)
                    return 0x17;
                if (map->CheckEntrance(dir))
                    candidator.push_back(gs->m_secondCandidates[i]);
            }
        }
    }
    else if (m_field14 == x && m_field18 == y &&                          // +0x14/+0x18
             m_questStageMap != 0)                                        // +0x19c
    {
        mapId = m_questStageMap;
        if (mapId < 1)
        {
            LogManager::logFormat(
                1, "battle_field.cpp",
                "int CBattle_Field::get_candidator_map(const CDungeon*, "
                "const GridScript*, int&, INT_VECTOR&, int, int, int)", 0x12bb,
                "[CBattle_Field::MoveMap] Error(quest_stage_map_==-1) Dungeon[%d] "
                "Pos(%d,%d), candidator(%d), gs->mapIndex(%d), [Map Kind : %s], "
                "gs->layeredMapIndex(%d)\n",
                m_dungeonIndex, m_curX, m_curY,
                static_cast<unsigned int>(candidator.size()),
                static_cast<unsigned int>(gs->m_mapCandidates.size()),
                GetMapInfoFromPos(m_curX, m_curY, false)->m_layeredMap
                    ? "layeredMap" : "generalMap",
                static_cast<unsigned int>(gs->m_layeredMapIndexes.size()));
        }
    }
    else if (m_hellPosX == x && m_hellPosY == y && m_hellPartyActive)     // +0x1c0/+0x1c4/+0x1b8
    {
        mapId = m_hellMapIdx;                                             // +0x1bc
        m_visitedGotoFlag = 1;                                            // +0x1c8
    }
    else
    {
        if (gs->m_mapCandidates.size() == 0)                              // gs+0x04
        {
            mapId = dungeon->ChooseRandomMap(0, gs->m_gridValue, dir);
        }
        else
        {
            // [b28d-Lane27] ORIG 槽位：map=-0x24 / i=-0x20（map 先声明更深）。
            // [L27-r2·判据⑩②/widthscan] 同上：去 `= NULL` 消 `movl $0x0,-0x24`。
            CMap* map;
            for (unsigned int i = 0; i < gs->m_mapCandidates.size(); ++i)
            {
                map = static_cast<CMap*>(
                    G_CDataManager()->find_map(gs->m_mapCandidates[i]));
                if (map == NULL)
                    return 0x17;
                if (map->CheckEntrance(dir))
                    candidator.push_back(gs->m_mapCandidates[i]);
            }
            if (!GetCurrentMapInfo()->m_layeredMap &&
                candidator.size() == 0)
            {
                for (size_t i = 0; i < gs->m_mapCandidates.size(); ++i)
                {
                    // [round5-C05 2026-09-10] P171 死写入照抄：ORIG 0x08308631
                    // `mov %eax,-0x24(%ebp)` 把 find_map 结果写回具名 `map` 槽
                    // （其后无读点）；我方 `(void)` 丢弃 ⇒ 少 1 条（327 vs 325）。
                    map = static_cast<CMap*>(
                        G_CDataManager()->find_map(gs->m_mapCandidates[i]));
                    LogManager::logFormat(
                        1, "battle_field.cpp",
                        "int CBattle_Field::get_candidator_map(const CDungeon*, "
                        "const GridScript*, int&, INT_VECTOR&, int, int, int)",
                        0x12e4,
                        "[CBattle_Field::MoveMap] Error(< map_candidator.size()==0) "
                        "Dungeon[%d] Pos(%d,%d), (Direction: %d) \n",
                        m_dungeonIndex, m_curX, m_curY, dir);
                    LogManager::logFormat(
                        1, "battle_field.cpp",
                        "int CBattle_Field::get_candidator_map(const CDungeon*, "
                        "const GridScript*, int&, INT_VECTOR&, int, int, int)",
                        0x12e5,
                        "[CBattle_Field::MoveMap] Error(< map_candidator.size()==0) "
                        "gs->mapIndexes_[%d]=%d \n",
                        static_cast<int>(i), gs->m_mapCandidates[i]);
                }
            }
        }
    }
    }
    return 0;
