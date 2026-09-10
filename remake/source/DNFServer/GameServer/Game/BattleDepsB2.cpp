// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BattleDepsB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <list>
#include <utility>
#include <vector>

#include "BattleData.h"           // BattleData（CParty+0x32c 子对象，m_hellPartyValueTotal）
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h        // CBattle_Field + CMap/CDungeon/CParty/WarField 权威类型
#include "CGuildServerProxy.h"    // CEventBase::IsEventing / CEventManager::GetRepeatEvent
#include "DataManagerInitDepends.h"  // stHellParty_t（get_hellparty_script_values 返回值）
#include "Quest.h"               // Quest 完整类型（m_field120）
#include "GlobalData.h"           // GlobalData::s_systemTime_ / s_event_manager

// ---- MONSTER_POS（ORIG 全局类型：W operator== 0x830e6e9 双字节比较；
//      SetGridPath 局部 std::list<MONSTER_POS> 去重表。全源码此前无定义点。）----
struct MONSTER_POS
{
    char m_x;   // +0x00
    char m_y;   // +0x01
};

inline bool operator==(const MONSTER_POS& a, const MONSTER_POS& b)
{
    return a.m_x == b.m_x && a.m_y == b.m_y;
}

// ============================ CRidable ============================

// ORIG 0x0830ea0a：直接返回 m_objects.size()（call _ZNKSt3mapIi20DungeonRidableObject...4sizeEv）。
#include "BattleDepsB2/GetObjectCnt.inc"
{
    return (int)m_objects.size();
}

// ============================ CBattle_Field ============================

// ORIG 0x082ff988 CheckHellPartyCondition：
//   m_party 为空 → false；rate=(int)party->BattleData.m_hellPartyValueTotal(+0xb0c)；
//   rate >= get_rand_int(100) 且 rate >= stHellParty_t.m_guideLineValue(+0x90) → true。
// b28d-55（P0/P183）：ORIG 无 bd/vals 指针局部、无 ok 槽——party+0xb0c 两次
//   独立读取均单条 `flds 0xb0c(%eax)` 折叠，bool 汇聚在 %eax（g1 形探针
//   /tmp/b28d_l55/probe_bool.cpp）。故去掉中间指针缓存与具名 bool，改嵌套正向
//   if + return（P214①/P169）。残差：ORIG 尾部多一层 bool 归一岛（4.1 寄存器化
//   bool，4.4 具名局部必占槽）——P0 判读为无具名局部，落 N（跨版本 bool 物化）。
#include "BattleDepsB2/CheckHellPartyCondition.inc"
{
    // b28d-solo-BattleDepsB2：探针 /tmp/b28d_bdb2/probe_cp2.cpp f_fwd_and 实证——ORIG 形
    // =「正向包裹 NULL 检查 + `&&` 短路条件 + 尾部 return false」：
    //   test;je<尾部 mov$0> … jl<内层 mov$0> ×2 ; mov$1;jmp ; mov$0 ;
    //   test %al,%al;je<尾部> ; mov$1 ; mov$0。嵌套 if（f_cur 形）反产
    //   setge/test 物化（+2 条）且 NULL 失败块前置。f_fwd_and 50 条 + ORIG
    //   特有的 G_CDataManager 二次调用 2 条 = ORIG 52 条逐条同形。
    if (m_party != NULL)
    {
        if ((int)m_party->m_battleData.m_hellPartyValueTotal >= get_rand_int(100) &&
            (int)m_party->m_battleData.m_hellPartyValueTotal >=
                static_cast<stHellParty_t*>(
                    G_CDataManager()->get_hellparty_script_values())->m_guideLineValue)
        {
            return true;
        }
    }
    return false;
}

// ORIG 0x083031d2 ConsistMap：当前图装配怪物/物品链（逐调用与报告一致）。
#include "BattleDepsB2/ConsistMap.inc"
{
    if (m_party == NULL)
        return;

    MapInfo* mapInfo = GetCurrentMapInfo();
    m_pMap = getMap(mapInfo->m_mapIndex);        // 实参 = mapInfo->m_mapIndex(+0x04)
    if (m_pMap == NULL)
        return;

    setDungeonClearTrace(mapInfo->m_mapIndex, m_pMap->GetBossCount(), 0, 0);

    CDungeon* dungeon = G_CDataManager()->find_dungeon(m_dungeonIndex);
    if (dungeon == NULL)
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              __PRETTY_FUNCTION__, 0x7f2,
                              "G_CDataManager()->find_dungeon( %d) fail", m_dungeonIndex);
        return;
    }

    int addChampionCnt = getAddChampionMobCnt();
    if (addChampionCnt < 0)
        return;

    std::list<_mapMonster> mobList;
    if (!get_monster_list(mobList))
        return;

    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    add_monster_list_bossroom_condition(mobList, curX, curY);

    int monsterCnt = 0;                          // -0x28
    map_monster mob;                             // -0x390（默认构造 081511c8）
    std::vector<int> championList;               // -0x34
    // b28d-solo-BattleDepsB2：ORIG 槽序 seq(-0x38) 浅于 uid(-0x3c) 且 `movl $0,-0x38`
    //   （seq=0）先于 get_rand_int（+0x197/+0x19e）⇒ 声明序 seq 在 uid 前。
    int seq = 0;                                 // -0x38
    int uid = get_rand_int(60000);               // -0x3c
    add_monsters_current_map(mobList, championList, mob, monsterCnt, uid, seq);

    bool killedAll = false;                      // -0x3d
    add_monster_hell_party(mob, killedAll, uid, seq);

    // b28d-solo-BattleDepsB2：ORIG +0x236 `xor $1,%eax; test; je <成功路径>`、失败落
    //   `mov $0,%ebx; jmp <cleanup>` ⇒ 源码为否定守卫早返（P214①/P222）。
    if (!add_monster_blood(mob, uid, seq))
        return;

    {
        m_dungeonClearTracer.SetMaxMonsterCount(monsterCnt);
        add_monster_APC_AI(uid);
        add_monster_APC_AI_by_rate(mob, uid, curX, curY);
        add_monster_additional_champion(championList, addChampionCnt);
        add_monster_event(uid, curX, curY);
        set_monster_ability();
        insert_tournamentRoundMonster();

        int itemUid = get_rand_int(60000);       // -0x44
        add_item_curr_map(itemUid, uid);
        mapInfo->m_dropItemCnt = itemUid;        // +0x40

        // ORIG 083034ed..0830351c：m_pMap->m_ec(+0xec)==0 时取
        // *(uint*)(G_CDataManager()+0x49d0) 作 SelectDonsterItemDropLimit 上限。
        // 即 CDataManager::m_serverParameter.m_4968（CDataManager+0x68+0x4968）。
        // b28d-solo-BattleDepsB2：ORIG 无 dm/dropLimit 具名槽（test;jne; call; mov 0x49d0;
        //   jmp join; else mov 0xec; join: mov %eax,0x4(%esp)）⇒ 三目右值直传；
        //   具名局部版多 2 槽 4 条并把全帧推深 8（P0/P183）。
        mapInfo->SelectDonsterItemDropLimit(
            m_pMap->m_ec == 0
                ? static_cast<unsigned int>(
                      G_CDataManager()->m_serverParameter.m_4968)
                : static_cast<unsigned int>(m_pMap->m_ec));
    }
}

// ORIG 0x082ff128 GetAppropriateMaze(diff)：
//   按 questType 0/1 取任务迷宫向量；过滤血战态/hell 标记/任务连接/单人格挡；
//   questType==0 额外要求 maze->m_54 <= diff；候选 1 个直取、多个 rand()%n，
//   两轮无候选则回退 CDungeon::GetRandMazebyPartyCount(成员数)。
// b28d-solo-BattleDepsB2 定案（N 封顶 217/212）：已对齐项 = 正向复合条件嵌套（血战/hell/
//   连接/任务三过滤）、候选 size 三分支直接 return（无 picked/found 具名槽）、
//   push_back 右值重载、size() 三次独立调用；p167 CLEAR(25/25)。残差 = 4.1/4.4
//   槽序（maze 等局部 N82/N98）+ `get_member_count()>1` 的 setg 物化
//   （ORIG cmp/setg/test vs 4.4 直跳，P197④ 族）⇒ 封顶。
#include "BattleDepsB2/GetAppropriateMaze.inc"
{
    for (int questType = 0; questType < 2; ++questType)
    {
        std::vector<MazeScript>* mazes =
            m_pDungeon->GetQuestMazeScriptVector((ENUM_MAZE_QUEST_TYPE)questType);
        // b28d-solo-BattleDepsB2：ORIG 逐条结构（Ghidra C bVar2 汇聚 + 0x082ff128 段）：
        //   正向复合条件嵌套（血战/hell/连接/任务三条过滤），候选 size 三分支
        //   直接 return（无 picked/found 具名槽——ORIG 值在 %esi、标志在 %ebx）。
        if (mazes != NULL && !mazes->empty())
        {
            std::vector<MazeScript*> candidates;
            for (std::vector<MazeScript>::iterator it = mazes->begin();
                 it != mazes->end(); ++it)
            {
                MazeScript* maze = &*it;
                if ((int)m_bloodState != 1 &&             // +0x1b4
                    ((char)m_hellPartyActive == 0 || maze->m_70 == 0))
                {
                    if (m_party != NULL &&
                        m_party->CheckQuestConnection(
                            maze->m_50, (ENUM_MAZE_QUEST_TYPE)questType))
                    {
                        Quest* quest = G_CDataManager()->find_quest(maze->m_50);
                        if (quest == NULL || quest->m_field120 != 1 ||
                            m_party->get_member_count() < 2)
                        {
                            if (questType == 0)
                            {
                                if (maze->m_54 <= (int)diff)
                                    // b28d-55（P270/P234）：ORIG 走
                                    // push_back(T&&)=EOS1_（0x082ff2a4 实证）；
                                    // static_cast<T&&> 是纯语言构造（零额外 call）
                                    // 且选中同一右值重载，P167 差集 25/25 CLEAR。
                                    candidates.push_back(static_cast<MazeScript*&&>(maze));
                            }
                            else
                            {
                                candidates.push_back(static_cast<MazeScript*&&>(maze));
                            }
                        }
                    }
                }
            }

            // b28d-55（P167 差集实证）：ORIG 三次独立 size() 调用
            // （`==1`→sete / `>1`→seta / `%size()`→结果缓存 -0x3c 槽），
            // 无 cnt 具名缓存。
            if (candidates.size() == 1)
                return candidates[0];
            if (candidates.size() > 1)
                return candidates[::rand() % candidates.size()];
        }
    }
    return (MazeScript*)m_pDungeon->GetRandMazebyPartyCount(m_party->get_member_count());
}

// ORIG 0x082ffa2e GetHellPartyDifficulty(type)：
//   六字节难度比例取自 stHellParty_t（type==1 → m_field84[6]，否则 m_field7e[6]
//   且非空队伍经 increase_hell_party_ratio 提升），rnd=get_rand_int(100) 落区间
//   [i]=1..5 返回 i，否则 0（下标 0 槽不参与）。
#include "BattleDepsB2/GetHellPartyDifficulty.inc"
{
    // ORIG 0x082ffa2e：ratios `{0}` 单零聚合标量（P202① movl+movw 两写）+
    // get_hellparty_script_values 各分支独立重调（P48）。
    char ratios[6] = { 0 };
    if ((int)type == 1)
    {
        memcpy(ratios, static_cast<stHellParty_t*>(
                   G_CDataManager()->get_hellparty_script_values())->m_field84, 6);
    }
    else
    {
        memcpy(ratios, static_cast<stHellParty_t*>(
                   G_CDataManager()->get_hellparty_script_values())->m_field7e, 6);
        if (m_party != NULL)
            m_party->increase_hell_party_ratio(ratios);
    }

    // ORIG 0x082ffa2e：for(i=1;i<=5;i++) 双入口（进入直接落底部守卫 +0xc6，
    //   setle/test/jne）+ `if(acc>=rnd) return i`（mov acc;cmp rnd;jl continue，
    //   与 ORIG +0xb5/+0xbb 同形）；守卫落空 return 0（P0 具名局部 acc/rnd/i）。
    int acc = 0;
    int rnd = get_rand_int(100);
    int i;
    for (i = 1; i <= 5; i++)
    {
        acc += ratios[i];                        // movsbl 有符号字节累加
        if (acc >= rnd)
            return i;
    }
    return 0;
}

// ORIG 0x082ffb06 GetHellPartyEventType(diff)：查 stHellParty_t.m_field8a[diff]。
// b28d-solo-BattleDepsB2 定案（N 封顶 13/14）：残差为 base/index 寄存器放置——ORIG 留 call
//   结果于 %eax、索引 `movsbl -0xc(%ebp),%edx`；4.4 ISel 恒 index→%eax、base 挪
//   %edx（多 1 条 `mov %eax,%edx`）。探针定案：唯一旋钮 = getter 返回类型
//   void*→stHellParty_t*（/tmp/b28d_bdb2/probe3.cpp h1 = 14 条、probe4.cpp a1
//   = 13 条 = ORIG 形；4.4.7 与 4.4.6 后端读数一致，非路由敏感）。该改动涉
//   CDataManager.h + 7 个消费 .cpp 的 static_cast<const float*>，Main 裁决本波
//   不落（收益 1 件 < 跨 TU 风险）⇒ 封顶。
#include "BattleDepsB2/GetHellPartyEventType.inc"
{
    return (signed char)static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_field8a[(int)diff];
}

// ORIG 0x08308ae6 MoveMap(x, y, firstVisit)：换图主流程（错误码 0/1/0x12/0x14/0x15 或检查函数码）。
#include "BattleDepsB2/MoveMap.inc"
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
}

// ORIG 0x082ffb30 SetGridPath(maze, dummyAppearCnt)：
//   记录迷宫宽高与起点/boss 槽位链、dummy 图序列、hell 三字段、ridable 对象；
//   bossRoom90c==1 时装配进 boss 房条件组；事件怪两轮生成（0x24 重复事件判活 +
//   MONSTER_POS 去重 / 第二事件不去重）。
// b28d-solo-BattleDepsB2（超大件 §6.2，N 封顶 1101/1121）修法与残差：
//   * 全程 `m_gridMaze->` 成员访问（ORIG 0x74(%eax) 装载实证，非形参 maze）；
//   * 三个 pair C1Ev 默认构造 + 赋值（无 dummyPos 具名局部，临时槽直传
//     push_back）；`size()==0` 判空（非 empty()）；
//   * getEventMonsters/getSecondEventMonsters 各两次独立调用（begin/end 内联
//     receiver，P157）+ const 版 begin/end（编译期 const 指针转换）；
//   * MONSTER_POS 去重：`_List_iterator` 先声明后赋值（P50 复活）+ operator==
//     （非 operator!=）；mapCandidates 循环后显式 `cit == citEnd` 判定（无 ok bool）。
//   p167 差集仅剩 EXTRA stEventMonsterStatus_t::~D1Ev ×4：ORIG 的 D1 为头内 W
//   空体（08316adc push/pop/ret），4.1 前端省略该调用；4.4 对 out-of-line 空 dtor
//   恒发射。消除需 CBattle_Field.h 头内化 + CBattle_Field.cpp 删定义（非本泳道
//   .cpp，已上报 Main）。其余残差为槽基/帧差（N82/N95）。
#include "BattleDepsB2/SetGridPath.inc"
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
}

// ORIG 0x0830e32e checkLayeredMap(x, y)：迷宫格带层叠表则推进层号并置当前图层标记。
#include "BattleDepsB2/checkLayeredMap.inc"
{
    MazeScript* maze = getMaze();
    if (maze != NULL)
    {
        const GridScript* grid = &maze->getGridR(x, y);
        if (grid != NULL && grid->m_layeredMapIndexes.size() != 0)
        {
            if ((int)grid->m_layeredMapIndexes.size() > getLayeredMapIndex() + 1)
                setLayeredMapIndex();

            getCurrentLayeredMapInfo()->m_layeredMap = 1;    // MapInfo+0xe8
        }
    }
}

// ORIG 0x0830dbde onUseItem(user, item)：技能材料计时 + 用物计数。
#include "BattleDepsB2/onUseItem.inc"
{
    MapInfo* mapInfo = GetCurrentMapInfo();

    if (item.m_addInfo == 0xbdd)                 // +0x02 dword（技能材料物品）
    {
        // P0：ORIG tick 无槽（call 结果 eax 直通实参），内联右值。
        mapInfo->m_killChecker1.setUseSkillMaterial(
            GlobalData::s_systemTime_.getCurTickCount());   // +0x4c
        mapInfo->m_killChecker2.setUseSkillMaterial(
            GlobalData::s_systemTime_.getCurTickCount());   // +0x64
    }

    if ((unsigned char)item.m_field1 == 2)
        ++mapInfo->m_playInfo.m_useItemType2Count;          // +0x84
    else
        ++mapInfo->m_playInfo.m_useItemOtherCount;          // +0x88
}

// ORIG 0x0830e170 setQuestMazeInfo()：扫描两类任务迷宫，符合条件者置任务迷宫标记。
// b28d-solo-BattleDepsB2 定案（NEAR 117/117，N-P75 封顶）：已对齐 = for(i<=1) 尾测 setle、
//   `mazes!=NULL && !empty()` 正向包裹、while(it!=itEnd) 三槽阶梯、continue 中继
//   nop 岛、全部失败路径汇入循环尾 ++it（ORIG +0x154）。残差仅 2 处 receiver/
//   实参装载序（ORIG 先 receiver 后实参；4.4 恒先算实参，probe_ord2 f1..f10
//   十形全同，4.4.6 后端读数逐字节一致）⇒ 跨版本装载时刻，封顶。
#include "BattleDepsB2/setQuestMazeInfo.inc"
{
    // b28d-solo-BattleDepsB2（ORIG 0x0830e170 逐条）：外层 `for(i=0;i<=1;++i)`（尾测 setle，P2）；
    //   `mazes!=NULL && !empty()` 正向包裹（ORIG +0x02e/+0x03f xor$1 双条件共享
    //   失败块）；内层 while(it!=itEnd)（入口 jmp 到 +0x15f 尾测，P25）；所有失败
    //   路径汇入循环尾 `++it`（+0x154）；连接判定直接 `if (m_party!=NULL && call)`
    //   正向包裹（ORIG +0xa9..+0xe0 bool 物化岛）。
    for (int questType = 0; questType <= 1; ++questType)
    {
        std::vector<MazeScript>* mazes =
            m_pDungeon->GetQuestMazeScriptVector((ENUM_MAZE_QUEST_TYPE)questType);
        if (mazes != NULL && !mazes->empty())
        {
            std::vector<MazeScript>::iterator it = mazes->begin();
            std::vector<MazeScript>::iterator itEnd = mazes->end();
            for (; it != itEnd; ++it)
            {
                MazeScript* maze = &*it;
                if ((int)m_bloodState == 1)
                {   // 血战态跳过（ORIG je 落 +0x153 nop 中继岛）
                    continue;
                }
                if (m_party != NULL &&
                    m_party->CheckQuestConnection(
                        maze->m_50, (ENUM_MAZE_QUEST_TYPE)questType))
                {
                    Quest* quest = G_CDataManager()->find_quest(maze->m_50);
                    if (quest != NULL)
                    {
                        if (quest->m_field120 == 1)
                        {   // 单人限定任务迷宫：仅单人队伍置位
                            if (m_party->get_member_count() == 1)
                                setQuestMaze(true);
                        }
                        else
                        {
                            setQuestMaze(true);
                        }
                    }
                }
            }
        }
    }
}
