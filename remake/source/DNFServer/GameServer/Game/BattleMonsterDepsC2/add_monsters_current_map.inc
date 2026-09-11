// ORIG 0x0830162e：把 mobList 按 spawnType 展开生成到当前图。
// 命名怪条件：m_pDungeon->m_vec8f0 非空且 _mapMonster.m_champion 时查已清场表跳过。
// [SweepBattleMonster 2026-09-09] 已修（399 vs 398，clean 397=397，DIFF→near）：①P167 end()
// 每轮回边重求值（不缓存 itEnd）；②P214① 正极性 `m_villageMonsterId!=0 && m_party!=NULL`
// 包裹队伍分支、普通分支内 `ref.m_flag04!=0` 在前；③真 bug：flag==0 分支等级来源应为
// ref+0x8（m_field08，ORIG 0x08301893 `mov 0x8(%eax),%eax`），旧写 m_levelAdd(+0x10)；
// ④P226 语句表达式复现 `!empty() && champion` 物化岛；⑤P324 case 升序(0,1,2)；⑥P225
// `(signed char)dim>0`+if/else 复现 setg；⑦__PRETTY_FUNCTION__；⑧P231 register 复用 uid。
// 残差仅 uid 值落 %ebx vs ORIG %eax（N7 寄存器角色，指令数/序列全等）。
// [P8S6 2026-09-11] P139 修复：ORIG prett 串（0x08c23b60 size 0x85）=
// `void CBattle_Field::add_monsters_current_map(MOB_REF_LIST&, std::vector<int, ...>&, ...)`
// ⇒ 第 1 参按 ORIG 拼写 MOB_REF_LIST&（statically identical to std::list<_mapMonster>，
// mangled 不变、零 ABI/零 codegen）。typedef 落本 include 位置（全局作用域 ⇒ prett 打裸名）。
typedef std::list<_mapMonster> MOB_REF_LIST;
void CBattle_Field::add_monsters_current_map(MOB_REF_LIST& mobList,
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
                                             std::vector<int>& championList,
                                             map_monster& mob, int& count, int& uid,
                                             int& mapCount)
{
    if (m_pDungeon == NULL)
        return;
    int spawnCnt = 0;                          // 跨条目保留（ORIG local_18 初值 0）
    // ORIG local_18(-0x14) = GetCurrentMapInfo() 一次缓存（Ghidra `local_18 = (MapInfo *)GetCurrentMapInfo(this)`），
    // 其后 m_layeredMap/m_mapIndex/Add_Mob/m_dummyBossMap 全走该指针（p167 EXTRA GetCurrentMapInfo×3）。
    MapInfo* mapInfo = GetCurrentMapInfo();
    // [b28d-D9] P167：ORIG 用 `_List_const_iterator` + 后置 `operator++(int)`；end() 在
    // 循环尾每轮重新求值（`lea -0x370; call end; C1ERK` 位于 +0x500 回边处）⇒ 不缓存 itEnd。
    for (std::list<_mapMonster>::const_iterator it = mobList.begin();
         it != mobList.end();)
    {
        const _mapMonster& ref = *it;
        // P226 条件向：ORIG 前两条件先物化 `mov $1/$0; test %al,%al; je`（无槽）⇒
        // 语句表达式包住 `!empty() && champion`，checkCleared 作为嵌套内层条件。
        if (({ !m_pDungeon->m_vec8f0.empty() && ref.m_champion; }))
        {
            if (checkClearedNamedMonsterMap(static_cast<unsigned char>(ref.m_posX),
                                            ref.m_mobId))
            {
                it++;                // 已清场命名怪：不生成不计数（仅推进迭代器）
                continue;
            }
        }

        switch (ref.m_spawnType)
        {
        case 0:
            spawnCnt = 1;
            break;
        case 1:
            if (m_field198 > 2)
                spawnCnt = 2;
            else
                spawnCnt = 1;
            break;                                                     // +0x198
        case 2:
            spawnCnt = m_field198;
            break;
        default:                   // 其余类型沿用上一条目数量（ORIG 不落任何赋值）
            break;
        }

        for (int s = 0; s < spawnCnt; ++s)
        {
            mob.m_mobId = ref.m_orderIdx;                              // ref+0x2c → mob+0x00
            // P214①/P133：ORIG 两条 je 同落普通分支（+0x1b6）⇒ 正极性 `!=0 && !=NULL`
            // 包裹队伍分支；普通分支内 flag!=0 代码在前（ORIG +0x1c1），flag==0 在后
            // （+0x235），且 flag==0 分支等级来源 = ref+0x8（Ghidra local_14[2]）。
            if (m_villageMonsterId != 0 && m_party != NULL)            // +0x128
            {
                mob.m_level = static_cast<char>(m_party->getMemberMaxLevel());
                if (mob.m_level == 0)
                    mob.m_level = 1;
                if (static_cast<unsigned char>(mob.m_level) > 200)
                    mob.m_level = 200;
            }
            else
            {
                if (ref.m_flag04 != 0)
                {
                    // P225：ORIG `test %al,%al; setg %al; test %al,%al; je`（有符号
                    // `>0` 物化）⇒ 显式 (signed char) 比较 + if/else（三目被折成统一写）。
                    if (static_cast<signed char>(
                            m_pDungeon->get_dimension_possible()) > 0)
                        mob.m_level = static_cast<char>(m_standardDimensionLevel);
                    else
                        mob.m_level = static_cast<char>(
                            m_pDungeon->get_standard_level() +
                            static_cast<char>(ref.m_field08));
                    if (mob.m_level == 0)
                        mob.m_level = 1;
                    if (static_cast<unsigned char>(mob.m_level) > 200)
                        mob.m_level = 200;
                }
                else
                {
                    if (static_cast<signed char>(
                            m_pDungeon->get_dimension_possible()) > 0)
                        mob.m_level = static_cast<char>(m_standardDimensionLevel);
                    else
                        mob.m_level = static_cast<char>(ref.m_field08); // +0x238 低字节
                }
            }

            if (ref.m_mobId == -1)
            {
                mob.m_mobIndex =
                    G_CDataManager()->m_monsterManager->get_levelrandom_mob(
                        static_cast<int>(static_cast<unsigned char>(mob.m_level)));
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    __PRETTY_FUNCTION__,
                    0x5c0,
                    "HEROES_LOG : RANDOM MONSTER INDEX (%d) MONSTER LEVEL (%d) MAP "
                    "INDEX (%d) [Map Kind : %s]\n",
                    mob.m_mobIndex,
                    static_cast<unsigned int>(
                        static_cast<unsigned char>(mob.m_level)),
                    mapInfo->m_mapIndex,
                    mapInfo->m_layeredMap ? "layeredMap" : "generalMap");
            }
            else
            {
                mob.m_mobIndex = ref.m_mobId;
            }

            // mob+0x11(低字节)←ref.m_levelAdd、+0x12(高字节)←ref.m_field14（ORIG 逐字节写）
            mob.m_field11b.m_field11b0 = static_cast<unsigned char>(ref.m_levelAdd);
            mob.m_field11b.m_field11b1 = static_cast<unsigned char>(ref.m_field14);
            mob.m_roleType = static_cast<char>(ref.m_roleType);        // mob+0x08 ← ref+0x18
            // P231：ORIG 读 uid 入 %eax 后跨 store 复用（`lea 0x1(%eax),%edx`）；裸
            // 单条后置自增（一个求值点）⇒ 4.4 只读一次 uid 并跨 store 保活；
            // 具名 curUid 局部令 4.4 落 %ebx，两句写法则因引用别名重读 uid（均反劣）。
            mob.m_instId = uid++;
            mob.m_field20 = 0;
            mob.m_field24 = 0;

            if (mob.m_roleType == 0 && !mob.isNamedMonster() &&
                !m_pDungeon->isTournamentDungeon())
            {
                championList.push_back(mob.m_instId);
            }

            mob.m_field13 = ref.m_champion;                            // mob+0x13 ← ref+0x1c
            mob.m_field14 = static_cast<char>(ref.m_posX);             // mob+0x14 ← ref+0x20
            mob.m_mobList = ref.m_mobList;                             // mob+0x328 ← ref+0x34
            mob.m_dropCnt = ref.m_hpPct;                               // mob+0x2c ← ref+0x28
            mob.m_flag34 = static_cast<char>(ref.m_flag30);            // mob+0x34 ← ref+0x30

            mapInfo->Add_Mob(mob);
            if (mob.m_field13 == 0)
                ++count;
            if (mob.m_roleType == 3 && !mapInfo->m_dummyBossMap)
            {
                m_apcList.push_back(mob.m_instId);                     // +0x118
            }
        }
        it++;
        ++mapCount;
    }
}