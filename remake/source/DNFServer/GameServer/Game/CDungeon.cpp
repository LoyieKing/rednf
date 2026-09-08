#include "GameRand.h"
// df_game_r 还原 —— CDungeon / CDungeon_TowerOfDespair / STDungeonScript
//（G2 副本链）。
// 逐函数对照 docs/class_func_reports/CDungeon.md、CDungeon_TowerOfDespair.md
// 与 ORIG df_game_r 反汇编实现；目标 identical 口径。
// 支撑类型（MazeScript/stDungeonAssignItem_t/STWarRoomData/...）的 ORIG W 符号
// 也在本 TU 内实现。
#include "CDungeon.h"

#include <string.h>
#include <stdlib.h>

#include <algorithm>

#include "CMap.h"
#include "GameTypes.h"
#include "LogManager.h"
#include "DNFFunctionLib.h"
#include "CInventory.h"             // check_item_exist/GetInvenSlot/delete_item
#include "PacketGuard.h"            // 门票回执包
#include "GlobalData.h"             // GlobalData::s_systemTime_/s_statistic_proxy_mgr
#include "CServerProxyMgr.h"        // GetServerProxy 完整模板
#include "CStatisticServerProxy.h"  // 统计上报
#include "DataManagerInitDepends.h" // TOD_UserState/TOD_Layer（绝望之塔扩展数据）
#include "CBoosterGage.h"           // charac_expand::CData::alter（脏标记）

// ============================================================================
// 支撑类型：GridScript / RidableObject / RidableScript / DungeonClearCondition
// ============================================================================

// ============================================================================
// CDungeon 弱访问器 / 自包含逻辑（ORIG 地址见各函数注释；
// ctor/dtor/set_dungeon 及 GameStubs.cpp 已有桩的方法不在此重复定义）
// ============================================================================

int CDungeon::getDungeonKind() const
{
    return m_dungeonKind;
}

bool CDungeon::isNoFatigueDungeon() const
{
    return m_noFatigueFlag;
}

unsigned char CDungeon::get_blood_dungeon_type() const
{
    return m_bloodDungeonType;
}

unsigned char CDungeon::getQuestNpcDungeon() const
{
    return m_questNpcDungeon;
}

char CDungeon::get_limit_party_count() const
{
    return m_limitPartyCount;
}

char CDungeon::get_limit_Inout_count() const
{
    return m_limitInoutCount;
}

bool CDungeon::limitOfStackableItemInTower() const
{
    return m_limitStackableItemInTower;
}

int CDungeon::getHeroesRequiredQuest() const
{
    return m_heroesRequiredQuest;
}

int CDungeon::getAICharacterAppearRate() const
{
    return m_aiCharacterAppearRate;
}

int CDungeon::getRandomTowerMaxStage() const
{
    return m_randomTowerMaxStage;
}

int CDungeon::getDeathTowerMaxStage() const
{
    return m_deathTowerMaxStage;
}

int CDungeon::getDeathTowerMapIndex(unsigned int idx) const
{
    return m_deathTowerMapIndexes[idx];
}

int CDungeon::getTowerMaxClearRewardItemNum() const
{
    return m_towerMaxClearRewardItemNum;
}

int CDungeon::getEventMonsterTotalProb() const
{
    return m_eventMonsterTotalProb;
}

int CDungeon::getTryGenEventMonsterCnt() const
{
    return m_tryGenEventMonsterCnt;
}

int CDungeon::getTryGenEventMonsterProb() const
{
    return m_tryGenEventMonsterProb;
}

int CDungeon::getSecondEventMonsterTotalProb() const
{
    return m_secondEventMonsterTotalProb;
}

int CDungeon::getTryGenSecondEventMonsterCnt() const
{
    return m_tryGenSecondEventMonsterCnt;
}

int CDungeon::getTryGenSecondEventMonsterProb() const
{
    return m_tryGenSecondEventMonsterProb;
}

unsigned char CDungeon::hasTowerItemDrop() const
{
    return m_hasTowerItemDrop;
}

int CDungeon::GetDummyAppearCount() const
{
    return m_dummyAppearCount;
}

unsigned char CDungeon::IsHaveDummyMap() const
{
    return m_hasDummyMap;
}

bool CDungeon::isRiskDungeon() const
{
    return m_riskFlag;
}

bool CDungeon::IsOpenEvenEnemy() const
{
    return m_openEvenEnemy;
}

int CDungeon::GetPartyMemberCoinLimit() const
{
    return m_partyMemberCoinLimit;
}

float CDungeon::get_exp_weight() const
{
    return m_expWeight;
}
std::vector<std::pair<int, int> >* CDungeon::getDropItems() const
{
    return const_cast<std::vector<std::pair<int, int> >*>(&m_vec40);
}
std::vector<std::pair<int, int> >* CDungeon::getObjectDropItems() const
{
    return const_cast<std::vector<std::pair<int, int> >*>(&m_vec4c);
}

std::vector<stDungeonAssignItem_t>* CDungeon::getSpecialPassiveObjectItems()
    const
{
    return const_cast<std::vector<stDungeonAssignItem_t>*>(&m_vec58);
}

std::vector<stEventMonster_t>* CDungeon::getEventMonsters() const
{
    return const_cast<std::vector<stEventMonster_t>*>(&m_eventMonsters);
}

std::vector<stSecondEventMonster_t>* CDungeon::getSecondEventMonsters() const
{
    return const_cast<std::vector<stSecondEventMonster_t>*>(&m_secondEventMonsters);
}

std::map<int, std::vector<RandomList> >* CDungeon::getRandomTowerMapIndexes()
    const
{
    return const_cast<std::map<int, std::vector<RandomList> >*>(&m_map658);
}

unsigned char CDungeon::get_dimension_min_partymem() const
{
    return m_dimensionMinPartymem;
}

std::vector<MazeScript>* CDungeon::GetQuestMazeScriptVector(
    ENUM_MAZE_QUEST_TYPE mazeQuestType) const
{
    return const_cast<std::vector<MazeScript>*>(&m_mazeList0[mazeQuestType]);
}

// ---- 迁移自 GameStubs.cpp 的 CDungeon 桩方法（ORIG 弱符号，签名已核对）----
CDungeon::CDungeon()
    : m_typeInheritance(0),
      m_riskFlag(false),
      m_partyMemberCoinLimit(0),
      m_enterEachMap(false),
      m_openEvenEnemy(false)
{
    // ORIG 0x08373f3c：初始化列表按成员声明位置赋值（vtable 后立即 +0x04，
    // 0x6bc vector 构造后 0x6c9/0x6cc/0x6d0/0x6d1），其它 POD 不初始化。
}
CDungeon::~CDungeon()
{
    // ORIG 0x08376fd8：析构 std 容器成员（自动）。
}

int CDungeon::get_index() const
{
    // ORIG 0x080fdcf0：return m_index（+0x08）
    return m_index;
}

int CDungeon::get_min_level() const
{
    // ORIG 0x0814559a：return m_minLevel（+0x0c）
    return m_minLevel;
}

int CDungeon::get_standard_level() const
{
    // ORIG 0x080f9810：return m_standardLevel（+0x10）
    return m_standardLevel;
}

const char* CDungeon::GetDungeonName() const
{
    // ORIG 0x081455a6：return m_name.c_str()（m_name +0x18）
    return m_name.c_str();
}
unsigned char CDungeon::get_dimension_possible() const
{
    // ORIG 0x0826b938：return m_dimensionPossible（单 movzbl）
    return m_dimensionPossible;
}

bool CDungeon::isTowerOfDespairDungeon() const
{
    // ORIG 0x0822b4fa：return GetType_DungeonInHeritance() == 1。
    // [b28c-21] ORIG `sub $0x18` 保守帧；4.4 对「TU 内已见定义的 callee」精确
    // 继承 $0x4（P210 三态②）⇒ 把 GetType_DungeonInHeritance 定义移到本函数
    // 之后（本 TU 仅此处调用它），4.4 只见声明 ⇒ 恢复 $0x18（P210 三态③）。
    return GetType_DungeonInHeritance() == 1;
}

int CDungeon::GetType_DungeonInHeritance() const
{
    return m_typeInheritance;
}

bool CDungeon::isTournamentDungeon() const
{
    // ORIG 0x0836520a：m_tournamentFlag 非 0 且 (signed char)<=1
    //（1=普通锦标赛，2=特殊，负值视为普通）
    if (m_tournamentFlag != 0 && (char)m_tournamentFlag <= 1)
        return true;
    return false;
}

int CDungeon::getTournamentRoundFatigue() const
{
    // ORIG T 0x08365238：返回 m_tournamentRoundFatigue（+0x8a4）；0 兜底返回 1。
    if (m_tournamentRoundFatigue == 0)
        return 1;
    return m_tournamentRoundFatigue;
}

bool CDungeon::IsEnterEachMap() const
{
    // ORIG 0x08365278：return m_enterEachMap（movzbl 0x6d0）
    return m_enterEachMap;
}

// ORIG _ZN12CDataManager18set_dimensionInoutEic / _ZN12CDataManager21set_limit_inout_countEc15ENUM_BLOOD_TYPE
// 真实 C++ 成员实现（权威声明在 CDataManager.h），语义按 ORIG 反汇编：
//   set_dimensionInout（080eed82）：idx>5 或 idx<0 早退，再写数组
//   set_limit_inout_count（08374c82）：写 m_limitInoutCount[type]
void CDataManager::set_dimensionInout(int idx, char value)
{
    if (idx > 5)
        return;
    if (idx < 0)
        return;
    m_dimensionInout[idx] = value;
}

void CDataManager::set_limit_inout_count(char value, ENUM_BLOOD_TYPE type)
{
    m_limitInoutCount[type] = value;
}

void CDungeon::set_dungeon(STDungeonScript& script)
{
    // [b28d-39 判词 · N82/N89] ORIG 0x0834acc4：超大件帧/槽号整体平移（N82，
    // ORIG 具名临时 -0x88 vs 4.4 -0x8c）+ 容器访问器跨版本差（N89）；
    // 全字段赋值序与来源逐条对齐，语义等价。
    // ORIG 0x0834acc4：从脚本复制全部字段（顺序按 ORIG 汇编）
    m_index = script.m_18;
    m_standardLevel = script.m_ac;
    m_minLevel = script.m_78;
    m_expWeight = script.m_b0;

    char nameBuf[500] = {0};
    strcpy(nameBuf, script.m_str1c.c_str());
    if (!CodePage::script2Database((char*)script.m_str1c.c_str(), nameBuf))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x58a, 5)(
            "Error!!! %s->%s [%s][%s][%d]",
            CodePage::script(), CodePage::database(),
            script.m_str1c.c_str(), __PRETTY_FUNCTION__, 0x58a);
    }
    m_name = nameBuf;

    for (int i = 0; i < 5; ++i)
        m_6e0[i] = script.m_b4[i];

    m_heroesRequiredQuest = script.m_14;
    m_dimensionPossible = script.m_304;
    m_dimensionInoutValue = script.m_305;
    m_dimensionMinPartymem = script.m_306;
    if (m_dimensionPossible != 0)
    {
        G_CDataManager()->set_dimensionInout(
            (int)(char)m_dimensionPossible - 1, (char)m_dimensionInoutValue);
        G_CDataManager()->set_original_dimensionInout(
            (int)(char)m_dimensionPossible - 1, (char)m_dimensionInoutValue);
    }
    m_limitPartyCount = script.m_324;
    m_limitInoutCount = script.m_325;
    if (m_index == 0x2aff)
    {
        G_CDataManager()->set_limit_inout_count(
            (char)m_limitInoutCount, (ENUM_BLOOD_TYPE)2);
    }
    if (m_index == 0x2afe)
    {
        G_CDataManager()->set_limit_inout_count(
            (char)m_limitInoutCount, (ENUM_BLOOD_TYPE)1);
    }

    std::vector<MazeScript>::iterator mit = script.m_mazeList170.begin();
    std::vector<MazeScript>::iterator mitEnd = script.m_mazeList170.end();
    while (mit != mitEnd)
    {
        MazeScript* maze = &(*mit);
        int questType = maze->m_questType;
        if (questType == 0)
            m_mazeList0[0].push_back(*maze);
        else if (questType == 1)
            m_mazeList0[1].push_back(*maze);
        else if (questType == -1)
            m_mazeList.push_back(*maze);
        ++mit;
    }

    int totalRate = 0;
    m_clearItemMap.clear();
    std::vector<std::pair<int, int> >::const_iterator cit =
        script.m_vecc8.begin();
    std::pair<int, int> p;
    do
    {
        do
        {
            if (cit == script.m_vecc8.end())
                goto copy_rest;
            p = *cit;
        } while (p.first == 0);
        totalRate = p.second + totalRate;
        m_clearItemMap.insert(std::make_pair(totalRate, p.first));
        if (totalRate > 100)
        {
            LogManager::logFormat(
                1, "data_manager.cpp",
                "void CDungeon::set_dungeon(STDungeonScript&)", 0x5e1,
                "total_rate(%d) > 100", totalRate);
            goto copy_rest;
        }
        ++cit;
    } while (true);

copy_rest:
    m_vec40 = script.m_vecd4;          // 通关掉落
    m_vec4c = script.m_vece0;          // 对象掉落
    m_vec58 = script.m_vecec;          // 指定物品对象
    script.m_warRoom18c.copy(&m_warRoomData);
    m_entryRequiredItem = script.m_requiredItem7c;
    m_entryRestrictFlag = script.m_88;
    m_entryRequiredItem2 = script.m_requiredItem8c;
    m_vec86c = script.m_vec98;
    m_minAttackPartyMember = script.m_314;
    m_maxAttackPartyMember = script.m_315;
    m_maxRevengeCount = script.m_317;
    if (script.m_316)
        m_villageAttackEventFlag = 1;
    else
        m_villageAttackEventFlag = 0;
    m_87c = script.m_a4;
    m_bossRoom90c.m_0 = script.m_bossRoom328.m_0;
    m_bossRoom90c.m_vec4 = script.m_bossRoom328.m_vec4;
    std::copy(script.m_128, script.m_128 + 5, m_880);
    m_894 = script.m_13c;
    m_898 = script.m_168;
    m_89c = script.m_28c;
    m_89d = script.m_140;
    m_89e = script.m_28d;
    m_bloodDungeonType = script.m_28e;
    m_advanceAltarSurvivalFlag = script.m_2ab;
    m_advanceAltarType = script.m_2ac;
    memcpy(m_808, script.m_2b0, sizeof(m_808));
    memcpy(m_820, script.m_2bc, sizeof(m_820));
    m_vec814 = script.m_vec2c8;
    m_clearReward = script.m_clearReward2d4;
    m_survivalReward = script.m_survival2f8;
    m_hasDummyMap = 0;
    m_dummyAppearCount = script.m_16c;
    m_aiCharacterAppearRate = script.m_338;
    m_dungeonKind = script.m_358;
    m_deathTowerMaxStage = script.m_368;
    if (m_deathTowerMaxStage != 0)
    {
        m_deathTowerMapIndexes.reserve(m_deathTowerMaxStage + 1);
        for (unsigned int i = 0; i < script.m_vec35c.size(); ++i)
            m_deathTowerMapIndexes[script.m_vec35c.at(i).first] =
                script.m_vec35c.at(i).second;
    }
    m_randomTowerMaxStage = script.m_33c;
    if (m_randomTowerMaxStage != 0)
    {
        m_map658.clear();
        for (std::map<int, std::vector<RandomList> >::const_iterator rit =
                 script.m_map340.begin();
             rit != script.m_map340.end(); ++rit)
        {
            m_map658.insert(std::make_pair(rit->first, rit->second));
        }
    }
    m_8a0 = script.m_3c4;
    m_vec8f0 = script.m_vec3c8;
    m_tryGenEventMonsterCnt = script.m_3d4;
    m_tryGenEventMonsterProb = script.m_3d8;
    m_eventMonsterTotalProb = script.m_3dc;
    m_eventMonsters = script.m_vec3e0;
    m_tryGenSecondEventMonsterCnt = script.m_3ec;
    m_tryGenSecondEventMonsterProb = script.m_3f0;
    m_secondEventMonsterTotalProb = script.m_3f4;
    m_secondEventMonsters = script.m_vec3f8;
    m_limitStackableItemInTower = (script.m_380 == 1);
    m_hasTowerItemDrop = (script.m_3b4 == 1);
    m_towerMaxClearRewardItemNum = script.m_3b8;
    m_vec900 = script.m_vec404;
    m_noFatigueFlag = script.m_48c;
    if (script.m_2a9 != 0)
    {
        m_typeInheritance = 1;
    }
    m_questNpcDungeon = script.m_2aa;
    m_tournamentFlag = script.m_494;
    m_tournamentRoundFatigue = script.m_498;
    m_itemRate8c4 = script.m_itemRate4b4;
    m_map8ac = script.m_map49c;
    m_8d4 = script.m_4c4;
    m_map8d8 = script.m_map4c8;
    m_map91c = script.m_map4e4;
    m_dimensionPartyCount.swap(script.m_dimension308);
    m_riskFlag = script.m_4e0;
    m_partyMemberCoinLimit = script.m_4fc;
    m_enterEachMap = script.m_504;
    m_openEvenEnemy = script.m_505;
}

// ---- 绝望之塔（TOD）----
// ORIG：GetCharacExpandData(0xd) 返回 TOD_UserState（charac_expand 扩展数据，
// 权威布局见 DataManagerInitDepends.h；IncreaseEnterCount/getEnterCount/
// getTodayEnterLayer/TOD_Layer::GetLayer 均为 ORIG 真实符号）。

// ORIG T 0x0864388a（报告 TOD_UserState/IncreaseEnterCount.md）：enterCount(+0x10)
// 与 lastEnterLayer(+0x16) 各自 ++、lastEnterTime(+0x18)=getCurSec；当日层槽
// (+0x14) 按 ORIG 是内嵌 TOD_Layer——判 0 走真实 call TOD_Layer::GetLayer
// (W 0x0822ec40, this+0x14)，非字段直读。（Sec 批：TOD_UserState +0x8 段已内嵌
// SIG_REQUEST_TOD_USER_STATE，m_todayLayer 本体即 TOD_Layer，直调免 cast。）
// [b22判词·跨版本] 残余 1 insn：ORIG 4.1 对 ushort 回写生成 `add $0x1,%eax;
// mov %eax,%edx`，4.4 恒融合为 `lea 0x1(%eax),%edx`（三变体实测均 lea，见
// 范式库跨版本豁免表 movzwl 段）；`movl $0x0,(%esp)` 系 .o R_386_32 数据地址
// reloc 占位（§6.3），relink 后即 &GlobalData::s_systemTime_。语义等价。
// [b28c-lane17] fresh 复验确认：字段已 unsigned short（movzwl 宽度/偏移全对），
// 唯一残差即该 lea-vs-add+mov 折叠 ⇒ N18a（单段 regmove 不可造）封顶。
void TOD_UserState::IncreaseEnterCount()
{
    // [b28d-39 判词 · N4/P30] ORIG 0x0864388a：残余 `add $0x1,%eax; mov %eax,%edx`
    // vs 4.4 `lea 0x1(%eax),%edx`（add-vs-lea 固有差）；三次自增/时间戳/alter 调用
    // 逐条等价。
    m_data.m_enterCount = m_data.m_enterCount + 1;
    m_data.m_lastEnterLayer = m_data.m_lastEnterLayer + 1;
    m_data.m_lastEnterTime = GlobalData::s_systemTime_.getCurSec();
    if (m_data.m_todayLayer.GetLayer() == 0)
        m_data.m_firstEnterTime = GlobalData::s_systemTime_.getCurSec();
    ((charac_expand::CData*)this)->alter();
}

// 注：保持 POD（无 user ctor）——GCC 4.4 __attribute__((packed)) 对含非 POD
// 成员的外层 struct 不生效，会致 Key 偏移 4 对齐（0xc）而非 ORIG 0xa。
struct TowerOfDespairStatistic_Key
{
    unsigned int m_0;   // +0x00（packet+0xe，当日层写入口）
    unsigned int m_4;   // +0x04

    TowerOfDespairStatistic_Key();
};

TowerOfDespairStatistic_Key::TowerOfDespairStatistic_Key()
{
    m_0 = 0;
    m_4 = 0;
}

// Packet_TowerOfDespair_Statistic_GTS（ORIG ctor 0x08374cba W，总尺寸 0x17）：
// PacketHeader(0x271c,0x17)[10] + Key[8] + byte flag + uint accId。
// 注：用 #pragma pack（而非 __attribute__((packed))）——GCC 4.4 的 attribute
// packed 对含非 POD 成员（Key 有 user ctor）的外层 struct 不生效，会致 Key
// 偏移 4 对齐（0xc）而非 ORIG 0xa。
#pragma pack(push, 1)
struct Packet_TowerOfDespair_Statistic_GTS
{
    PacketHeader m_header;   // +0x00（packed 0xa 字节）
    TowerOfDespairStatistic_Key m_key;  // +0x0a
    unsigned char m_flag;    // +0x12
    unsigned int m_accId;    // +0x13

    Packet_TowerOfDespair_Statistic_GTS();
};
#pragma pack(pop)

Packet_TowerOfDespair_Statistic_GTS::Packet_TowerOfDespair_Statistic_GTS()
    : m_header(0x271c, 0x17)   // 成员序：header → key(默认构造) → flag/accId
{
    m_flag = 0;
    m_accId = 0;
}

CDungeon_TowerOfDespair::CDungeon_TowerOfDespair()
{
    // ORIG C1/C2 0x08377606：先构造基类 CDungeon，再覆写 vtable 为 0x8c349f8
    //（唯一虚槽 _do_after_dungeon_start 指向派生实现）；成员构造由编译器生成。
}
int CDungeon_TowerOfDespair::_do_after_dungeon_start(CUser* user) const
{
    // [b28d-39 判词 · N93/N82] ORIG 0x0834d8ec：具名 int 槽 -0x14 vs -0x18 方向
    // 相反（N93）+ 帧 0x28 族平移（N82）；调用点差集与发包/统计写入逐条对齐，
    // 语义等价。
    // ORIG 0x0834d8ec：进入绝望之塔——递增计数、二次进入扣门票、发包回执、统计。
    // 判例（ADeepY4[6]）：DIFF(N封)，gap +1（orig 254 / our 255）。全调用点成立
    // （GetCharacExpandData/IncreaseEnterCount/getEnterCount/check_item_exist/GetInvenSlot/
    // delete_item/cMyTrace×2/PacketGuard 族/getTodayEnterLayer/GetServerProxy/SendPacket）。
    // 残余=4.1.2 跨 call 值寄存器缓存（N82）：失败路径 ORIG 将 m_itemCount→%esi、getCurCharacNo
    // →%ebx 于 cMyTrace ctor 之前 calc（callee-saved 存活）、统计素 ORIG 令 &gts→%ebx 提前；
    // 4.4-O0 对同样本轮把它下栈槽（P197② 记 N）。语义与 Ghidra C 逐条对应。
    if (user == 0)
        return 0;

    TOD_UserState* todState =
        (TOD_UserState*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd);
    todState->IncreaseEnterCount();
    if (todState->getEnterCount() > 1)
    {
        if (m_entryRequiredItem.m_enableFlag != 0)
        {
        // 二次及以上进入需持有门票物品（ORIG this+0x7f4/+0x7f8/+0x7fc）。
        int slot = user->getCurCharacInvenR()->check_item_exist(
            m_entryRequiredItem.m_itemIdx);
        if (slot == -1)
            return 0;
        Inven_Item item =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slot);
        if (item.m_addInfo2 < m_entryRequiredItem.m_itemCount)
            return 0;

        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, m_entryRequiredItem.m_itemCount,
                (eItemDelReason)0xe, true))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x96f, 5);
            trace("CDungeon_TowerOfDespair::_do_after_dungeon_start, delete_item failed , User ch =%d , %d %d",
                  user->getCurCharacNo(), slot, m_entryRequiredItem.m_itemCount);
            return 0;
        }

        PacketGuard pg;
        pg.put_header(0, 0xe);
        pg.put_byte(0);
        pg.put_short(1);
        pg.put_short(slot);
        int rest = item.m_addInfo2 - m_entryRequiredItem.m_itemCount;
        if (rest == 0)
            pg.put_int(-1);
        else
            pg.put_int(item.m_addInfo);   // ORIG local_8b（item idx 字段）
        pg.put_int(rest);
        pg.put_byte(item.GetItemAttr());
        pg.put_short(item.m_fieldb);
        pg.put_byte(item.m_field0);
        pg.put_int(item.m_fieldd);
        pg.put_byte(item.m_amp.getAbilityType());
        pg.put_short(item.m_amp.getAbilityValue());
        pg.put_byte(0);
        pg.put_packet(item);
        pg.finalize(true);
        user->Send(pg);
        user->SaveInventory();
        }
    }

    Packet_TowerOfDespair_Statistic_GTS gts;
    TOD_Layer todayLayer = todState->getTodayEnterLayer();
    gts.m_key.m_0 = todayLayer.GetLayer();
    gts.m_flag = 1;
    gts.m_accId = user->get_acc_id();

    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    proxy->SendPacket((char*)&gts, sizeof(Packet_TowerOfDespair_Statistic_GTS));
    return 1;
}
int CDungeon::_do_after_dungeon_start(CUser* user) const
{
    return 1;
}

int CDungeon::GetMaze(int idx) const
{
    if ((unsigned int)idx >= m_mazeList.size())
        return 0;
    return (int)&m_mazeList[idx];
}

void CDungeon::GetRandMaze(int& out) const
{
	out = rand() % m_mazeList.size();
	(void)m_mazeList[out];
}

int CDungeon::GetRandMazebyPartyCount(int partyCount) const
{
    // [b28d-39 判词 · N82] ORIG 0x0834d06c：帧 0x40 vs 0x30 + 除法 lowering
    // （ORIG `mov %eax,-0x2c; divl -0x2c` vs 4.4 寄存器 div）；候选收集/随机
    // 语义逐条等价。
	// ORIG 0x0834d06c：唯一迷宫直返；否则按 [m_partyCountMin, m_partyCountMax]
	// 收集候选；候选唯一/随机取，空则全表随机。
	if (m_mazeList.size() == 1)
		return (int)&m_mazeList[0];

	MazeScript* local_1c;
	std::vector<MazeScript*> candidates;
	unsigned int r;
	unsigned int i;

	i = 0;
	while (m_mazeList.size() > i)
	{
		MazeScript* elem = const_cast<MazeScript*>(&m_mazeList[i]);
		if (elem->m_partyCountMin <= partyCount &&
			partyCount <= elem->m_partyCountMax)
		{
			local_1c = elem;
			candidates.push_back(local_1c);
		}
		i = i + 1;
	}

	r = 0;
	if (candidates.empty())
	{
		r = rand() % m_mazeList.size();
		return (int)&m_mazeList[r];
	}
	if (candidates.size() == 1)
		return (int)candidates[0];
	r = rand() % candidates.size();
	return (int)candidates[r];
}

int CDungeon::GetStartMap(int direction) const
{
    // ORIG 0x0834c75a：方向守卫共享一个 -1 返回块；空表 log 后 -1；
    // 随机前进 it++（后置）逐槽检查；返回 it->first。
    if (direction > 0xf || direction < 0)
        return -1;

    if (m_maplist[direction].size() == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp", __PRETTY_FUNCTION__,
            0x79a,
            "CDungeon::GetStartMap() : direction(%d), dungeon index(%d), "
            "start_maplist_[direction].size() == 0, May be Script Error!",
            direction, m_index);
        return -1;
    }

    int r = get_rand_int(m_maplist[direction].size());
    __gnu_cxx::hash_map<int, CMap*>::const_iterator it =
        m_maplist[direction].begin();
    for (int i = 0; i < r; i++)
    {
        it++;
        if (it == m_maplist[direction].end())
            return -1;
    }
    return it->first;
}

int CDungeon::get_clear_item() const
{
	// ORIG 0x0834c8c0：ret 先置栈、迭代器默认构造、lower_bound 到临时再赋值
	int ret = 0;
	int key;
	std::map<int, int>::const_iterator it;
	key = get_rand_int(100) + 1;
	it = m_clearItemMap.lower_bound(key);
	if (it == m_clearItemMap.end())
		ret = -2;
	else
		ret = it->second;
	return ret;
}

int CDungeon::get_clear_item(int level) const
{
	// ORIG 0x0834c95c：同 get_clear_item()，key = level + 1
	int ret = 0;
	int key;
	std::map<int, int>::const_iterator it;
	key = level + 1;
	it = m_clearItemMap.lower_bound(key);
	if (it == m_clearItemMap.end())
		ret = -2;
	else
		ret = it->second;
	return ret;
}

int CDungeon::getLimitCoinDiff(int diff, int& out) const
{
    // ORIG 0x08365316：size==0 → 0；命中取 value.m_0 写 out 返回 1；未命中返回 0
    if (m_map91c.size() == 0)
        return 0;
    std::map<int, UseCoinDungeonDiff>::const_iterator it = m_map91c.find(diff);
    if (it != m_map91c.end())
    {
        out = it->second.m_0;
        return 1;
    }
    return 0;
}

bool CDungeon::checkFreeRevivalCondition(int diff) const
{
    // ORIG 0x08365298：it != end 且 !(value.m_4)（xor $1 取反测试）→ true
    std::map<int, UseCoinDungeonDiff>::const_iterator it = m_map91c.find(diff);
    return it != m_map91c.end() && !it->second.m_4;
}

bool CDungeon::get_dimension_member_count(DimensionPartyCount& out) const
{
    // ORIG 0x0834d220：std::find 匹配 m_0/m_1，命中复制 m_2 返回 true
    std::vector<DimensionPartyCount>::const_iterator it = std::find(
        m_dimensionPartyCount.begin(), m_dimensionPartyCount.end(), out);
    if (it != m_dimensionPartyCount.end())
    {
        out.m_2 = it->m_2;
        return true;
    }
    return false;
}

void GridScript::clear()
{
    m_gridValue = 0;
    m_mapCandidates.clear();
    m_secondCandidates.clear();
    m_layeredMapIndexes.clear();
}

GridScript::GridScript()
{
    clear();
}

RidableObject::RidableObject()
{
    clear();
}

RidableObject::~RidableObject()
{
}

void RidableScript::clear()
{
    m_0 = 0;
    m_4 = 0;
    m_objects.clear();
}

RidableScript::RidableScript()
{
    clear();
}

RidableScript::~RidableScript()
{
}

DungeonClearCondition::DungeonClearCondition()
{
    clear();
}

DungeonClearCondition::~DungeonClearCondition()
{
}

void DungeonClearCondition::clear()
{
    // 定义置于 ctor 之后：4.4 对"被调函数定义未前置"的 ctor 帧发 sub $0x18，
    // 与 ORIG 0x088f29f4 帧形一致（探针 /tmp/near7/p7-p8）。
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
}

// ============================================================================
// MazeScript（0x74 字节）
// ============================================================================
void MazeScript::clear()
{
    // ORIG 0x88f2a76：宽高置 1；三组 vector clear；0x2c..0x54 置 -1；
    // 0x64..0x6c 置 0；0x70 字节清零；ridable clear。
    // ORIG 0x088f2a85 段：宽高置 1 是 `for (i=0;i<=1;++i) arr[i]=1` 索引循环
    // （`movl $0x1,(%eax,%edx,4)` + setle 回边），两条独立直写复现不了 ⇒
    // 经匿名 union 的数组视图 m_dims 写。
    for (int i = 0; i <= 1; ++i)
        m_dims[i] = 1;
    m_grids.clear();
    m_vec14.clear();
    m_vec20.clear();
    m_2c = -1;
    m_30 = -1;
    m_34 = -1;
    m_questType = -1;
    m_50 = -1;
    m_54 = -1;
    m_clearCondition.clear();
    m_64 = 0;
    m_partyCountMin = 0;
    m_partyCountMax = 0;
    m_70 = 0;
    m_ridable.clear();
}

MazeScript::MazeScript()
{
    clear();
}

MazeScript::~MazeScript()
{
    // ORIG D1/D2 0x08376f00（合成形态）：逆序析构 m_clearCondition(+0x58)、
    // m_ridable(+0x38)、m_vec20/m_vec14/m_grids；由编译器按成员声明生成。
}

GridScript* MazeScript::getGrid(int x, int y)
{
    // ORIG 0x88f2c2a：返回 &m_grids[m_width*y+x]（vector::operator[] 结果）
    return &m_grids[m_width * y + x];
}

// ============================================================================
// stDungeonAssignItem_t / RandomList / 事件怪结构
// ============================================================================

void stDungeonAssignItem_t::clear()
{
    m_assignItemIdx = -1;
    m_list.clear();
}

stDungeonAssignItem_t::stDungeonAssignItem_t()
{
    clear();
}

stDungeonAssignItem_t::~stDungeonAssignItem_t()
{
}

RandomList::~RandomList()
{
}

stEventMonster_t::~stEventMonster_t()
{
}

stSecondEventMonster_t::~stSecondEventMonster_t()
{
}

// ============================================================================
// STMonsterTypeSpawnProb / STDeadTowerData / STWarRoomData / RequiredItem
// ============================================================================

// ORIG W 0x088f2c64（报告缺失兜底：nm 定址+objdump 切片；清单给
// docs/class_func_reports/CCharacter/clear.md 系撞名报告，签名为
// _ZN10CCharacter5clearEv，已按 §6.3 弃用）：clear 整体 memset(this,0,0x10)。
// [b22判词·跨版本] 残余 1 序差：4.4 填 memset 出参槽先于取 this，4.1 先生成
// `mov 0x8(%ebp),%eax`（指令集相同仅两 mov 对调，三变体实测 4.4 无法复现前序）；
// call 目标 reloc 即 memset（§6.3 占位假阳性）。语义等价。
void STMonsterTypeSpawnProb::clear()
{
    // P68 正向：ORIG 0x88f2c6a 实参序「this 先物化→0x10→0x0」＝ 带 cast 形。
    memset((char*)this, 0, sizeof(STMonsterTypeSpawnProb));
}

STMonsterTypeSpawnProb::STMonsterTypeSpawnProb()
{
    clear();
}

void STDeadTowerData::clear()
{
    m_vec.clear();
}

STDeadTowerData::STDeadTowerData()
{
    clear();
}

STDeadTowerData::~STDeadTowerData()
{
    // ORIG D1/D2 0x0836b41c（合成形态）：仅析构 m_vec（+0x00）。
}

STWarRoomData::STWarRoomData()
{
    // ORIG C1/C2 0x0836a1f6：构造 m_vec14[4]（+0x14）/m_vec44（+0x44）/
    // m_vec6c[4]（+0x6c）/m_spawnProb（+0xb0）后调用 clear()。
    clear();
}

void STWarRoomData::clear()
{
    // ORIG 0x836a398：两个 vector<int>[4] 数组循环 + 标量区。
    memset(&m_0, 0, 0xc);
    m_c = 0;
    m_10 = -1;
    for (int i = 0; i < 4; ++i)
        m_vec14[i].clear();
    m_vec44.clear();
    m_50 = 0;
    m_54 = 0;
    m_58 = 0;
    memset(m_5c, 0, sizeof(m_5c));
    for (int i = 0; i < 4; ++i)
        m_vec6c[i].clear();
    memset(m_9c, 0, sizeof(m_9c));
    m_ac = 0;
    m_spawnProb.clear();
    m_bc = 0;
    m_c0 = 0;
    memset(m_c4, 0, sizeof(m_c4));
    m_d4 = 0;
    memset(m_dc, 0, sizeof(m_dc));
    m_d8 = 0;
    m_ec = 0;
    m_f0 = 0;
    m_f4 = 0;
    m_f8 = 0;
    m_fc = 0;
}

void STWarRoomData::copy(STWarRoomData* other)
{
    // [b28d-39 判词 · N84/N90] ORIG 0x0836a578：结构整体赋值/memcpy 的 rep movsl
    // 分块启发式跨版本差（4.1 分块重载 src/dst，4.4 单次连发，N84/N90）；
    // 逐字段复制集合与顺序一致，语义等价。
    // ORIG 0x836a578：数组按元素 reserve/push_back，标量逐个/memcpy。
    memcpy(other, this, 0xc);
    other->m_c = m_c;
    other->m_10 = m_10;
    for (int i = 0; i < 4; ++i)
    {
        other->m_vec14[i].reserve(m_vec14[i].size());
        for (size_t j = 0; j < m_vec14[i].size(); ++j)
            other->m_vec14[i].push_back(m_vec14[i][j]);
    }
    other->m_vec44.reserve(m_vec44.size());
    for (size_t i = 0; i < m_vec44.size(); ++i)
        other->m_vec44.push_back(m_vec44[i]);
    other->m_50 = m_50;
    other->m_54 = m_54;
    other->m_58 = m_58;
    memcpy(other->m_5c, m_5c, sizeof(m_5c));
    for (int i = 0; i < 4; ++i)
    {
        other->m_vec6c[i].reserve(m_vec6c[i].size());
        for (size_t j = 0; j < m_vec6c[i].size(); ++j)
            other->m_vec6c[i].push_back(m_vec6c[i][j]);
    }
    memcpy(other->m_9c, m_9c, sizeof(m_9c));
    other->m_ac = m_ac;
    other->m_spawnProb.reserve(m_spawnProb.size());
    for (size_t i = 0; i < m_spawnProb.size(); ++i)
        other->m_spawnProb.push_back(m_spawnProb[i]);
    other->m_bc = m_bc;
    other->m_c0 = m_c0;
    memcpy(other->m_c4, m_c4, sizeof(m_c4));
    other->m_d4 = m_d4;
    other->m_d8 = m_d8;
    memcpy(other->m_dc, m_dc, sizeof(m_dc));
    other->m_ec = m_ec;
    other->m_f0 = m_f0;
    other->m_f4 = m_f4;
    other->m_f8 = m_f8;
    other->m_fc = m_fc;
}

STWarRoomData::~STWarRoomData()
{
}

RequiredItem::RequiredItem()
{
    clear();
}

// clear 定义移到 ctor 之后（NL7 实证：callee 后置时 4.4 unit-at-a-time 不塌缩
// ctor outgoing，保持 ORIG sub $0x18）。
void RequiredItem::clear()
{
    m_itemIdx = -1;
    m_itemCount = 0;
    m_enableFlag = 0;
}
void advancealtar::ClearRewardData::clear()
{
    // ORIG：vector<_RewardItem> m_vec[3] 数组循环 clear（i=0..2）。
    for (int i = 0; i < 3; ++i)
        m_vec[i].clear();
}

advancealtar::ClearRewardData::ClearRewardData()
{
    // ORIG 0x8810d5e：仅 3 个 vector 默认构造，无 clear() 调用。
}

advancealtar::ClearRewardData::~ClearRewardData()
{
}

advancealtar::ClearRewardData& advancealtar::ClearRewardData::operator=(
    const ClearRewardData& other)
{
    // [b28d-39 判词 · N18a] ORIG 0x08376c9c：语义已按 ORIG 修正（--n 前置测试只拷
    // 前 2 个元素，P171 照抄）；残余仅 `mov 0x8(%ebp),%eax; mov %eax,%esi`
    // 被 4.4 折成单条 `mov 0x8(%ebp),%esi`（N18a 单段 regmove 不可造）。
    // [b28d-39] ORIG 0x08810d8a：指针步进 do-while（esi=dst/ebx=src 各 +0xc、
    // edi=2 递减到 -1、`setne/test/jne`）——索引形在 4.4 生 add/add/shl 地址链，DIFF。
    register std::vector<advancealtar::_RewardItem>* dst = m_vec;
    register const std::vector<advancealtar::_RewardItem>* src = other.m_vec;
    register int n = 2;
    // ORIG 0x08810d8a 为 `--n != -1` 前置测试 while：n=2 只拷贝 2 个元素（P171 照抄）。
    while (--n != -1)
    {
        *dst = *src;
        ++dst;
        ++src;
    }
    return *this;
}

advancealtar::SurvivalRewardData::SurvivalRewardData()
{
}

advancealtar::SurvivalRewardData::~SurvivalRewardData()
{
}

advancealtar::SurvivalRewardData&
advancealtar::SurvivalRewardData::operator=(const SurvivalRewardData& other)
{
    m_vec = other.m_vec;
    return *this;
}

void TournamentRewardItemRate::Clear()
{
    m_c = 0;
    m_items.clear();
}

TournamentRewardItemRate::TournamentRewardItemRate()
{
    Clear();
}

TournamentRewardItemRate::~TournamentRewardItemRate()
{
}

TournamentRewardItemRate& TournamentRewardItemRate::operator=(
    const TournamentRewardItemRate& other)
{
    m_items = other.m_items;
    m_c = other.m_c;
    return *this;
}

stConditionEnterBossRoom_t::stConditionEnterBossRoom_t()
{
    // ORIG 0x836b552：仅 vector<int> 构造（m_0 不初始化）。
}

stConditionEnterBossRoom_t::~stConditionEnterBossRoom_t()
{
}

void stConditionEnterBossRoom_t::clear()
{
    m_0 = 0;
    m_vec4.clear();
}

RecoveryStairInfo::RecoveryStairInfo()
{
}

RecoveryStairInfo::~RecoveryStairInfo()
{
    // ORIG D1/D2 0x083dc7d2（合成形态）：仅析构 m_vec（+0x0c）。
}

void STTowerDialog::clear()
{
    // ORIG W 0x088f2d38：m_index=0、m_name.clear()、三个 flag 字节清零。
    m_index = 0;
    m_name.clear();
    m_flag8 = 0;
    m_flag9 = 0;
    m_flagA = 0;
}

STTowerDialog::STTowerDialog()
{
    clear();
}

STTowerDialog::~STTowerDialog()
{
}

STTowerDialog& STTowerDialog::operator=(const STTowerDialog& other)
{
    m_index = other.m_index;
    m_name = other.m_name;
    m_flag8 = other.m_flag8;
    m_flag9 = other.m_flag9;
    m_flagA = other.m_flagA;
    return *this;
}

void STTowerFullDialog::clear()
{
    m_vec0.clear();
    m_vecc.clear();
    m_vec18.clear();
}

STTowerFullDialog::STTowerFullDialog()
{
    clear();
}

STTowerFullDialog::~STTowerFullDialog()
{
}

// ---- ORIG W 弱符号发射段（头内联定义，本 TU 无 odr-use 故未发射）----
//   TournamentRewardItem  C1/C2 @0x088f2dcc（三字段清零）、D1/D2 @0x088f9ef6（空体）
//   STNamedMonsterMapPos  C1/C2 @0x088f2d6c（m_y=0; m_x=m_y）、D1/D2 @0x088f8cbc（空体）
// 真实构造/析构强制发射，不引入导出符号。
namespace
{
void emitDungeonRewardWeakSymbols()
{
    TournamentRewardItem item;      // C1/C2
    item.~TournamentRewardItem();   // D1/D2
    STNamedMonsterMapPos pos;       // C1/C2
    pos.~STNamedMonsterMapPos();    // D1/D2
}
}

