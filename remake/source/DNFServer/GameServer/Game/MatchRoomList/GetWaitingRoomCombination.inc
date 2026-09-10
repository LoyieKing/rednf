bool CMultiMatchRoomList::GetWaitingRoomCombination(IMatch* me, int playCount,
                                                    std::vector<IMatch*>& out)
{
    // b28d-solo-MatchRoomList VERDICT: DIFF（447/446 均 424 vs 435）| 修法：
    // for+break 重构、totalPlayers 头声明、min 直读、doMatch 两独立守卫、
    // avgScore 前置、bestAvg/bestIdx 赋值序、count int 物化 | N82（槽布局）
    // + N86b（三维下标强度归约：4.4 恒 imul $0x8c/add 链 vs ORIG imul
    // $0x46/scaled；一维视图探针 g1b 实测净负已回退）
    // 与 ORIG 同名的函数内静态常量/组合表（数据导出自 0x8ca3200，
    // 0x8ca35d4 起为四个 1 字节常量）。
    static const unsigned char MAX_SAME_COUNT = 2;
    static const unsigned char MAX_WAITING_ROOM_COUNT = 7;
    static const unsigned char MAX_COMBINATION_COUNT = 10;
    static const unsigned char MAX_REMAIN_WAITING_ROOM = 7;
    static const unsigned char CombinationTable[MAX_REMAIN_WAITING_ROOM]
        [MAX_COMBINATION_COUNT][MAX_WAITING_ROOM_COUNT * 2] = {
        /* playCount=0（无效区，全 0） */
        { {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=1：1 队 x 1 人 */
        { {1,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=2：1x2 / 2x1 */
        { {1,2,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=3：1x3 / 2x1+1x1 / 3x1 */
        { {1,3,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {3,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=4：1x4 / 2x1+1x2 / 2x1+2x1 / 3x1+1x1 / 4x1 */
        { {1,4,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,2,0,0,0,0,0,0,0,0,0,0},
          {2,1,2,1,0,0,0,0,0,0,0,0,0,0}, {3,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {4,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=5：1x5 / 2x1+1x3 / 2x2+1x1 / 3x1+2x1 / 3x1+1x2 / 4x1+1x1 */
        { {1,5,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,3,0,0,0,0,0,0,0,0,0,0},
          {2,2,1,1,0,0,0,0,0,0,0,0,0,0}, {3,1,2,1,0,0,0,0,0,0,0,0,0,0},
          {3,1,1,2,0,0,0,0,0,0,0,0,0,0}, {4,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=6：1x6 / 2x1+1x4 / 2x2+1x2 / 2x3 / 3x1+1x3 /
           3x1+2x1+1x1 / 4x1+1x2 / 4x1+2x1 / 5x1+1x1 */
        { {1,6,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,4,0,0,0,0,0,0,0,0,0,0},
          {2,2,1,2,0,0,0,0,0,0,0,0,0,0}, {2,3,0,0,0,0,0,0,0,0,0,0,0,0},
          {3,1,1,3,0,0,0,0,0,0,0,0,0,0}, {3,1,2,1,1,1,0,0,0,0,0,0,0,0},
          {4,1,1,2,0,0,0,0,0,0,0,0,0,0}, {4,1,2,1,0,0,0,0,0,0,0,0,0,0},
          {5,1,1,1,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
    };

    int totalPlayers = 0;
    float avgScore[MAX_COMBINATION_COUNT] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    std::vector<IMatch*> combination[MAX_COMBINATION_COUNT];
    std::multimap<float, IMatch*> matched;
    int resultCount = 0;
    int combIdx = 0;

    // b28d-solo-MatchRoomList: for 循环 + 首行标志 break（ORIG 条件在底部、
    // 收尾块在循环外）；totalPlayers 函数头声明、每轮重置。
    // 注：一维视图 (ci*7+pc*70+j)*2 探针（/tmp/b28d_gwc_probe.cpp g1b）实测
    // 外层 j=0 被 4.4 常量折叠回 imul $0x8c、内层多 1 条中转 ⇒ 净负，保留
    // 三维下标（4.4 强度归约与 ORIG 不同形，判 N86b）。
    for (combIdx = 0; combIdx <= 9; ++combIdx) {
        if (CombinationTable[playCount][combIdx][0] == 0)
            break;
        totalPlayers = 0;
        for (int j = 0; j < MAX_WAITING_ROOM_COUNT; ++j) {
            if (CombinationTable[playCount][combIdx][j * 2] == 0)
                break;
            matched.clear();
            // ORIG：if 检查为表达式直读（test %al），find 侧再算一次并物化 int 槽
            unsigned int count =
                (unsigned char)CombinationTable[playCount][combIdx][j * 2];
            // ORIG 对 find 结果不做 end() 检查（GetPlayCountPerOneTeam=1 时
            // 组合表只使用 key=1 的列表），此处忠实还原。
            std::map<int, CWaitingRoomList>::iterator it = m_roomMap.find(count);
            // ORIG 两个独立守卫（xor$1/test/jne 与 size<min 各一 break 岛）
            if (!it->second.doMatch(me, matched))
                break;
            if (matched.size() <
                CombinationTable[playCount][combIdx][j * 2 + 1])
                break;
            int k = 0;
            for (std::multimap<float, IMatch*>::iterator mit = matched.begin();
                 mit != matched.end() &&
                 k < CombinationTable[playCount][combIdx][j * 2 + 1];
                 ++mit, ++k) {
                avgScore[combIdx] = (totalPlayers * avgScore[combIdx] +
                                     mit->second->GetPlayerCount() * mit->first) /
                                    (float)(mit->second->GetPlayerCount() + totalPlayers);
                totalPlayers += mit->second->GetPlayerCount();
                combination[combIdx].push_back(mit->second);
            }
        }
        if (!combination[combIdx].empty()) {
            if (totalPlayers == playCount)
                ++resultCount;
            else
                combination[combIdx].clear();
        }
    }
    if (resultCount == 0)
        return 0;
    bool found = false;
    int bestIdx = 0;
    float bestAvg = 0.0f;
    for (int i = 0; i < MAX_COMBINATION_COUNT; ++i) {
        if (!combination[i].empty()) {
            if (!found) {
                bestIdx = i;
                bestAvg = avgScore[i];
                found = true;
            }
            if (avgScore[i] == bestAvg) {
                bestAvg = avgScore[i];
                bestIdx = i;
            }
        }
    }
    out.insert(out.end(), combination[bestIdx].begin(),
               combination[bestIdx].end());
    return 1;
