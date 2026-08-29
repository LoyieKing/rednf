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
#include <map>
#include <vector>

#include "CBattle_Field.h"
#include "DataManagerInitDepends.h"   // stHellParty_t/stHellPartyMonsterGroupList（须在 CDataManager.h 之后）
#include "HellPartyGroupOrder.h"

// ==================================================================
// CBattle_Field::ConsistTournament @ 0x083035c8
// 报告：docs/class_func_reports/CBattle_Field/ConsistTournament.md
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
        // ORIG 错误分支在构造 cMyTrace 前重新调用 get_member_count() 取日志参数。
        const int invalidCount = m_party->get_member_count();
        cMyTrace trace("bool CBattle_Field::ConsistTournament(const CMap*, "
                       "MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
                       0xb6c, 5);
        trace("CBattle_Field::ConsistTournament ERROR!! : party member count is invalid(%d)",
              invalidCount);
        return false;
    }

    std::vector<TournamentInfo> roundInfo;
    if (!MakeBasicRoundInfo(memberCount, monsters, roundInfo))
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
// ==================================================================
bool CBattle_Field::checkClearedNamedMonsterMap(unsigned char idx, int uid)
{
    if (m_pDungeon == NULL)                       // +0x188
        return false;

    // CDungeon+0x8f0：m_vec8f0（vector<STNamedMonsterMapPos>{int m_x; int m_y;}）。
    // ORIG 对 size()/operator[] 按操作数独立求值（x/y 各一次索引）。
    if (static_cast<unsigned int>(idx) >= m_pDungeon->m_vec8f0.size())
        return false;

    MapInfo* info = GetMapInfoFromPos(m_pDungeon->m_vec8f0[idx].m_x,
                                      m_pDungeon->m_vec8f0[idx].m_y, false);
    if (info->m_visited == 0)                     // MapInfo+0x01（无空检查，忠实转写）
        return false;

    map_monster mob;                              // 默认构造（ORIG 081511c8）
    mob.m_mobIndex = uid;                         // +0x0c ← 参数 2
    return mob.isNamedMonster();                   // W 08151182（析构后返回）
}

// ==================================================================
// CDataManager::get_hellparty_groupOrder_map @ 0x0836379a
// 报告：docs/class_func_reports/CDataManager/get_hellparty_groupOrder_map.md
// 返回 map<int, HellPartyGroupOrderMap> 中 key 对应内层 map 地址；未命中返回 NULL。
// ==================================================================
void* CDataManager::get_hellparty_groupOrder_map(int idx)
{
    HellPartyGroupOrderMapPool* pool = Get_instance_groupOrder_map_pool();
    HellPartyGroupOrderMapPool::iterator it = pool->find(idx);
    if (it == pool->end())
        return NULL;
    return &it->second;                           // pair<const int, ...>+4
}

// ==================================================================
// CBattle_Field::add_hell_party_monster @ 0x083071ba
// 报告：docs/class_func_reports/CBattle_Field/add_hell_party_monster.md
// ==================================================================
void CBattle_Field::add_hell_party_monster(map_monster& mob, int& uid, int& seq,
                                           std::map<unsigned short, STHellPartyGroupOrder>* groups)
{
    // 帧内 -0x24：命中率累加器为函数级变量，仅在成功插入后清零（跨外层迭代保留，
    // 忠实转写）；-0x20/-0x1c/-0x18 的 0/1/0 初值为局部临时构造残留，无可观测读取。
    int accumulatedRate = 0;

    GetCurrentMapInfo()->m_hellPartyGroup.clear();   // MapInfo+0x8c

    for (std::map<unsigned short, STHellPartyGroupOrder>::iterator oit = groups->begin();
         oit != groups->end(); ++oit)
    {
        std::list<STHellPartyGroupRate> rates(oit->second.m_rates);

        // 按当前地狱派对难度取该组总权重（u16 零扩展）后掷点。
        const unsigned short totalRate =
            oit->second.m_rateByDifficulty[m_hellPartyDifficulty];      // +0x00[diff]
        const int threshold = get_rand_int(totalRate);                  // -0x14

        std::list<STHellPartyGroupRate>::iterator rit = rates.begin();
        while (rit != rates.end())
        {
            if (rit->m_difficulty != m_hellPartyDifficulty)
            {
                ++rit;                                   // 难度不匹配：仅推进
                continue;
            }

            accumulatedRate += rit->m_rate;
            if (accumulatedRate < threshold)
            {
                ++rit;
                continue;
            }

            // find_hellparty_group 返回 stHellPartyMonsterGroupList 视图：
            // int m_monsterIdx[16]@+0x00（>0 结尾）、unsigned char m_rate[16]@+0x40。
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

            int i = 0;
            for (; group->m_monsterIdx[i] > 0; ++i)
            {
                const int nextMobId = seq;
                mob.m_mobId = nextMobId;                 // +0x00 ← seq++
                seq = nextMobId + 1;
                mob.m_hellPartyGroupIdx =
                    static_cast<short>(oit->first);      // +0x334 ← 组序表键
                mob.m_hellPartyGroupOrder =
                    static_cast<short>(rit->m_groupIdx); // +0x336 ← 组索引
                mob.m_level = static_cast<char>(
                    m_pDungeon->get_standard_level());   // +0x10 byte ← 副本标准等级
                mob.m_mobIndex = group->m_monsterIdx[i]; // +0x0c
                mob.m_roleType =
                    (group->m_rate[i] != 0) ? 5 : 0;     // +0x08 byte

                const int instId = uid;
                mob.m_instId = instId;                   // +0x04 ← uid++
                uid = instId + 1;
                mob.m_dropCnt = 100;                     // +0x2c ← 100（ORIG movl $0x64）
                mob.m_field13 = 1;                       // +0x13 byte
                mob.m_field14 = 0;                       // +0x14 byte

                // +0x11/+0x12 为逐字节写：脚本 tag 字节与常量 1。
                stHellParty_t* scriptValues = static_cast<stHellParty_t*>(
                    G_CDataManager()->get_hellparty_script_values());
                unsigned char* field11Bytes =
                    reinterpret_cast<unsigned char*>(&mob.m_field11);
                field11Bytes[0] =
                    scriptValues->m_field6c[m_hellPartyDifficulty];   // +0x6c[diff]
                field11Bytes[1] = 1;

                mob.m_hellPartyScriptIdx = m_hellPartyDifficulty;     // +0x338 byte

                map_monster copy(mob);                   // ORIG 08151aea 拷贝构造
                GetCurrentMapInfo()->Add_Mob(copy);
            }

            GetCurrentMapInfo()->m_hellPartyGroup.insert(   // MapInfo+0x8c
                std::make_pair(rit->m_groupIdx, i - 1));

            accumulatedRate = 0;                         // 仅成功路径清零
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

    // CMap+0x12c 非 1 时按密度掷点追加一只（ORIG movzbl/xor $1/test 的布尔判反形态）。
    if (!m_pMap->m_12c &&
        (baseCnt * 100 / (m_mapHeight * m_mapWidth) > get_rand_int(100)))
    {
        championCnt += 1;
    }
    return championCnt;
}
