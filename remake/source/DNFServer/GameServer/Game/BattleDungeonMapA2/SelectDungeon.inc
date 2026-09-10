bool CBattle_Field::SelectDungeon(int dungeonIdx, const CDungeon* dungeon,
                                  int diff, ENUM_DUNGEON_TYPE type,
                                  int memberCount, CWorldMap* worldMap,
                                  bool isHellParty, int questStageMap)
{
    (void)memberCount;  // 第 5 参仅落存 m_field198（ORIG 0x082ff4c1）

    m_dungeonIndex = dungeonIdx;                       // +0x194
    set_dungeon_diff((char)diff);                      // ORIG set_dungeon_diff(char)
    m_field198 = memberCount;                          // +0x198
    m_questStageMap = questStageMap;                   // +0x19c
    m_hellPartyActive = 0;                             // +0x1b8
    // ORIG @0x41 为 LHS-first `mov this; mov type; store` ⇒ 字段与 RHS 同型
    // （P211 档③）；用权威头匿名 union 的 enum 视角（int 视角保留给其他消费点）。
    m_hellPartyModeE = type;                           // +0x1b0

    // ORIG 三处 logFormat 第 3 参为 `_ZZN13CBattle_Field13SelectDungeon…
    // E19__PRETTY_FUNCTION__` 局部静态（P118/P139）⇒ 必须传 __PRETTY_FUNCTION__，
    // 手写字符串只产匿名 .rodata 且留 missing。
    // 三条件各自 je 直达统一出口（ORIG @0x1e/@0x25/@0x36 →0x22a）：`&&` 链在
    // 4.4 末尾条件折 bool 聚合岛（mov$1/jmp/mov$0，+3），正向嵌套同形 ORIG；
    // ORIG @0x63 为 `call; test %al,%al; je`（bool callee 直判，P292）⇒ 裸调用。
    if (IsEnableHellDungeon())
    {
    if (worldMap != 0)
    {
    if (worldMap->IsInHellDungeon())
    {
        if (type == 1)
        {
            m_party->sub_hellparty_value();
            m_hellPartyDifficulty = (char)GetHellPartyDifficulty(type);
            if (m_hellPartyDifficulty == 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1fc,
                    "HELLPARTY_LOG : HELLPARTY DIFFICULTY ERROR!!\n");
                return false;
            }
            m_hellPartyEventType = (char)GetHellPartyEventType(m_hellPartyDifficulty);
            m_hellPartyActive = 1;
        }
        else
        {
        // ORIG else 链同直跳形：isHellParty（-0x2fc 参数叶局部槽 cmpb/je）、
        // member_count != 1（setne 物化为 4.1 && 行为，嵌套后 N 封顶）、
        // CheckHellPartyCondition 结果具名 bool 落槽回读（@0x155 mov%al,-0x9）。
        if (isHellParty)
        {
        if (m_party->get_member_count() != 1)
        {
            bool hellCond = CheckHellPartyCondition();
            if (hellCond)
            {
                m_party->sub_hellparty_value();
                m_hellPartyActive = 1;
                m_hellPartyRandom = isHellParty;       // +0x1ce（byte 直copy）
                m_hellPartyDifficulty = (char)GetHellPartyDifficulty(type);
                if (m_hellPartyDifficulty == 0)
                {
                    LogManager::logFormat(
                        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x22b,
                        "HELLPARTY_LOG : HELLPARTY DIFFICULTY ERROR!!\n");
                    return false;
                }
                m_hellPartyEventType = (char)GetHellPartyEventType(m_hellPartyDifficulty);
            }
        }
        }
        }
    }
    }
    }

    if (dungeon != 0 && !dungeon->m_89d)
        m_hellPartyActive = 0;                         // +0x89d 非 1 清标记

    m_pDungeon = const_cast<CDungeon*>(dungeon);       // +0x188
    // ORIG 082ff6e9 读 *(dungeon + (0x1b8+diff)*4) = m_6e0[diff]（每图怪物基数 int，
    //   getAddChampionMobCnt 按难度缩放；非 MazeScript* 表）。
    m_mapMobCount = dungeon->m_6e0[diff];              // +0x70
    m_field1a4 = 0;                                    // +0x1a4

    MazeScript* maze = GetAppropriateMaze((ENUM_DUNGEON_DIFFICULTY)diff);
    m_mazeType = maze->m_64;                           // +0x1ac ← maze+0x64
    setQuestMazeInfo();
    m_clearCondition.SetDungeonClearConditionVector(maze->m_clearCondition);
    SetGridPath(maze, dungeon->GetDummyAppearCount());

    int posX = -1;
    int posY = -1;
    getCurPosXY(posX, posY);
    const GridScript* grid = &m_gridMaze->getGridR(posX, posY);
    if (grid == 0)
        return false;

    // ORIG @0x34b：`movl $0x0,-0x10` 先置 0 再赋 GetStartMap 结果（P198③
    // 声明=0 + 独立赋值两句式）。
    int startMap = 0;
    startMap = dungeon->GetStartMap(grid->m_gridValue);
    if (startMap < 1)
    {
        LogManager::logFormat(1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x27c,
                              "[CBattle_Field::SelectDungeon] Error Map : (%d)(%d,%d)\n",
                              dungeonIdx, m_curX, m_curY);
        return false;
    }

    MapInfo newMapInfo;
    newMapInfo.m_mapIndex = startMap;                  // +0x04
    newMapInfo.m_gridValue = (char)grid->m_gridValue;  // +0x08（byte 写 %al）
    int key = GetMapInfoKeyValue(posX, posY);
    m_mapInfo.insert(std::make_pair(key, newMapInfo));
    ConsistMap();
    return true;
