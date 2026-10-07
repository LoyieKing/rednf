// 结构终态（o=704 n=689）：七段语义/控制流全对齐——monster 短路空体、ai
// empty()==false 岛、dungeonList 条件序、item×dungeon 三层 at() 汇聚岛、
// hash 扫描 for+break（found 检查体首 0x08362481）、无条件 dungeonList 复扫
// (0x0836283c)、missing 判定后置、entrance 读取弃。残余 ndiff 归因：
//  跨版本 this-cache 物化——4.1.2 对局部容器成员调用先 `lea obj; mov %eax,-slot`
//  （-0x58=&monsterList、-0x54=aiList 等，0x08362125 实证），4.4.7 直发 lea 不
//  物化，全帧槽号系统性平移（P112 族）；条目基址/负值检查行随之整块错位。
// [b28d-solo-CDataManager DIFF-N] 历轮翻正：① monsterList 具名指针
// `pMonsterList`（ORIG 0x08362125 `lea -0xd0; mov %eax,-0x58`，P0 槽往返）；
// ② 空体短路链（ORIG 0x08362153 两处条件跳转）。
// [SweepCDataManager 2026-09-09 本轮] gap 12→5（o 704 / n 699，.o 口径），
// 帧尺寸 0x130 与 ORIG 一致，p167 94/94 CLEAR。翻正项：
//  ① **真 bug**：hash 扫描条件极性反——ORIG 0x0836251c `call empty;
//     test %al,%al; je 0836292a`（非空跳第二段、**空**落扫描体，Ghidra
//     `if (cVar1=='\0') { LAB_0836292a: }` 同证）；旧 `empty()==false` 语义反。
//  ② ai 循环删具名 apcType、实参内联（ORIG 0x08362227 `mov (%eax),%eax;
//     mov %eax,%ebx` 跨 G_CDataManager 保活，P52）+ get() 结果具名判空
//     （ORIG -0x40 槽 + `cmpl $0x0,槽; jne`，P197④）。
//  ③ usable 判定改正极性短路 `m_objectIdx != -1 && !empty()`（ORIG
//     0x0836240b/0x0836241d 两 je 同落 FALSE 块 + `mov $1/$0` 物化，P214①）。
//  ④ maze 两条 find 改 `||` 短路（ORIG 0x08362787 `jne 真块`/0x0836281a
//     `je 假块`/0x0836281c `mov $1/jmp`+`mov $0`，P214①）。
//  ⑤ mazeList 尾测 unsigned（ORIG 0x083628a1 `cmp -0x24,%eax; jbe`，P196/P218）。
//  ⑥ itemList 具名指针 `pItemList`（ORIG 0x0836231f `mov %eax,-0x50`）。
//  ⑦ aiList/内层 dungeonList/第二段 dungeonList 三处 end() 提到循环初始化处
//     缓存（ORIG 0x08362206 / 0x08362384 / 0x0836297f，P13）。
// 残余 5 条为跨版本 -O0 DCE/布局差，不可翻：
//  a) 0x08362401 `cmp %edx,%eax`（size()==m_objectIdx 死比较）与
//     0x0836248c `cmp $0xffffffff,%eax`（at().m_assignItemIdx==-1 死比较）：
//     4.1 -O0 保留纯表达式语句的比较，4.4 DCE（探针 /tmp/probe_deadcmp.cpp
//     h1/h3 加 __builtin_expect 反多 sete/movzbl/test 三条）。
//  b) 0x0836282c 处 `jmp` 短程 + 1 条 nop 填充（跳转距离布局）。
//  c) 0x08362a61 `cmp $0x64,%eax` 死比较 vs 我方 `mov %eax,-0xc`（同 N）。
//  d) [SweepCDataManager-2 复核补] 0x083629d9 `call empty; test %al,%al`
//     （m_rivalData2.empty() 返回值归一化）我方缺 test 1 条；探针
//     /tmp/probe_dce2.o f1/f2/f4 复证 4.4 -O0 恒删无消费者比较/test
//     （f7 具名 bool / f8 volatile 反多 store 与 movzbl）。
// [b28d-CDataManager DIFF→NEAR] 死比较/死 test 复活三处 + volatile 死读一处：
//   ① passive->size()==item.m_objectIdx ② 第三次 at() 的 !=-1 ③ m_rivalData2
//   .empty() ④ e.first vs 0x64 —— ORIG 均保留比较/test（0x08362401/0x0836248c/
//   0x083629d7/0x08362a5e），我方原写法（裸表达式语句 / 具名 int cmp / 空体 if）
//   被 4.4 tree-DCE。修法实测：① ② 空体 if（探针 /tmp/probe_deadcmp2.cpp f5/f6：
//   `call; cmpl $-1,%eax` 逐条同形）③ `while (cond) break;`（/tmp/probe_dead4.cpp
//   f17：`call; testb %al,%al` 精确复现；空体 if 的 test 会被 4.4 删，f15）
//   ④ `*(volatile const int*)&e.first`（/tmp/probe_dead3.cpp f21）。
//   读数 697/703 ndiff7 → **703/703、归一化 ndiff=0**（DIFF→NEAR）。
// 残余 NEAR 归因（5 处聚合对象槽归属，N82附 族：4.4 聚合对象分区 vs 4.1 生命周期
//   穿插）：dungeonList ORIG -0xd8 / 我方 -0xd0；itemList ORIG -0xc0 / 我方 -0xd8；
//   ~list<_mapMonster> ORIG -0xd0 / 我方 -0xc8；end 临时 -0xc8/-0xc0；it++ 临时
//   -0xc4/-0xbc。声明序=构造序=指令序，调换会改 push_back/get_item_list 调用序
//   ⇒ 无源码解。
bool CDataManager::VerifyMap(const CMap& map, int idx) const
{
    // ORIG 0x083620ee：map type 与 monster list 只被读取，结果不参与返回。
    (void)map.get_map_type();
    const std::list<_mapMonster> monsterList = map.get_monster_list();
    // ORIG 0x08362125：`lea -0xd0; mov %eax,-0x58` —— monsterList 经具名指针
    // 局部访问（-0x58 槽，P0 槽往返判据），begin/end 实参从该槽重载。
    const std::list<_mapMonster>* pMonsterList = &monsterList;
    for (std::list<_mapMonster>::const_iterator it = pMonsterList->begin();
         it != pMonsterList->end(); it++)
    {
        // ORIG 0x08362153..：`test %al; jne ++; mov 0x8; test %eax` 尾 test 无
        // 消费者 = `flag==0 && field` 空体短路链（结果丢弃，不物化槽）。
        const _mapMonster& mon = *it;
        // ORIG 0x08362153..：`movzbl 0x4; test; jne INCR; mov 0x8; test`
        // ——内层空体 if 的 je 被 4.1 jump-to-next 删除、外层保留。4.4 对裸空
        // if 会连条件求值一起 DCE；`__builtin_expect(cond,0)` 使其保留求值并
        // 同样删除内层 jcc（探针 /tmp/probe_mon5.cpp g8 逐条同构）。
        if (mon.m_flag04 == 0)
        {
            if (__builtin_expect(mon.m_field08 == 0, 0))
            {
            }
        }
    }

    const std::vector<STAICharacterMapArrangeData>* aiList =
        map.getAICharacters();
    if (aiList->empty() == false)
    {
        // ORIG 0x083621f6..0x0836220b：end() 在循环**初始化**处求值一次并缓存
        // 到具名 itEnd(-0xe4)，条件块 0x0836229d 只调 ne（P13）——旧形
        // `it != aiList->end()` 把 end() 留在条件块（每次迭代重调）。
        for (std::vector<STAICharacterMapArrangeData>::const_iterator it =
                 aiList->begin(),
                 itEnd = aiList->end(); it != itEnd; ++it)
        {
            const STAICharacterMapArrangeData& ai = *it;
            // ORIG 0x08362..：经 G_CDataManager() 全局取 m_aiCharacterList（+0x869c）。
            // 实参 ai.m_type 在 G_CDataManager() 之前求值并跨调用保活
            // （ORIG 0x08362227 `mov (%eax),%eax; mov %eax,%ebx`，P52/P0：
            // 无具名 apcType 槽）；结果具名指针后判空（ORIG -0x40 槽 +
            // `cmpl $0x0,槽; jne`，P197④ 槽比零直跳）。
            CAICharacter* pAICharacter =
                G_CDataManager()->m_aiCharacterList->get(ai.m_type);
            if (pAICharacter == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp", __PRETTY_FUNCTION__, 0x2847,
                    "Unknown APC Type(%d). Map(%d)", ai.m_type,
                    map.get_index());
            }
        }
    }

    std::list<int> dungeonList;
    // ORIG 0x083622cb..：idx != -1 → push_back(&idx)（形参槽按引用）；
    // 否则拷贝赋值 getDungeonLists() 解引用。
    if (idx != -1)
        dungeonList.push_back(idx);
    else
        dungeonList = *map.getDungeonLists();

    const std::list<_mapItem> itemList = map.get_item_list();
    // ORIG 0x0836231f..：`lea -0xc0; mov %eax,-0x50` —— itemList 经具名指针
    // 局部访问（-0x50 槽，P0 槽往返判据），begin/end 实参从该槽重载。
    const std::list<_mapItem>* pItemList = &itemList;
    for (std::list<_mapItem>::const_iterator itemIt = pItemList->begin();
         itemIt != pItemList->end(); itemIt++)
    {
        const _mapItem& item = *itemIt;
        // ORIG 0x08362369/0x08362384：内层 dungeonList 的 begin/end 同在
        // 循环初始化处求值（end 缓存 -0xf0），条件块只调 ne（P13）。
        for (std::list<int>::const_iterator dungeonIt = dungeonList.begin(),
                 dungeonEnd = dungeonList.end();
             dungeonIt != dungeonEnd; ++dungeonIt)
        {
            CDungeon* dungeon = find_dungeon(*dungeonIt);
            const std::vector<stDungeonAssignItem_t>* passive =
                dungeon->getSpecialPassiveObjectItems();
            // ORIG 0x08362401 `call size; mov (%edx),%edx; cmp %edx,%eax`：
            // size() 结果与 m_objectIdx 的死比较（无消费者；4.4 把 cmp DCE、
            // 仅保留 call —— 跨版本 -O0 DCE 差，N 封顶）。
            // [b28d-CDataManager 死比较复活] 裸表达式语句的 cmp 被 4.4 tree-DCE、
            // 4.1 保留 ⇒ 改空体 if：两侧均 jump-to-next 删 jcc、保留比较（探针
            // /tmp/probe_deadcmp2.cpp f5/f6：`call; cmpl $-1,%eax` 逐条同形）。
            if (passive->size() == (unsigned int)item.m_objectIdx)
            {
            }
            // ORIG 0x0836240b/0x0836241d：`m_objectIdx != -1` 与 `!empty()` 两
            // 守卫共落 FALSE 块、`mov $1/jmp/mov $0` 寄存器物化 ⇒ 正极性短路
            // 形（P214①；旧 usable 具名 bool 产生槽物化 movb/cmpb 三连）。
            if (item.m_objectIdx != -1 && !passive->empty())
            {
                // ORIG 0x0836243e/0x0836245d：两分支各自重取 at()（P48），
                // 命中再第三次 at() 读弃（0x08362476）。
            if (passive->at(item.m_objectIdx).m_assignItemIdx < 1 ||
                passive->at(item.m_objectIdx).m_assignItemIdx > 200)
                {
                // [b28d-CDataManager 死比较复活] ORIG 0x0836248c
                // `mov (%eax),%eax; cmp $0xffffffff,%eax` = 第三次 at() 的死比较；
                // 空体 if 保留比较、jcc 被 jump-to-next 删除（同 f6 探针）。
                if (passive->at(item.m_objectIdx).m_assignItemIdx != -1)
                {
                }
                }
            }
        }
    }

    // ORIG 0x0836242a 起：missingDungeon(-0x49) 先置 0；hash 全表扫描仅在
    // dungeonList **为空**侧执行——0x0836251c `call empty; test %al,%al;
    // je 0836292a`：empty()!=0（空）落扫描体，非空直接跳到第二段
    // （Ghidra `if (cVar1=='\0') { LAB_0836292a: 第二段 }` 同证）；found(-0x2d)
    // 检查位于循环体首（for+break 形，0x08362481）；扫描后无条件再过一遍
    // dungeonList（0x0836283c，空表 0 轮），missing 判定在两段扫描之后。
        bool missingDungeon = false;
    if (dungeonList.empty())
    {
    bool found = false;
    for (__gnu_cxx::hash_map<int, CDungeon*>::const_iterator it =
             m_dungeonList->m_dungeonMap.begin();
             it != m_dungeonList->m_dungeonMap.end(); ++it)
        {
            if (found)
                break;
        PvPMapInfo pvpInfo;
        (void)pvpInfo;
        // ORIG 0x08362592/0x083625a4：范围判定两次独立 get_index（P44 实参即时求值）。
        if (map.get_index() > 0xc350 && map.get_index() <= 0xc738)
            {
                found = true;
                break;
            }

        CDungeon* dungeon = it->second;
        if (dungeon->getDungeonKind() == 1)
            {
                // ORIG 0x083625e3..：直读 m_deathTowerMaxStage(+0x680) 字段 +
                // m_deathTowerMapIndexes(+0x674) operator[]，非 getter。
            for (int stage = 1;
                 stage <= dungeon->m_deathTowerMaxStage && !found;
                 ++stage)
                {
                if (dungeon->m_deathTowerMapIndexes[stage] == map.get_index())
{
                        found = true;
            break;
    }
                }
            }

            // ORIG 0x08362657..：m_mazeList(+0x62c).size() + operator[] 索引。
            // ORIG 0x083628a1 `cmp -0x24(%ebp),%eax; jbe` 为**无符号**尾测
            // （P196/P218）⇒ mi 声明 unsigned（旧 `(int)size()` 强转产 jle）。
        for (unsigned int mi = 0; mi < dungeon->m_mazeList.size() && !found; ++mi)
            {
            const MazeScript& maze = dungeon->m_mazeList[mi];
            if (maze.m_sealDoorMapIdx == map.get_index())
                {
                    found = true;
                break;
                }
            for (int y = 0; y < maze.m_height && !found; ++y)
                {
                for (int x = 0; x < maze.m_width && !found; ++x)
    {
                    const GridScript& grid = maze.getGridR(x, y);
                    if (grid.m_gridValue == 0)
                continue;
                        // ORIG 0x0836276a/0x08362803：两条 find 走 `||` 短路
                        // 物化（0x08362787 `jne 真块`、0x0836281a `je 假块`、
                        // 0x0836281c `mov $1/jmp` + `mov $0`），命中即 found；
                        // 旧 if/else-if 两独立块多一组 movb/jmp（P214①）。
                    if (std::find(grid.m_mapCandidates.begin(),
                                  grid.m_mapCandidates.end(),
                                  map.get_index()) != grid.m_mapCandidates.end() ||
                        std::find(grid.m_secondCandidates.begin(),
                                  grid.m_secondCandidates.end(),
                                  map.get_index()) != grid.m_secondCandidates.end())
                        found = true;
            }
                }
            }
        }
    }

        // ORIG 0x08362952/0x0836297f：第二段 dungeonList 的 begin/end 同在
        // 循环初始化处求值（end 缓存 -0x108），条件块只调 ne（P13）。
        for (std::list<int>::const_iterator dungeonIt = dungeonList.begin(),
                 dungeonEnd = dungeonList.end();
             dungeonIt != dungeonEnd; ++dungeonIt)
    {
            CDungeon* dungeon = find_dungeon(*dungeonIt);
            if (dungeon == 0)
                missingDungeon = true;
            // ORIG 0x083629c2：直读 m_eventMonsters(+0x690) 成员（内联 getEventMonsters）。
        if (dungeon->m_eventMonsters.empty() == false)
        {
            // ORIG 0x083629d4..0x083629d7：`call empty; test %al,%al`（jcc 被
            // jump-to-next 删）。空体 if 会被 4.4 连 test 一并 DCE（探针
            // /tmp/probe_dead4.cpp f15）；`while (cond) break;` 精确保留
            // call+test 且无 jcc（同探针 f17／P225 dead 变体）。
            while (map.m_rivalData2.empty())
                break;
        }
    }

        if (missingDungeon)
            return 0;

    // ORIG 0x08362a43..：读 pair first/second 负值检查 + first 与 0x64 比较，
    // 结果全部丢弃；保留读取不发明校验规则。
        for (std::vector<std::pair<int, int> >::const_iterator it =
                 map.m_entranceVec.begin(); it != map.m_entranceVec.end(); it++)
    {
            const std::pair<int, int>& e = *it;
        if (e.first >= 0 && e.second >= 0)
        {
            // ORIG 0x08362a59..0x08362a5e `mov (%eax),%eax; cmp $0x64,%eax`
            // （死比较、无分支）：非 volatile 读会被 4.4 tree-DCE（探针 f20），
            // volatile 读强制发射且逐条同形（探针 /tmp/probe_dead3.cpp f21）。
            if (*(volatile const int*)&e.first > 0x64)
            {
            }
        }
    }
    return 1;
}