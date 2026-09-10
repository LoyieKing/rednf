int CBattle_Field::MoveMap(int x, int y, bool& firstVisit)
{
    MapInfo* mapInfo = GetCurrentMapInfo();

    // b28d-solo-BattleDepsB2（真语义 bug 修正）：ORIG +0x027/+0x039 两处 `xor $1; test;
    //   je <主流程 +0x063>` ⇒ A(IsEnterEachMap) 或 B(check_grid_clear) 为真
    //   **走主流程**，二者皆假才落 +0x040 拒绝块（moveMap_1_Log; mov
    //   $0x14,%ebx; jmp exit）。原重建 `if (A||B) { 拒绝 }` 分支整体反置。
    //   正解 = 嵌套否定守卫把拒绝块放最内层、主流程 fall-through（4.4 两个
    //   xor/je 共享主流程出口，与 ORIG 逐条同形）。
    if (!m_pDungeon->IsEnterEachMap())
    {
        if (!check_grid_clear())
        {
            moveMap_1_Log(x, y);
            return 0x14;
        }
    }
        if (m_pDungeon != NULL && (char)m_pDungeon->m_bloodDungeonType > 0)
        {   // 血战副本换图时重置血轮
            reset_blood_values();
            m_bloodRound.reset_blood_values();
            m_bloodRound.setBloodRoundOrder();
        }

        if (x < 0 || x > 0x31 || y < 0 || y > 0x31)
            return 1;

        // b28d-solo-BattleDepsB2：ORIG 此处**重新**调用 GetCurrentMapInfo（Ghidra C 三处调用
        //   0x08308bxx/+0xe8 段，p167 差集 MISSING×1 实证）——非复用开头 mapInfo。
        if (GetCurrentMapInfo()->m_layeredMap != 1)
        {   // 非层叠图：原地不动判定
            int curX = -1;
            int curY = -1;
            getCurPosXY(curX, curY);
            if (x == curX && y == curY)
                return 0x12;
        }

        if (m_gridMaze == NULL)
        {
            moveMap_2_Log(x, y);
            return 0x15;
        }

        if (!(x < m_gridMaze->m_width && y < m_gridMaze->m_height))
            return 0x15;

        // b28d-solo-BattleDepsB2：ORIG +0x17d `movl $0x0,-0x18`（err 预清，P198①）+ 4 处
        //   `cmpl $0,-0x18; setne %al; test; je`（P225 while+break 物化形，
        //   4.4 对 if 直跳不产 setne）。
        int err = 0;
        err = check_boss_map(x, y);
        while (err != 0)
        {
            return err;
        }

        err = check_movemap_range(x, y);
        while (err != 0)
        {
            return err;
        }

        const GridScript* grid = &m_gridMaze->getGridR(x, y);
        if (grid == NULL)
        {
            moveMap_3_Log(x, y);
            return 0x15;
        }

        if (check_visited_map(x, y, firstVisit))
            return 0;

        int dir = get_invade_direction(x, y);
        CDungeon* dungeon = G_CDataManager()->find_dungeon(m_dungeonIndex);
        if (dungeon == NULL)
        {
            moveMap_4_Log(x, y, grid);
            return 1;
        }

        int newMapId = -1;
        bool dummyFlag = true;
        choose_random_bossmap_from_dummymaps(dungeon, grid, newMapId, dummyFlag, x, y, dir);

        std::vector<int> candidator;
        m_visitedGotoFlag = 0;                       // +0x1c8 byte 清零

        err = get_candidator_map(dungeon, grid, newMapId, candidator, x, y, dir);
        while (err != 0)
        {
            return err;
        }

        set_visited_map(x, y, dummyFlag);
        swap_curpos_xy(x, y);
        set_visited_map_goto(x, y);
        GetCurrentMapInfo()->m_gridValue = (char)grid->m_gridValue;   // +0x08 ← 网格值低字节

        err = set_map_id(grid, newMapId, candidator);
        while (err != 0)
        {
            return err;
        }

        ConsistMap();
        if (m_party != NULL)
            m_party->m_killMobCounter.clear();   // +0x1844
        return 0;
