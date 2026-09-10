void CBattle_Field::choose_random_bossmap_from_dummymaps(const CDungeon* dungeon,
                                                         const GridScript* gs,
                                                         int& mapId, bool& flag,
                                                         int x, int y, int dir)
{
    // [b28d-Lane27] ORIG 0x08307ed1/0x08307edb/0x08307ee5：三条独立 `je` 各跳**不同**
    // nop 岛，且第三岛（0x0830820d）**直落** epilogue（无 jmp）⇒ 用 P223 多岛形
    // 「独立否定 goto + 尾标签串」（标签序=守卫序）复现；if/return 形 4.4 会为第三
    // 个 return 也发 jmp（+2 条）。
    if (dungeon == NULL)
        goto L_dungeonNull;
    if (gs == NULL)
        goto L_gsNull;
    if (m_gridMaze == NULL)
        goto L_gridMazeNull;

    if (dungeon->IsHaveDummyMap())
    {
        // [round5-C05 2026-09-10] D08 配方（多守卫同目标合并为单条短路 goto）：
        // ORIG 0x08307f0f/0x08307f1e 两条 `jne <epilogue>`，我方平铺 return 令
        // 4.4 在尾部各发一对 `jmp epilogue;nop` 中继岛（尾部 5 岛 vs ORIG 3 岛）
        // ⇒ 合并为单条 `||` goto 到尾标签串（与上方三守卫同目标）。
        if (m_endX != x || m_endY != y)                                   // +0xc/+0x10
            goto L_dungeonNull;
        if (static_cast<unsigned int>(m_dummyMapPosIdx) <
            m_dummyMapPos.size())                                         // +0xcc < +0xc0.size
        {
            std::pair<int, int> next = m_dummyMapPos[m_dummyMapPosIdx];
            m_endX = next.first;
            m_endY = next.second;

            if (GetMapInfoFromPos(m_endX, m_endY, false)->m_visited)
            {
                GetMapInfoFromPos(m_endX, m_endY, false)->m_visited = 0;
                GetMapInfoFromPos(m_endX, m_endY, false)->m_mapType = 2;
            }

            int curX = -1;
            int curY = -1;
            getCurPosXY(curX, curY);
            if (m_endX == curX && m_endY == curY)
            {
                flag = false;
                GetMapInfoFromPos(m_endX, m_endY, false)->m_mapType = 2;
            }

            mapId = dungeon->ChooseRandomMap(2, gs->m_gridValue, dir);
            GetMapInfoFromPos(x, y, false)->m_dummyBossMap = 1;
            m_dummyMapPosIdx = m_dummyMapPosIdx + 1;
        }
        else
        {
            if (GetMapInfoFromPos(x, y, false)->m_dummyBossMap)
                GetMapInfoFromPos(x, y, false)->m_dummyBossMap = 0;
        }
    }
    else if (dungeon->m_entryRestrictFlag != 0)
    {
        // [b28d-Lane27] ORIG 0x083081f7 `cmp -0x10(%ebp),%eax; setl %al`（**有符号**）
        // ⇒ count 与 i 都是 int（size_t 会生 setb）。
        const int count = static_cast<int>(m_gridMaze->m_vec20.size());   // maze+0x20
        for (int i = 0; i < count; ++i)
        {
            // ORIG 两字段各重取一次 operator[]（P157 逐字段重读，p167 缺 1 处）
            if (m_gridMaze->m_vec20[i].first == x &&
                m_gridMaze->m_vec20[i].second == y)
            {
                m_endX = x;                                               // +0xc
                m_endY = y;                                               // +0x10
                // [b28d-Lane27] P171：ORIG 0x083081eb `mov %eax,0x10(%ebp)` 把
                // getGridR 返回的引用地址**死写入 gs 形参槽**（gs 此后不再使用）。
                // 旧版写成 `mapId = <地址>` 是**真语义 bug**（越权改输出参数）。
                gs = &m_gridMaze->getGridR(x, y);
                return;
            }
        }
    }

L_dungeonNull:;
L_gsNull:;
L_gridMazeNull:;
