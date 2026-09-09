// BattleMonsterTailC3A.cpp — BattleMonsterDepsC2 剩余依赖第一组（5 符号，ORIG nm 全部 T 实证）。
// 依据 docs/class_func_reports 逐函数汇编+反编译 C 转写；字段化布局（无裸偏移/asm 桥）。
//
//   _ZN13CBattle_Field17ConsistTournamentEPK4CMapRSt4listI11_mapMonsterSaIS4_EERSt3mapIs...
//       CBattle_Field::ConsistTournament @ 0x083035c8
//   _ZN13CBattle_Field27checkClearedNamedMonsterMapEhi
//       CBattle_Field::checkClearedNamedMonsterMap @ 0x08300c76
//   _ZN12CDataManager28get_hellparty_groupOrder_mapEi
//       CDataManager::get_hellparty_groupOrder_map @ 0x0836379a
//   _ZN13CBattle_Field22add_hell_party_monsterER11map_monsterRiS2_PSt3mapIt21STHellPartyGroupOrder...
//       CBattle_Field::add_hell_party_monster @ 0x083071ba
//   _ZN13CBattle_Field20getAddChampionMobCntEv
//       CBattle_Field::getAddChampionMobCnt @ 0x083010ee
//
// 依赖（均由已登记 TU 提供）：MakeBasicRoundInfo/ChoiceFirstGroup/ConsistTournamentRound
// （TournamentTailC3C.cpp）、TournamentInfo::operator<（CBattle_Field.cpp）、
// map_monster 生命周期/isNamedMonster 与 MapInfo::Add_Mob（WarField.cpp/MapInfo.cpp）、
// CDungeon::get_standard_level、CDataManager::find_hellparty_group/get_hellparty_script_values、
// Get_instance_groupOrder_map_pool（定义见 TournamentTailC3C.cpp）。

#include <list>
// ============================================================================
// b28d-52 / SweepBattleMonsterC3A 判词表（fresh check_tu_routed，四数
// 100/155/0/4/57 → 101/156/1/3/55；gap = ORIG−OUR）：
//   get_hellparty_groupOrder_map 0x0836379a —— gap 0（identical/ae）
//   getAddChampionMobCnt        0x083010ee —— gap 0（identical/ae）
//   ConsistTournament           0x083035c8 —— gap 0，NEAR：P264（.o 数据立即数/重定位
//     显示差）+ N82 槽号；calls 30=30、callee 差集仅 C1/C2 同址别名（P93）。
//   add_hell_party_monster      0x083071ba —— gap −1，DIFF：calls 41=41、差集全为
//     C1/C2·D1/D2 同址别名；残余 1 条 = script_values 段 4.4 把返回值 `mov %eax,%edx`
//     中转（ORIG 4.1 用 %edx 装 this）⇒ N7 寄存器角色封顶。
//   checkClearedNamedMonsterMap 0x08300c76 —— gap −2，DIFF：calls 7/9，差集仅
//     `_Unwind_Resume`+1（4.4 为局部 map_monster 布 EH landing，ORIG 无 ⇒ N91 族）
//     与 C1/C2 别名；另 char m_visited 判零 4.4 多 sete（P73 已知，字段 bool 化会
//     破坏 set_visited_map 的 AE 写入形，见 BattleMonsterDepsC2.cpp:1083 判词）。
// ============================================================================
#include <map>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "DataManagerInitDepends.h"   // stHellParty_t/stHellPartyMonsterGroupList（须在 CDataManager.h 之后）
#include "HellPartyGroupOrder.h"

// ==================================================================
// CBattle_Field::ConsistTournament @ 0x083035c8
// 报告：docs/class_func_reports/CBattle_Field/ConsistTournament.md
// [SweepBattleMonsterC3A] DIFF→NEAR（gap 209/209=0；calls 30=30，差集仅 C1/C2 同址
// 别名）：①P227 错误分支改 cMyTrace 全表达式临时链（ORIG 0x083036e9 在 ctor 前
// `call get_member_count; mov %eax,%ebx` 跨 ctor 保活）；②P198① MakeBasicRoundInfo
// 结果用具名 bool 局部（ORIG 0x0830377c `mov %al,-0x9; movzbl -0x9,%eax; xor $1`）。
// 残差 = P264（.o 数据立即数/重定位显示差）+ N82 槽号。
// ==================================================================
bool CBattle_Field::ConsistTournament(const CMap* pMap, std::list<_mapMonster>& mobList,
                                      std::map<short, std::vector<TournamentInfo> >& roundMap)
{
    if (pMap == NULL)
    {
        cMyTrace trace("bool CBattle_Field::ConsistTournament(const CMap*, "
                       "MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
                       0xb5a, 5);
        trace("CBattle_Field::ConsistTournament ERROR!! : pMap is NULL pointer");
        return false;
    }

    // CMap+0x114：m_tournamentMap（map<char, vector<TournamentMonster>>）。
    if (pMap->m_tournamentMap.empty())
    {
        cMyTrace trace("bool CBattle_Field::ConsistTournament(const CMap*, "
                       "MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
                       0xb61, 5);
        trace("CBattle_Field::ConsistTournament ERROR!! : tournamentMonster is empty");
        return false;
    }

    std::vector<TournamentMonster> monsters;
    const int memberCount = m_party->get_member_count();

    std::map<char, std::vector<TournamentMonster> >::const_iterator group =
        pMap->m_tournamentMap.find(static_cast<char>(memberCount));
    if (group == pMap->m_tournamentMap.end())
    {
        // ORIG 0x083036e9 `call get_member_count; mov %eax,%ebx` 在 cMyTrace ctor 之前
        // 求值并以 %ebx 跨 ctor 保活（无具名槽）⇒ 全表达式临时链（P227）。
        cMyTrace("bool CBattle_Field::ConsistTournament(const CMap*, "
                 "MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
                 0xb6c, 5)
            ("CBattle_Field::ConsistTournament ERROR!! : party member count is invalid(%d)",
             m_party->get_member_count());
        return false;
    }

    // ORIG 0x08303734：monsters = group->second（map 中该人数对应的怪物列表拷贝）。
    monsters = group->second;

    std::vector<TournamentInfo> roundInfo;
    // ORIG 0x0830377c `mov %al,-0x9(%ebp); movzbl -0x9(%ebp),%eax; xor $1` ⇒ 具名
    // bool 局部（P198①），勿直写 if(!call())。
    const bool basicOk = MakeBasicRoundInfo(memberCount, monsters, roundInfo);
    if (!basicOk)
        return false;

    std::sort(roundInfo.begin(), roundInfo.end());

    std::vector<TournamentInfo> firstGroup;
    ChoiceFirstGroup(roundInfo, firstGroup);
    ConsistTournamentRound(firstGroup, roundInfo, roundMap, mobList);
    return true;
}

// ==================================================================
// CBattle_Field::checkClearedNamedMonsterMap @ 0x08300c76
// 报告：docs/class_func_reports/CBattle_Field/checkClearedNamedMonsterMap.md
// [SweepBattleMonsterC3A] 符号线索闭环：ORIG 0x08300cee/0x08300d15 调 **const** 版
// `_ZNKSt6vectorI20STNamedMonsterMapPosSaIS0_EEixEj` @0x0830fec0（全库无**非** const
// 版定义；直接写 `[idx]` 会误选非 const 版 ⇒ 我方缺失该 ORIG 符号）⇒ 源码用 const
// 引用视图 cast（P323），
// p167 MISSING `…ixEj`×2 / EXTRA 非 const×2 已归零。残差 gap −2：`_Unwind_Resume`+1
// （N91 族，4.4 为局部 map_monster 布 landing，ORIG 无）+ char m_visited 判零 sete（P73）。
// ==================================================================
bool CBattle_Field::checkClearedNamedMonsterMap(unsigned char idx, int uid)
{
    if (m_pDungeon == NULL)                       // +0x188
        return false;

    // CDungeon+0x8f0：m_vec8f0（vector<STNamedMonsterMapPos>{int m_x; int m_y;}）。
    // ORIG 对 size()/operator[] 按操作数独立求值（x/y 各一次索引）。
    if (static_cast<unsigned int>(idx) >= m_pDungeon->m_vec8f0.size())
        return false;

    // ORIG 0x08300cee/0x08300d15 两次调 const 版 operator[]
    // （_ZNKSt6vectorI20STNamedMonsterMapPosSaIS0_EEixEj @0x0830fec0；非 const 版
    // 全库无定义）⇒ 接收者须为 const 左值：显式 const 引用视图 cast（P323），
    // 不引入具名局部槽。
    typedef std::vector<STNamedMonsterMapPos> NamedMonsterMapPosVec;
    // ORIG +0x0c6 直接 `movzbl 0x1(%eax),%eax; test; je`（无 info 具名局部槽，P0）。
    if (GetMapInfoFromPos(
            static_cast<const NamedMonsterMapPosVec&>(m_pDungeon->m_vec8f0)[idx].m_x,
            static_cast<const NamedMonsterMapPosVec&>(m_pDungeon->m_vec8f0)[idx].m_y,
            false)->m_visited == 0)              // MapInfo+0x01（无空检查，忠实转写）
        return false;

    map_monster mob;                              // 默认构造（ORIG 081511c8）
    mob.m_mobIndex = uid;                         // +0x0c ← 参数 2
    // ORIG +0x0f3 段：`test %al,%al; je; mov $1,%ebx; mov $0,%esi; jmp; mov $1,%esi`
    // （esi 路径标志跨析构，P56 if 形）。
    if (mob.isNamedMonster())                      // W 08151182（析构后返回）
        return true;
    return false;
}

// ==================================================================
// CDataManager::get_hellparty_groupOrder_map @ 0x0836379a
// 报告：docs/class_func_reports/CDataManager/get_hellparty_groupOrder_map.md
// 返回 map<int, HellPartyGroupOrderMap> 中 key 对应内层 map 地址；未命中返回 NULL。
// ==================================================================
void* CDataManager::get_hellparty_groupOrder_map(int idx)
{
    HellPartyGroupOrderMapPool* pool = Get_instance_groupOrder_map_pool();
    // ORIG：it 先默认构造（C1Ev），find 按值返回到临时槽再赋值（4.1 无 copy-elision）；
    // 判定用 operator!=，命中分支 fall-through，NULL 块在尾部。
    HellPartyGroupOrderMapPool::iterator it;
    it = pool->find(idx);
    if (it != pool->end())
        return &it->second;                       // pair<const int, ...>+4
    return NULL;
}

// ==================================================================
// CBattle_Field::add_hell_party_monster @ 0x083071ba
// 报告：docs/class_func_reports/CBattle_Field/add_hell_party_monster.md
// [SweepBattleMonsterC3A] 符号线索闭环：ORIG 增量段调 **后置** `…ppEi` @0x08311c40
// （P28 增量侧推论）⇒ 两处 `rit++`；我方 .o 现已发射 ppEi（前置 ppEv 消失）。
// 其余实修（全部有 ORIG 地址直证）：入口 4 具名局部 0/0/1/0（0x083071c5..da）；
// rit=begin() 提前到 totalRate 之前（0x08307239）；totalRate 为 int 槽（0x0830727f
// `movzwl %ax,%eax; mov %eax,-0x18`）；i=0 在 find 之前（0x0830731e）；m_hellPartyGroupIdx
// 与 oit->first 同型化（0x083073ca 单段）；m_roleType 反极性 if/else（0x0830726c jne）；
// m_field11b 双字节视图 + Add_Mob 匿名临时（0x083074b4 单次 C2ERKS_）。
// 残差 gap −1 = script_values 段 4.4 `mov %eax,%edx` 中转（N7）。
// ==================================================================
void CBattle_Field::add_hell_party_monster(map_monster& mob, int& uid, int& seq,
                                           std::map<unsigned short, STHellPartyGroupOrder>* groups)
{
    // ORIG 入口 4 条具名局部初始化（0x083071c5..0x083071da：-0x24/-0x20/-0x1c/-0x18
    // = 0/0/1/0）。-0x24 为命中率累加器（成功插入后与 -0x20 成对清零，跨外层迭代
    // 保留）；-0x20/-0x1c 在 ORIG 全函数内只写不读（P171 死写入照抄，标 [推断]）；
    // -0x18 是 totalRate 的 int 槽（先置 0、循环内赋 u16 零扩展值）。
    int accumulatedRate = 0;
    int accumulatedTotalRate = 0;   // [推断] ORIG -0x20（仅入口/成功路径清零）
    int rateRollState = 1;          // [推断] ORIG -0x1c（仅入口置 1）
    int totalRate = 0;              // ORIG -0x18（int；非 unsigned short）

    GetCurrentMapInfo()->m_hellPartyGroup.clear();   // MapInfo+0x8c

    for (std::map<unsigned short, STHellPartyGroupOrder>::iterator oit = groups->begin();
         oit != groups->end(); ++oit)
    {
        std::list<STHellPartyGroupRate> rates(oit->second.m_rates);

        // ORIG 0x08307239 先求 rit=rates.begin()（sret -0x3ac），随后才取权重/掷点。
        std::list<STHellPartyGroupRate>::iterator rit = rates.begin();

        // 按当前地狱派对难度取该组总权重（u16 零扩展）后掷点。ORIG +0x0a3 段：
        // `movzwl 0x4(%eax,%edx,2)`（pair.second 的 +0x04 = m_rateByDifficulty[diff]）
        // → `movzwl %ax,%eax; mov %eax,-0x18`（int 槽，两次零扩展为 4.1 固有形）。
        totalRate = oit->second.m_rateByDifficulty[m_hellPartyDifficulty];
        const int threshold = get_rand_int(totalRate);                  // -0x14
        while (rit != rates.end())
        {
            if (rit->m_difficulty != m_hellPartyDifficulty)
            {
                rit++;                                   // 难度不匹配：仅推进
                continue;
            }

            accumulatedRate += rit->m_rate;
            if (accumulatedRate < threshold)
            {
                rit++;
                continue;
            }

            // find_hellparty_group 返回 stHellPartyMonsterGroupList 视图：
            // int m_monsterIdx[16]@+0x00（>0 结尾）、unsigned char m_rate[16]@+0x40。
            int i = 0;                               // ORIG +0x164：i=0 在 find 之前发射
            stHellPartyMonsterGroupList* group =
                reinterpret_cast<stHellPartyMonsterGroupList*>(
                    G_CDataManager()->find_hellparty_group(rit->m_groupIdx));
            if (group == NULL)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    "void CBattle_Field::add_hell_party_monster(map_monster&, int&, int&, "
                    "MAP_HELL_PARTY_GROUP_ORDER*)",
                    0x1076,
                    "HELLPARTY_LOG : HELLPARTY GROUP INDEX (%d) , MAP INDEX (%d) : "
                    "ADD HELL PARTY GROUP ERROR!!\n",
                    rit->m_groupIdx, GetCurrentMapInfo()->m_mapIndex);
                break;                                   // 清理副本 → 推进外层迭代
            }

            for (; group->m_monsterIdx[i] > 0; ++i)
            {
                // ORIG +0x1f0 形：seq 值读入 %eax → 写 mob → lea 0x1 → 写 seq（P231
                // 裸 register 提示免槽保活，非 asm 绑定，P0 合规）。
                register int curMobId = seq;
                mob.m_mobId = curMobId;                  // +0x00 ← seq
                seq = curMobId + 1;
                mob.m_hellPartyGroupIdx = oit->first;    // +0x334 ← 组序表键（隐式 u16→s16）
                mob.m_hellPartyGroupOrder =
                    static_cast<short>(rit->m_groupIdx); // +0x336 ← 组索引
                mob.m_level = static_cast<char>(
                    m_pDungeon->get_standard_level());   // +0x10 byte ← 副本标准等级
                mob.m_mobIndex = group->m_monsterIdx[i]; // +0x0c
                // ORIG +0x26c `test; jne <movb $5 块>`（0 块直落）⇒ 反极性写法。
                if (group->m_rate[i] == 0)
                    mob.m_roleType = 0;                  // +0x08 byte
                else
                    mob.m_roleType = 5;

                register int curInstId = uid;            // ORIG +0x280 同形
                mob.m_instId = curInstId;                // +0x04 ← uid
                uid = curInstId + 1;
                mob.m_dropCnt = 100;                     // +0x2c ← 100（ORIG movl $0x64）
                mob.m_field13 = 1;                       // +0x13 byte
                mob.m_field14 = 0;                       // +0x14 byte

                // +0x11/+0x12 为逐字节写：脚本 tag 字节与常量 1。
                mob.m_field11b.m_field11b0 =
                    static_cast<stHellParty_t*>(
                        G_CDataManager()->get_hellparty_script_values())
                        ->m_field6c[m_hellPartyDifficulty];   // +0x6c[diff]
                mob.m_field11b.m_field11b1 = 1;

                mob.m_hellPartyScriptIdx = m_hellPartyDifficulty;     // +0x338 byte

                // ORIG 08151aea 只发一次拷贝构造（C2ERKS_），实参直传临时 ⇒ 匿名
                // 临时形（P0/P278），勿用具名 copy（会多一次按值传参拷贝 + 析构）。
                GetCurrentMapInfo()->Add_Mob(map_monster(mob));
            }

            GetCurrentMapInfo()->m_hellPartyGroup.insert(   // MapInfo+0x8c
                std::make_pair(rit->m_groupIdx, i - 1));

            accumulatedRate = 0;                         // 仅成功路径清零（-0x24/-0x20 成对）
            accumulatedTotalRate = 0;
            break;
        }
        // rates 副本析构（RAII）→ 推进外层迭代（ORIG 尾部 ++oit）。
    }
}

// ==================================================================
// CBattle_Field::getAddChampionMobCnt @ 0x083010ee
// 报告：docs/class_func_reports/CBattle_Field/getAddChampionMobCnt.md
// ==================================================================
int CBattle_Field::getAddChampionMobCnt()
{
    if (m_pMap == NULL)                              // +0x108
        return -1;

    // CMap+0x80：基础冠军怪数量。
    int championCnt = m_pMap->m_80;

    if (m_mapWidth == 0 && m_mapHeight == 0)         // +0x18c/+0x190
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__, 0x4ed,
            "[FPE] [CBattle_Field::ConsistMap] Dungeon[%d] Width[%d] Height[%d]",
            m_dungeonIndex, m_mapWidth, m_mapHeight);
    }

    // +0x70：SelectDungeon 自 CDungeon::m_6e0[diff] 复制的每图怪物基数；
    // 按地下城难度缩放（150%/250%/500%）。
    int baseCnt = m_mapMobCount;
    switch (get_dungeon_diff())
    {
    case 0:
        baseCnt = m_mapMobCount;
        break;
    case 1:
        baseCnt = m_mapMobCount * 150 / 100;
        break;
    case 2:
        baseCnt = m_mapMobCount * 250 / 100;
        break;
    case 3:
        baseCnt = m_mapMobCount * 500 / 100;
        break;
    case 4:
        baseCnt = m_mapMobCount;
        break;
    default:                                         // >4 保持切换前读数
        break;
    }

    // CMap+0x12c 非 1 时按密度掷点追加一只。
    // ORIG 0x08301xx 序：先测 m_12c（xor$1/test/je），后算 denser
    // （0x18c Width 先载 ecx、0x190 Height 后载 ebx、setg 物化）——嵌套 if 形。
    // N96 清绑：原 `register bool denser asm("al")` 已删。ORIG 的 denser 无槽
    // （cmp;setg %al;test %al,%al;je）⇒ 按 P226扩展/P316① 用语句表达式包住整个条件
    // （内不含具名变量）复现零槽短路物化。清绑前后 127=127、与 ORIG 逐条全等
    // （仅 .o 重定位地址差）⇒ 残差 0，无需 N96+N18a 封顶；具名 bool 局部形为 128（+1 槽物化）。
    if (m_pMap->m_12c == 0)
    {
        if (({ baseCnt * 100 / (m_mapWidth * m_mapHeight) > get_rand_int(100); }))
            championCnt += 1;
    }
    return championCnt;
}
