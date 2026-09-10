void CBattle_Field::SetGridPath(MazeScript* maze, int dummyAppearCnt)
{
    m_gridMaze = maze;                           // +0x74
    m_mapWidth = m_gridMaze->m_width;                  // +0x18c ← maze+0x00
    m_mapHeight = m_gridMaze->m_height;                // +0x190 ← maze+0x04
    if (m_mapWidth + m_mapHeight < 4)
    {
        m_hellPartyActive = 0;                   // +0x1b8
        m_field1cb = 1;                          // +0x1cb
    }

    // b28d-solo-BattleDepsB2：ORIG +0x96..+0xbbb 三个 `pair<int,int>::C1Ev` 默认构造
    //   （startPos/bossPos/dummyPos），随后 +0x1cc/+0x1f1 `size()`+sete 判空
    //   （非 empty()，p167 MISSING×2 / EXTRA×2 实证），再经 `aSEOS0_`
    //   右值赋值回填。
    std::pair<int, int> startPos;
    std::pair<int, int> bossPos;
    // 注：ORIG 只有 3 个 pair C1Ev（-0xcc/-0xd4/-0xdc = startPos/bossPos/
    //   adjBossPos），无 dummyPos 具名局部——dummy 循环经临时槽 -0xe4 直传
    //   push_back（0x82ffd59/0x82ffd7b）。

    if (m_gridMaze->m_vec14.size() == 0)               // 起点槽
        return;
    startPos = m_gridMaze->m_vec14[::rand() % m_gridMaze->m_vec14.size()];

    if (m_gridMaze->m_vec20.size() == 0)               // boss 槽
        return;
    int bossIdx = (int)(::rand() % m_gridMaze->m_vec20.size());
    bossPos = m_gridMaze->m_vec20[bossIdx];

    if (dummyAppearCnt > 0)
    {
        int prevIdx = bossIdx;
        m_dummyMapPosIdx = 0;                    // +0xcc
        m_dummyMapPos.clear();                   // +0xc0
        for (int i = 0; i < dummyAppearCnt; ++i)
        {
            if (m_gridMaze->m_vec20.size() > 1)
            {   // 与上次不同下标（同值跳过再 +1）
                int idx = (int)(::rand() % (m_gridMaze->m_vec20.size() - 1));
                if (prevIdx <= idx)
                    idx += 1;
                m_dummyMapPos.push_back(m_gridMaze->m_vec20[idx]);
                prevIdx = idx;
            }
        }
    }

    std::pair<int, int> adjBossPos;              // 相邻 boss 槽（优先下一格）
    if (bossIdx - 1 >= 0)
        adjBossPos = m_gridMaze->m_vec20[bossIdx - 1];
    if (bossIdx + 1 <= (int)m_gridMaze->m_vec20.size() - 1)
        adjBossPos = m_gridMaze->m_vec20[bossIdx + 1];

    // 位置链（ORIG +0x2f0..+0x338）：注意 ORIG 存向 —— +4=起点.second、+8=起点.first。
    m_startX = startPos.second;
    m_startY = startPos.first;
    m_curX = m_startX;
    m_curY = m_startY;
    m_lastX = m_curX;
    m_lastY = m_curY;

    if (m_pDungeon != NULL && m_pDungeon->IsEnterEachMap())
    {
        for (int i = 0; i < 4; ++i)
        {
            m_curPos[i] = std::make_pair(startPos.second, startPos.first);
            m_lastPos[i] = std::make_pair(startPos.second, startPos.first);
        }
    }

    m_endX = bossPos.first;                      // +0x0c 终点槽
    m_endY = bossPos.second;                     // +0x10
    m_field14 = adjBossPos.first;                // +0x14 相邻槽
    m_field18 = adjBossPos.second;               // +0x18
    m_hellPosX = m_gridMaze->m_30;                     // +0x1c0
    m_hellPosY = m_gridMaze->m_34;                     // +0x1c4
    m_hellMapIdx = m_gridMaze->m_2c;                   // +0x1bc
    m_ridable.SetRidableObject(m_gridMaze->m_ridable);// +0x38 RidableScript

    if (m_pDungeon->m_bossRoom90c.m_0 == 1)      // dungeon+0x90c
    {
        // —— boss 房条件候选格：非空网格且其候选地图均有条件怪 ——
        std::vector<std::pair<int, int> > candidates;
        for (int row = 0; row < m_gridMaze->m_height; ++row)
        {
            for (int col = 0; col < m_gridMaze->m_width; ++col)
            {
                if ((m_endX == col && m_endY == row) ||
                    (m_startX == col && m_startY == row))
                    continue;

                const GridScript* grid = &m_gridMaze->getGridR(col, row);
                if (grid->m_mapCandidates.empty())
                    continue;

                // b28d-solo-BattleDepsB2：ORIG 无 ok bool 槽——循环后 0x8300182 显式
                //   `__gnu_cxx::operator==(cit, end)` 判定（break 早退则 !=end），
                //   故 cit 声明在循环外、循环体内 break（p167 MISSING×1 实证）。
                std::vector<int>::const_iterator cit = grid->m_mapCandidates.begin();
                std::vector<int>::const_iterator citEnd = grid->m_mapCandidates.end();
                for (; cit != citEnd; ++cit)
                {
                    CMap* cm = static_cast<CMap*>(G_CDataManager()->find_map(*cit));
                    if (cm == NULL)
                        break;
                    std::list<_mapMonster> condList = cm->get_condition_monster_list();
                    if (condList.empty())
                        break;
                }
                if (cit == citEnd)
                    candidates.push_back(std::make_pair(col, row));
            }
        }

        // —— 条件三元组（monsterIdx, spawnCnt, maxKillCnt）→ boss 房状态 ——
        const std::vector<int>& triples = m_pDungeon->m_bossRoom90c.m_vec4;  // +0x910
        std::vector<int>::const_iterator it = triples.begin();
        while (it != triples.end())
        {
            int monsterIdx = *it;
            ++it;
            int spawnCnt = *it;
            ++it;
            int maxKillCnt = *it;
            ++it;

            stConditionEnterBossRoomStatus_t status;
            status.m_monsterIdx = monsterIdx;
            status.m_curKillCount = 0;
            status.m_maxKillCount = maxKillCnt;
            for (int k = 0; k < spawnCnt; ++k)
            {
                int r = get_rand_int((int)candidates.size());
                status.m_pos.push_back(candidates.at(r));
            }
            m_bossRoomConditions.push_back(status);   // +0x170
        }
    }

    // —— 事件怪生成（需 tryGenEventMonsterCnt!=0 且重复事件 0x24 判活）——
    if (m_pDungeon->getTryGenEventMonsterCnt() != 0)
    {
        bool eventOn = false;
        CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x24);
        if (repeatEvent->IsEventing((CUser*)0))       // vtable+0x34 槽 13，ORIG 不判空
            eventOn = true;

        if (eventOn)
        {
            std::vector<std::pair<int, int> > positions;
            for (int row = 0; row < m_gridMaze->m_height; ++row)
            {
                for (int col = 0; col < m_gridMaze->m_width; ++col)
                {
                    const GridScript* grid = &m_gridMaze->getGridR(col, row);
                    if (grid->m_gridValue != 0)
                        positions.push_back(std::make_pair(col, row));
                }
            }

            std::list<MONSTER_POS> usedPositions;
            for (int attempt = 0; attempt < m_pDungeon->getTryGenEventMonsterCnt();
                 ++attempt)
            {
                if (!(get_rand_int(10000) <=
                      (unsigned int)m_pDungeon->getTryGenEventMonsterProb()))
                    continue;

                // b28d-solo-BattleDepsB2：ORIG 0x83005a1/0x83005c7 对 getEventMonsters() 各
                //   调一次（begin/end 各自内联 receiver，P157 无缓存）；
                //   迭代器声明序 begin→end→acc→roll（ORIG -0x14c/-0x150/-0x30/-0x2c）。
                //   ORIG 的 begin/end 是 **const 版**（_ZNK...5beginEv），故 receiver
                //   经编译期无操作的 const 指针转换（getter 返回非 const 指针）。
                std::vector<stEventMonster_t>::const_iterator mit =
                    static_cast<const std::vector<stEventMonster_t>*>(
                        m_pDungeon->getEventMonsters())->begin();
                std::vector<stEventMonster_t>::const_iterator mitEnd =
                    static_cast<const std::vector<stEventMonster_t>*>(
                        m_pDungeon->getEventMonsters())->end();
                int acc = 0;
                int roll = get_rand_int(m_pDungeon->getEventMonsterTotalProb());
                for (; mit != mitEnd; ++mit)
                {
                    acc += mit->m_prob;
                    if ((unsigned int)roll >= (unsigned int)acc)
                        continue;

                    // 命中：随机位置 + 状态（0x18 字段族）
                    stEventMonsterStatus_t status;
                    status.m_eventIdx = mit->m_eventIdx;
                    int posIdx = get_rand_int((int)positions.size());
                    status.m_pos = positions.at(posIdx);
                    status.m_monsterIdx = mit->m_monsterIdx;
                    status.m_field10 = mit->m_field0c;
                    status.m_field14 = mit->m_field10;

                    MONSTER_POS mp;
                    mp.m_x = (char)status.m_pos.first;
                    mp.m_y = (char)status.m_pos.second;

                    // b28d-solo-BattleDepsB2：ORIG 0x8300558 `_List_iterator<MONSTER_POS>::C1Ev`
                    //   （先声明后赋值，P50 复活）+ 0x8300782 `operator==`
                    //   （非 operator!=，p167 差集实证）。
                    std::list<MONSTER_POS>::iterator sameIt;
                    sameIt = std::find(usedPositions.begin(), usedPositions.end(), mp);
                    if (sameIt == usedPositions.end())
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3e3, 0);
                        trace("Event monster generated (%d,%d ->%d(%d)",
                              status.m_pos.first, status.m_pos.second,
                              status.m_eventIdx, status.m_monsterIdx);
                        usedPositions.push_back(mp);
                        m_eventMonsterStatus.push_back(status);   // +0x17c
                    }
                    else
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3e8, 0);
                        trace("Event monster SameMap (not generated) (%d,%d ->%d(%d)",
                              status.m_pos.first, status.m_pos.second,
                              status.m_eventIdx, status.m_monsterIdx);
                    }
                    break;
                }
            }
            usedPositions.clear();
        }
    }

    // —— 第二事件怪生成（无去重、无日志）——
    if (m_pDungeon->getTryGenSecondEventMonsterCnt() != 0)
    {
        std::vector<std::pair<int, int> > positions;
        for (int row = 0; row < m_gridMaze->m_height; ++row)
        {
            for (int col = 0; col < m_gridMaze->m_width; ++col)
            {
                const GridScript* grid = &m_gridMaze->getGridR(col, row);
                if (grid->m_gridValue != 0)
                    positions.push_back(std::make_pair(col, row));
            }
        }

        for (int attempt = 0; attempt < m_pDungeon->getTryGenSecondEventMonsterCnt();
             ++attempt)
        {
            if (!(get_rand_int(10000) <=
                  (unsigned int)m_pDungeon->getTryGenSecondEventMonsterProb()))
                continue;

            // b28d-solo-BattleDepsB2：ORIG 0x8300a9f/0x8300ac5 对 getSecondEventMonsters()
            //   各调一次（begin/end 独立，P157）。
            std::vector<stSecondEventMonster_t>::const_iterator mit =
                static_cast<const std::vector<stSecondEventMonster_t>*>(
                    m_pDungeon->getSecondEventMonsters())->begin();
            std::vector<stSecondEventMonster_t>::const_iterator mitEnd =
                static_cast<const std::vector<stSecondEventMonster_t>*>(
                    m_pDungeon->getSecondEventMonsters())->end();
            int acc = 0;
            int roll = get_rand_int(m_pDungeon->getSecondEventMonsterTotalProb());
            for (; mit != mitEnd; ++mit)
            {
                acc += mit->m_prob;
                if ((unsigned int)roll >= (unsigned int)acc)
                    continue;

                stEventMonsterStatus_t status;
                status.m_eventIdx = mit->m_eventIdx;
                int posIdx = get_rand_int((int)positions.size());
                status.m_pos = positions.at(posIdx);
                status.m_monsterIdx = mit->m_monsterIdx;
                status.m_field10 = mit->m_field0c;
                status.m_field14 = mit->m_field10;
                m_eventMonsterStatus.push_back(status);
                break;
            }
        }
    }
