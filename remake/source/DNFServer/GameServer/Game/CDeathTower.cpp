#include "GameRand.h"
// df_game_r 还原 —— WongWork::CDeathTower（死亡之塔）
// 基于 ORIG 反汇编逐函数实现语义。
#include "CGameManager.h"
#include "CUser.h"
#include "CParty.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "CDungeon.h"
#include "TimerQueue.h"
#include "LogManager.h"
#include "CInventory.h"
#include "CCirculationStatistic.h"
#include "CDataManager.h"
#include "CMap.h"
#include "CBattle_Field.h"    // handleStartGame 读取内嵌 BF 随机 buff 类型
#include "GameWorld.h"
#include "CBossStage.h"
#include "CDungeonEntranceLog.h"
#include "CDeathTowerRanking.h"
#include "CMonster.h"
#include "CItemGenerator.h"    // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CDeathTowerWinPointEvent.h"
#include "Secu_HackLogCheckByParty.h"   // MSG_MONSTER_DIE 完整定义
#include "CStatisticServerProxy.h"    // statistc_proxy::add（handlePickupItem 随机选项统计）
#include "CMonitorServerProxy.h"      // updateTowerRank（_beginTowerClearProcess）
#include "GlobalData.h"               // s_monitor_proxy_mgr
#include "CServerProxyMgr.h"          // GetServerProxy 完整定义
#include "MonsterIndependentDrop.h"        // CAICharacter 完整镜像（destroy 需 delete）

// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（CParty.h 以
// m_battleField[0x158] 等 pad 占位、未引入 CBattle_Field 类型；
// 与 QuickParty.cpp/CParty.cpp 口径一致）。布局：pad 0xb24 + BF 0xcd0。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};

// ---- 死亡塔统计/记录接口（定义见 Store.cpp / OnlyOrigBatch14.cpp / W8LT_DBTask.cpp）----
enum DEATHTOWER_VALUE_STATISTIC_FIELD
{
    DEATHTOWER_VALUE_STATISTIC_FIELD_0 = 0,
    DEATHTOWER_VALUE_STATISTIC_FIELD_1 = 1,
    DEATHTOWER_VALUE_STATISTIC_FIELD_9 = 9
};
class CDeathTowerValueStatistic
{
public:
    // ORIG 0x0860f342 返回 bool（PRETTY_FUNCTION "bool ..."，true/false 分支）
    bool SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field,
                                      char a, char b, unsigned int c);
};
extern CDeathTowerValueStatistic* GetInstanceDeathTowerValueStatistic();
#include "CDeathTowerPlayDataStatistic.h"          // SendDeathTowerPlayInfo 完整类型
extern CDeathTowerPlayDataStatistic* GetInstanceDeathTowerPlaydataStatistic();
class DB_UpdateDeathTowerBestRecord
{
public:
    static void makeRequest(unsigned int characNo, unsigned int accId,
                            unsigned int memberCount,
                            const stDeathTowerRecordMemberInfo_t* members,
                            unsigned int stage, unsigned int playTime);
};

// ---- CAICharacter / CAICharacterList ----
// 布局依据 ORIG set 0x0834a066 / ctor 0x08373a9a / 各访问器：
//   +0x00 int  idx             （getIdx 0x08150850：mov 0(%eax),eax）
//   +0x04 int  level           （getLevel 0x0815085a：mov 4(%eax),eax）
//   +0x08 int  m_field8        （set 写 script+0xd80）
//   +0x0c int  appearancePoint （getAppearancePoint 0x08150866：mov c(%eax),eax）
//   +0x10 vector<pair<int,int> > m_dropItems （deathTowerSpecifyItemDrop 遍历 +0x10）
//   +0x1c int  m_pvpWinLow     （getPvPWinPoint 0x0834a240 读取 +0x1c）
//   +0x20 int  m_pvpWinHigh    （getPvPWinPoint 读取 +0x20）
//   +0x24 vector<stMonsterIndependentDrop_t>
//   +0x30 stEnamyDropAvatarInfo_t
//   +0x6c std::string m_name
//   +0x70 unsigned char m_rarity
#ifndef DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED
#define DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED
class CAICharacter
{
public:
    int getIdx() const { return m_idx; }
    int getLevel() const { return m_level; }
    int getAppearancePoint() const { return m_appearancePoint; }
    // ORIG 0x0834a19c _ZNK12CAICharacter25deathTowerSpecifyItemDropERSt6vectorImSaImEE：
    // 遍历 m_dropItems（vector<pair<int,int>>），对每项 r=get_rand_int(10000)；
    // 若 pair.second > r 则把 pair.first（itemId）push_back 到 dropIds。
    void deathTowerSpecifyItemDrop(std::vector<unsigned long>& dropIds) const
    {
        for (std::vector<std::pair<int, int> >::const_iterator it =
                 m_dropItems.begin();
             it != m_dropItems.end(); ++it)
        {
            int r = get_rand_int(10000);
            if (it->second > r)
                dropIds.push_back((unsigned long)it->first);
        }
    }
    // ORIG 0x0834a240 _ZNK12CAICharacter14getPvPWinPointEv：
    // 返回 PvP 胜点。+0x1c=最小胜点，+0x20=最大胜点；等则返回该值，
    // 否则返回 min + get_rand_int(max - min + 1)。
    int getPvPWinPoint() const
    {
        if (m_pvpWinLow == m_pvpWinHigh)
            return m_pvpWinLow;
        return m_pvpWinLow + get_rand_int(m_pvpWinHigh - m_pvpWinLow + 1);
    }

private:
    int m_idx;                                      // +0x00
    int m_level;                                    // +0x04
    int m_field8;                                   // +0x08
    int m_appearancePoint;                          // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;  // +0x10
    int m_pvpWinLow;                                // +0x1c
    int m_pvpWinHigh;                               // +0x20
    char m_pad24[0x70 - 0x24];                      // +0x24..+0x6f（独立掉/头像/名字区，本 TU 未用）
};
#endif  // DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED

CAICharacter* CAICharacterList::get(unsigned int idx)
{
    // [b28d-D13] ORIG 0x08373d2c 额外 `lea -0x18;lea -0x1c;call` 段（迭代器
    // 转换/拷贝构造）⇒ 试 const_iterator（P96 判据两形互试）。
    std::map<int, CAICharacter*>::const_iterator it = m_charMap.find((int)idx);
    // [b28d-D13] ORIG 0x08373d5a 尾块 `mov $0` 岛在最后 ⇒ 正极性 if（P220/P47）。
    if (it != m_charMap.end())
        return it->second;
    return 0;
}
// ORIG dtor 0x08373d9e：destroy() 后按逆序析构各子对象（成员析构由编译器生成）。
CAICharacterList::~CAICharacterList() { destroy(); }

// ORIG destroy 0x0834a59a：遍历 m_charMap，对每个 CAICharacter 调析构并 delete。
void CAICharacterList::destroy()
{
    // [b28d-D13] ORIG 0x0834a59a：begin→-0x10、end→-0x14 各一次（P13 缓存 end），
    // p@-0xc 后仅 delete 自带守卫（P141，无显式 if(p!=0) 二次判断）。
    std::map<int, CAICharacter*>::iterator it = m_charMap.begin();
    std::map<int, CAICharacter*>::iterator e = m_charMap.end();
    for (; it != e; ++it)
    {
        // ORIG 此处调用 CAICharacter::~CAICharacter()（0x08373b32 W）后
        // operator delete；CAICharacter 在本工程尚未建模（仅前向声明），
        // 其析构当前无可执行语义，先直接释放【推断】。
        // ORIG 0x08373d60 单次 operator-> + `mov %eax,-0xc; mov -0xc,%ebx`
        // （具名局部缓存）。
        // [b28d-solo-CDeathTower] p167：ORIG 调 _Rb_tree_iterator::operator*（非 ->）。
        CAICharacter* p = (*it).second;
        delete p;                // 镜像类成员（vector/string）随析构释放
    }
    // [b28d-D13] ORIG 0x0834a59a 无 clear 调用（P171：ORIG 不写即删；map 由
    // ~CAICharacterList 的隐式成员析构清理）。
}

// ---- stMapMonsterKillChecker_t / map_item / map_monster ----
// 权威定义见 WarField.h（经 CGameManager.h → PvP_Room.h → PvP_deps.h 链包含），
// 本地重复定义已删除（§7 单声明点）。方法定义保留在下方（归属 WarField.h 声明）。

unsigned int stMapMonsterKillChecker_t::getUseSkillMaterialTime() { return m_useSkillMaterialTime; }

int stMapMonsterKillChecker_t::dieMonster(unsigned int tick, unsigned int limit)
{
    // ORIG 0x830e74e..0x830e78a 逐条核对：m_lastMobDieTime=tick 仅在超限
    // 重置分支内（0x830e77d），cnt++ 分支 fall 后直 return；<= 极性使 4.4
    // 复现 cnt++ fall / 重置块后置的块序。
    if (tick - m_lastMobDieTime <= limit)
    {
        m_uncontinuallyMobDieCnt++;
    }
    else
    {
        m_uncontinuallyMobDieCnt = 0;
        m_lastMobDieTime = tick;
    }
    return m_uncontinuallyMobDieCnt;
}

void stMapMonsterKillChecker_t::initMonsterInfo()
{
    // ORIG 0x830e71c：仅清零 +0x08
    m_uncontinuallyMobDieCnt = 0;
}

class TimerQueue;
TimerQueue* G_TimerQueue();

// CDungeonEntranceLog（CDungeonEntranceLog.h 权威声明）/ WongWork::CDeathTowerRanking
// （CDeathTowerRanking.h 权威声明）直接调用。

namespace WongWork {

// 具名字段访问依据（ORIG 报告 WongWork__CBossTower/CBossTower.md @0x81429b4 /
// ~CBossTower.md @0x8142a74）：
//   +0xaf0 = CBossTower::m_idx（movl 清 0）
//   +8     = CBossPlay::m_field8（movl 清 0）
//   +0xd   = CBossPlay::m_memberAlive[4]（ORIG memset(0xd,0,4) → 保持 memset 形态）
//   +0x9f0 = CBossStage::m_dungeon（ORIG memset(0x9f0,0,4) → 保持 memset 形态）
//   +4     = CBossPlay::m_party（movl 清 0）
//   +0xb00 = CBossTower::m_dungeonLog（CBossDungeonEntranceLog，0x18，编译器构造/析构）
// 继承：CBossTower : CBossStage : CBossPlay（隐式基类构造/析构，编译器处理 vptr）。
CBossTower::CBossTower()
{
    CBossStage::reset();
    m_idx = 0;
    m_field8 = 0;
    memset(m_memberAlive, 0, 4);
    m_dungeon = 0;   // ORIG 0x081429b4 证据 movl $0x0,0x9f0(%eax)：单条 dword 直存，非 memset call
    m_party = 0;
    // m_dungeonLog（CBossDungeonEntranceLog）由编译器在进入函数体前按权威
    // ctor（ORIG 0x081456c2：map C1 + reset）构造，此处不再 memset。
}

CBossTower::~CBossTower()
{
    CBossStage::reset();
    m_idx = 0;
    m_field8 = 0;
    memset(m_memberAlive, 0, 4);
    m_dungeon = 0;   // ORIG 0x08142a74 证据 movl $0x0,0x9f0(%eax)：单条 dword 直存
    m_party = 0;
    // m_dungeonLog 由编译器在函数体结束后按权威 dtor（ORIG 0x08145704：
    // reset + map D1）析构，此处不再 memset。
}


// CDeathTowerRanking 方法实现（权威声明见 CDeathTowerRanking.h；本 TU 为唯一定义点，
// CUser.cpp 的本地同名类/桩已删除）。表对象即本类布局（m_towers[5] 每塔 0x390、
// 槽 0 起点 +0x98、m_tableBest@+0x7058、m_tableType@+0x70f0），一律具名字段访问。

unsigned int CDeathTowerRanking::makeDungeonIdx2TowerIdx(unsigned int dungeonIdx)
{
    // ORIG 08469d69 W：dungeonIdx∈[0x2af8,0x2afd] → idx-0x2af8（塔序号 0..5），其余 -1。
    // ORIG 0x8469d6c..0x8469d88：cmp1 jbe→NEG@8469d7e；cmp2 jbe→POS@8469d85；
    // NEG 为 cmp2 fall-through，POS 殿后 —— && 条件 goto POS，-1 顺落（范式24 落点岛）。
    if (dungeonIdx > 0x2af7 && dungeonIdx <= 0x2afd)
        goto POS;
    return 0xffffffff;
POS:
    return dungeonIdx - 0x2af8;
}

void* CDeathTowerRanking::getRankTable(unsigned int type)
{
    // ORIG 082a774c W：4 张静态表（BSS 0x941f8c0，每表 0x70f4；Init 循环
    // type=1..4 调 getRankTable）懒构造（__cxa_guard + 逐表 C1）；
    // type==0 返回首表，否则 (type-1)*0x70f4 偏移。
    static CDeathTowerRanking s_tables[4];  // 静态初始化由编译器 guard 保护
    if (type == 0)
        return &s_tables[0];
    return &s_tables[type - 1];
}

int CDeathTowerRanking::checkRenewMyRecord(unsigned int towerIdx,
                                           unsigned int key,
                                           unsigned int stage,
                                           unsigned int playTime)
{
    // ORIG 084682fe T：m_bestTables[towerIdx].find(key)；无记录 → -1；
    // 否则构造临时纪录(stage,playTime)，临时 > 已有 → 1，否则 0。
    // [b28d-D13] 逐条对齐 ORIG：①开头 call _getBestRecordTable(towerIdx)（0x084682fe
    // 首块 `mov 0xc(%ebp),%eax;mov %eax,0x4(%esp);mov 0x8(%ebp),%eax;call`，结果存
    // -0xc）⇒ 删除原 towerIdx>=5 越界保护（P140/P171：ORIG 无检查）与 m_bestTables
    // 直接寻址；②tmp 声明在 find/比较之后（ORIG 构造在 0x08468390 后）；③赋值序
    // playTime(0x18)→-0x38 先于 stage(0x14)→-0x3c。
    BestMap* table = _getBestRecordTable(towerIdx);
    BestMap::const_iterator it(table->find(key));
    if (it == BestMap::const_iterator(table->end()))
        return -1;
    stBestRecord_t tmp;
    tmp.m_playTime = playTime;
    tmp.m_stage = stage;
    return tmp > it->second ? 1 : 0;  // ORIG: stBestRecord_t::operator>(tmp, existing)
}

int CDeathTowerRanking::checkRenewTopRecord(unsigned int towerIdx,
                                            unsigned int clearTime,
                                            unsigned int playTime)
{
    // [b28d-solo-CDeathTower] NEAR 3（31/33）：ORIG `add $0x128` 单条 vs 4.4 拆
    // 0x120+disp8（N94，0x128%8==0）+ 返回值 bool 加宽 movzbl（P9）；P167 CLEAR。
    // ORIG 0x08468290：塔块 +0x128（= m_towers[i].m_slots[1] 起点 +0x98 内
    // flag +0x90）movzbl 读取，!=1 直接返回 0；否则构造默认 stBestRecord_t，
    // 将 param_2（clearTime）写 m_stage(+0x88)、param_3（playTime）写
    // m_playTime(+0x8c)（0x3f/0x45 两条 movl），调 stBestRecord_t::operator>
    // (tmp, m_towers[towerIdx].m_slots[1])（ORIG call 0x08469ce4）。
    // 范式36：删 topSlot 具名中转（4.4 多 1 个 4B 槽 ⇒ ORIG tmp 基址 -0xa0 平移
    // -0xa4）；ORIG 0x084682a5/0x08468320 两处独立 imul 0x390 地址重算。
    // flag 判形 ORIG 0x084682ae：movzbl (%eax); xor $0x1; test %al; je —— (x^1) truthy。
    // [b27 封顶] flag 判形 ORIG xor $1 于 %eax 内 + test %al（8 位直测）；
    // 4.4 恒 movzbl %al,%eax 加宽（直写形折 cmp$1，变量形多槽两步——P168a③
    // 同族），test %al 形不可造。另 m_flag90 偏移 0x128 拆 0x120+disp8 为
    // 4.4 常量折叠策略（N94 邻族），ORIG add $0x128 全常量形不可定向。
    if (!m_towers[towerIdx].m_slots[1].m_flag90)
        return 0;
    stBestRecord_t tmp;
    tmp.m_playTime = playTime;
    tmp.m_stage = clearTime;
    return tmp > m_towers[towerIdx].m_slots[1];
}

int CDeathTowerRanking::getVirtualRank(unsigned int towerIdx,
                                       unsigned int stage,
                                       unsigned int playTime)
{
    // [b28d-solo-CDeathTower] NDIFF=428（610/581）：P167 CLEAR（54/54）；残差 = 全帧
    // 槽号平移（-0x120 vs -0x154 起）+ push %esi（N82/P112）。插值/跨层扫描语义全等。
    // 候选新纪录（stage, playTime）。ORIG 0x08468de0：本类无 towerIdx 越界检查。
    // 调用方（rankTable 槽位）保证 towerIdx 在 [0,4]，与 ORIG 保持一致不加防御。

    stBestRecord_t newRec;
    newRec.m_stage = stage;
    newRec.m_playTime = playTime;

    // 1) 表级最优（+0x7058 m_tableBest）严格优于新纪录 → -1。
    if (m_tableBest > newRec)
        return -1;
    // 2) 新纪录 == 表级最优 → 返回表级纪录存故名次（+0x70ec = m_tableBest.m_field94）。
    if (newRec == m_tableBest)
        return m_tableBest.m_field94;
    // 3) 新纪录严格优于该塔当前榜首（+0x98 = 槽 1）→ 名次 1。
    if (newRec > m_towers[towerIdx].m_slots[1])
        return 1;

    // ---- 层内 rank map（键=通关时间，值=名次）插值 ----
    RankMap& m = m_rankMap[towerIdx][stage];

    unsigned int result = 0xffffffff;  // ORIG local_20（初值 -1）
    RankMap::const_iterator ref;       // ORIG local_88（末次返回的基准迭代器）
    // [b28d-D13] ORIG 0x08468eab/0x08468eb9 两次 C1Ev（默认构造）⇒ nxt 先声明
    // 后赋值（P50 反例复活），禁初始化式（会折成拷贝构造，丢 C1Ev）。
    RankMap::const_iterator nxt;
    bool inMap = false;                // ORIG local_28 != 0

    if (m.size() >= 2)
    {
        // playTime 须落在 [首键, 末键] 内；否则跳至“层邻插值”分支。
        RankMap::const_iterator first = m.begin();
        RankMap::const_iterator last = m.end();
        --last;
        if (playTime >= first->first && last->first >= playTime)
        {
            // lower_bound(playTime)；若命中键 > playTime 且非 begin，退一位使
            // ref 恒为键 < playTime 的最近元素（= 插值下界）。
            ref = m.lower_bound(playTime);
            if (playTime < ref->first && ref != m.begin())
                --ref;
            nxt = ref;
            ++nxt;

            if (ref->first == playTime)
                return (int)ref->second;
            if (nxt->first == playTime)
                return (int)nxt->second;

            // 两相邻键线性插值：
            //   keySpan = nxt.key - ref.key（≥1）
            //   slope = |nxt.value - ref.value| / keySpan
            //   result = max(1, round((playTime - ref.key) * slope))
            unsigned int keySpan = nxt->first - ref->first;
            // [b28d-D13] p167 差集：ORIG 调 std::max<unsigned>×4 ⇒ 4 处 max 源形。
            keySpan = std::max(keySpan, 1u);
            long long valDiff = (long long)nxt->second - (long long)ref->second;
            long long absDiff = valDiff < 0 ? -valDiff : valDiff;
            float slope = (float)((long double)absDiff / (long double)keySpan);
            // [b28d-D13] 删发明 helper roundHalfEven（p167 EXTRA×2）；ORIG
            // 0x0846921c 形 = fildll+fmuls+fistpll（P176）⇒ 直接 (long long) 转换。
            long long scaled =
                (long long)((long double)(playTime - ref->first) * slope);
            unsigned int r = (unsigned int)scaled;
            result = std::max(r, 1u);
            inMap = true;
        }
    }

    if (!inMap)   // ORIG if (local_28 == 0)：map 元素 <2 或 playTime 超出键范围
    {
        unsigned int lowerStage = 0;   // ORIG local_18
        unsigned int upperStage = 0;   // ORIG local_14

        // 当前层 map 仅 1 元素时先判其末元素。
        if (!m.empty())
        {
            RankMap::const_iterator last = m.end();
            --last;
            if (last->first == playTime)
                return (int)last->second;
            if (last->first < playTime)
                upperStage = stage;
            else
                lowerStage = stage;
        }

        // 向下 / 向上找最近的、rank map 非空的相邻层（下界 ≥2，上界 <200）。
        if (lowerStage == 0)
        {
            lowerStage = stage;
            do
            {
                lowerStage = lowerStage - 1;
                if (lowerStage < 2 ||
                    !m_rankMap[towerIdx][lowerStage].empty())
                    break;
            } while (true);
        }
        if (upperStage == 0)
        {
            upperStage = stage;
            do
            {
                upperStage = upperStage + 1;
                if (upperStage >= kRankStageCount ||
                    !m_rankMap[towerIdx][upperStage].empty())
                    break;
            } while (true);
        }

        // 下界层无纪录（或已下探至 <2 仍空）或上界层无纪录（或已上探至 200 仍空）→ -1。
        // （ORIG 对上界 local_14==200 会无条件读 map[200] 越界；此处用
        //   upperStage>=kRankStageCount 短路护栏避免真实数组越界，语义同“无邻层”。）
        if (m_rankMap[towerIdx][lowerStage].empty() ||
            upperStage >= kRankStageCount ||
            m_rankMap[towerIdx][upperStage].empty())
            return -1;

        // 跨层插值：ref = 上界层末元素（最大键），基准 = 下界层首元素；
        //   stageSpan = upperStage - lowerStage（≥1）
        //   slope = |lower.first.value - upper.last.value| / stageSpan
        //   result = max(1, round((upperStage - stage) * slope))
        RankMap::const_iterator lowerFirst =
            m_rankMap[towerIdx][lowerStage].begin();
        ref = m_rankMap[towerIdx][upperStage].end();
        --ref;

        unsigned int stageSpan = upperStage - lowerStage;
        stageSpan = std::max(stageSpan, 1u);
        long long valDiff = (long long)lowerFirst->second -
                            (long long)ref->second;
        long long absDiff = valDiff < 0 ? -valDiff : valDiff;
        float slope = (float)((long double)absDiff / (long double)stageSpan);
        long long scaled =
            (long long)((long double)(upperStage - stage) * slope);
        unsigned int r = (unsigned int)scaled;
        result = std::max(r, 1u);
    }

    return (int)(result + ref->second);
}

void CDeathTowerRanking::unregistBestRecord(unsigned int characNo)
{
    // ORIG 08468b4c T：for i in 0..4: _getBestRecordTable(i).erase(characNo)。
    // ORIG 0x8468b4c：体内 call _getBestRecordTable@0x8469dae（跨 TU W）；int 循环（setle）。
    for (int i = 0; i <= 4; ++i)
        _getBestRecordTable(i)->erase(characNo);
}

void CDeathTowerRanking::unregistRanking(unsigned int characNo)
{
    // ORIG 08468552 T：for i in 0..4: _getRankingTable(i).erase(characNo)。
    // ORIG 0x8468561：体内 call _getRankingTable@0x8469d90（跨 TU W 符号）；
    // 循环变量 int（0x8468583 cmpl $0x4 + setle 有符号实证）。
    for (int i = 0; i <= 4; ++i)
        _getRankingTable(i)->erase(characNo);
}

// ORIG _ZN8WongWork18CDeathTowerRanking4initEj @ 0x084680a4（0x1ec 字节）。
// this = getRankTable(type) 表；对 5 座塔 × 槽位 1..5 重置 stBestRecord_t：
//   m_flag90(+0x90) 清字节（ORIG movb）；m_memberData[0](+0x04) memcpy 默认名
//   "AAAAAA"/.../"EEEEEE"（ORIG 0x8c63370..0x8c6338d，7 字节含 NUL）；
//   m_field94(+0x94)=槽序号、m_playTime(+0x8c)=0、m_stage(+0x88)=200-槽序号
//   （ORIG 三条 dword 写，顺序 0x94→0x8c→0x88）；
// （this=表对象，ORIG Init 0x082a1f69..0x082a1f75 实证 getRankTable 结果作
// this 传入；体内不调 getRankTable）最后按 ORIG 顺序 m_tableType(+0x70f0)=type、
// m_tableBest.m_stage(+0x70e0)=200（ORIG 0x1d3 先写 type、0x1dc 再写 200）。
void CDeathTowerRanking::init(unsigned int type)
{
    // [b28d-solo-CDeathTower] NDIFF=67（119/104）：双层循环变量槽序 + push %ebx
    // （N93/N98/P212）；P167 CLEAR（5/5）。语义（5 塔 x5 槽重置/memcpy 名/表级 stage=200）全等。
    for (unsigned int tower = 0; tower <= 4; ++tower)
    {
        for (unsigned int slot = 1; slot <= 5; ++slot)
            m_towers[tower].m_slots[slot].m_flag90 = 0;
        // ORIG 0x8c63370 实证每名 6 字符 + NUL（"AAAAAA\0" 等，7 字节；
        // memcpy 7 字节 = 6 字母 + NUL，与 ORIG 逐字节一致）。槽起点 +0x04 =
        // m_memberData[0]（ORIG 五次独立 memcpy 调用）。
        std::memcpy(m_towers[tower].m_slots[1].m_memberData[0], "AAAAAA", 7);
        std::memcpy(m_towers[tower].m_slots[2].m_memberData[0], "BBBBBB", 7);
        std::memcpy(m_towers[tower].m_slots[3].m_memberData[0], "CCCCCC", 7);
        std::memcpy(m_towers[tower].m_slots[4].m_memberData[0], "DDDDDD", 7);
        std::memcpy(m_towers[tower].m_slots[5].m_memberData[0], "EEEEEE", 7);
        for (unsigned int slot = 1; slot <= 5; ++slot)
        {
            stBestRecord_t& rec = m_towers[tower].m_slots[slot];
            rec.m_field94 = static_cast<int>(slot);
            rec.m_playTime = 0;
            rec.m_stage = 200 - slot;
        }
    }
    m_tableType = type;
    m_tableBest.m_stage = 200;
}

CDeathTower::CDeathTower()
{
    // [b28d-solo-CDeathTower] NEAR 8（45/53）：唯一残差 = CStage 子对象 ctor 异常路径
    // EH cleanup（4.4 恒发 ~CStage + _Unwind_Resume，4.1 无）——N91 封顶（判词保留）。
    // ORIG 0x82a75da：CDungeonMgr/CStage/CPlayData 子对象 ctor 由编译器生成；
    // 函数体仅将 m_timerKeys[TIMER_MESSAGE 0x48..0x4e] 清零。
    // [b27 封顶] 唯一残差 = EH cleanup 段：4.4 对 CStage 子对象 ctor 异常路径
    // 恒发 ~CStage call + _Unwind_Resume（N91：4.1 对 non-trivial dtor 元素
    // 隐式构造不生成 EH 残段），禁删子对象 dtor ⇒ 无源码解。主体/槽/循环全等。
    for (int i = 0x48; i <= 0x4e; ++i)
        m_timerKeys[i - 0x48] = 0;
}

// [b27 封顶] D1 残差（33 vs 43）= CStage 析构 EH 双份 epilogue（4.4 恒发
// ~CStage cleanup 块 + ebx/esi 保存，4.1 merge 为单块）——N91 同根（C1 同判
// 见 ctor 注），无源码解。
// [b28d-solo-CDeathTower] NEAR 9（33/41）：CStage 析构 EH 双份 epilogue
//（4.4 恒发 ~CStage cleanup + ebx/esi，4.1 merge 为单块）——N91 封顶。
CDeathTower::~CDeathTower() {}

int CDeathTower::getIdx()
{
    return m_idx;
}

int CDeathTower::getTowerState()
{
    return m_towerState;
}

void CDeathTower::setIdx(int idx)
{
    m_idx = idx;
}
int CDeathTower::setTowerState(ENUM_TOWER_STATE state)
{
    // ORIG 0x8469c34：state <= m_towerState 时返回 0；否则写入并返回 1。
    if (state <= (ENUM_TOWER_STATE)m_towerState)
    {
        return 0;
    }
    m_towerState = state;  // ORIG 0x8469c4b：同型直落 → 先载 this(%eax) 再载 state(%edx)
    return 1;
}

void CDeathTower::reset()
{
    m_dungeonMgr.reset();
    m_stage.reset();
    m_playData.reset();
    m_party = 0;
    m_towerState = TOWER_STATE_NONE;
}

void CDeathTower::_destroy()
{
    setTowerState((ENUM_TOWER_STATE)0);
    G_CGameManager()->returnDeathTower(this);
}

CDeathTower::CDungeonMgr* CDeathTower::getCDungeonMGr() const
{
    return (CDungeonMgr*)&m_dungeonMgr;
}

CDeathTower::CStage* CDeathTower::getCStage()
{
    return &m_stage;
}

void CDeathTower::attachParty(CParty* party)
{
    m_party = party;
    // ORIG 0x8462a3c/0x8462a45：i 双次置 0 直落 -0xc（范式8）；int 计数 setle。
    int i = 0;
    i = 0;
    for (; i <= 3; ++i)
    {
        // ORIG 0x8462a45：receiver 走 this->m_party reload（mov 0x8,%eax; mov(%eax),%eax），
        // 非 party 形参槽。
        if (m_party->checkValidUser(i))
        {
            // ORIG 0x8462a61：movswl %ax,%ebx sext 直落 %ebx（P200：短整表达式
            // 作普通实参直传并跨 get_user call 存活，禁 asm 绑定/中间局部）。
            m_party->get_user(i)->setDeathTowerIndex((short)m_idx);
        }
    }
    setTowerState((ENUM_TOWER_STATE)1);
}

int CDeathTower::genTimerKey(TIMER_MESSAGE msg)
{
    return _genTimerKey(msg);
}

int CDeathTower::_genTimerKey(TIMER_MESSAGE msg)
{
    // ORIG 0x830edec：返回 ++m_timerKeys[msg - 0x48]。
    return ++m_timerKeys[(int)msg - 0x48];
}

bool CDeathTower::_checkTimerKey(TIMER_MESSAGE msg, unsigned int key)
{
    // ORIG 0x8469c68：key == m_timerKeys[msg - 0x48]。
    return key == (unsigned int)m_timerKeys[(int)msg - 0x48];
}

bool CDeathTower::_checkMemberDie()
{
    // ORIG 0x8467188：循环变量 unsigned（0x84671ea setbe 实证）；双次置 0
    // （0x846718e/0x8467195）；&& 物化岛（范式40）：valid&&alive 共享 mov$1/$0
    // （0x84671d2/0x84671d9）；break 后尾置 i==4 两段物化（0x84671f4..0x8467201）。
    unsigned int i = 0;
    i = 0;
    for (; i <= 3; ++i)
    {
        // [b28d-D13] P0 清绑（N96）：ORIG 0x84671d2 mov$1,%eax 无栈槽 ⇒ 表达式直用。
        if (m_party->checkValidUser(i) && m_playData.isMemberAlive(i))
        {
            break;
        }
    }
    if (i == 4)
    {
        return 1;
    }
    return 0;
}

// ORIG rodata 0x8c635f0/0x8c635f4（nm：_ZL37CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT /
// _ZL36CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT，各 4B r）：4.1 以内存读引用
// （_checkMemberReady 0x8467122/0x8467129 `mov 0x8c635f0/4,%eax`）⇒ volatile 阻止
// 4.4 常量化为立即数；值 120/90 沿用既有转写。
static volatile const int CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT = 120;
static volatile const int CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT = 90;

void CDeathTower::_checkMemberReady()
{
    // [b28d-solo-CDeathTower] NEAR 8（41/43）：ORIG volatile const 内存读
    //（0x08c635f0/4）vs 4.4 常量/槽 + push %edi 中转（P0/N 族，既有判词保留）。
    m_playData.resetMemberReady();
    // ORIG 0x84670f8：call 结果直 cmp $0x1（免具名）；key/idx 走 ebx/esi
    // callee-saved（范式2）；函数尾无 mov %eax 置值 ⇒ 返回语义 void。
    // [b28d-D13] N 封顶：ORIG 0x084670f8 timeout@-0xc 直读 volatile const；
    // 4.4 三元恒经 -0x1c 槽 + %edi 中转（帧 0x30 vs 0x3c）。if/else 赋值形实测
    // 12→17 反劣 ⇒ 保持三元。
    unsigned int timeout =
        m_party->get_member_count() == 1
            ? (unsigned int)CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT
            : (unsigned int)CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT;
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x49, timeout,
        _genTimerKey((TIMER_MESSAGE)0x49), 0x49);
}

bool CDeathTower::onLeaveUser(CUser* user)
{
    user->setDeathTowerIndex((short)-1);
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    inven->RemoveFPItem();
    // [b28d-solo-CDeathTower] ORIG 0x08463724 起内联对齐 memset（无 memset call）⇒
    // 写 `= {0}`（P214③/P232）。
    char memberNames[0x100] = {0};

    if (m_party)
    {
        // ORIG 0x0846378d：state = (char)m_party->m_battleField.m_bloodState（+0xcd8 低字节）
        char state = (char)m_party->m_battleField.m_bloodState;
        m_party->getMemberNames(memberNames);   // char* 返回（ORIG W 转发 _getMemberNames）
        user->m_historyLog.LeaveDungeon(
            m_dungeonMgr.getDungeon()->GetDungeonName(), 0, memberNames, state);
    }

    // [b28d-solo-CDeathTower] ORIG 0x084637f7..0x08463845：m_party && getDungeon()!=0，
    // 再重调 getDungeon()->get_index()（P183 重物化，getDungeon 共 2 次）。
    if (m_party && m_dungeonMgr.getDungeon() != 0)
    {
        CDungeonEntranceLog* entranceLog = GetInstanceDungeonEntranceLog();
        entranceLog->DecrementDungeonEntrance(
            m_dungeonMgr.getDungeon()->get_index(), false);
    }

    // ORIG 0x0846384a：!m_party || get_member_count() <= 0 → _destroy()
    if (!m_party || m_party->get_member_count() <= 0)
    {
        _destroy();
    }
    return 1;
}

// [b28d-69] P167 CLEAR（ORIG calls=103 = OUR 103）：P183 每使用点重发
// getDungeon()->get_index()/getCurCharacNo()/getStartMemberCnt()（ORIG 11/9/6/2 次实证）
// + P0 去 partyState/key/mgrIdx 缓存 + P197③ playTime 预清 + P184 内层死块照抄
// + 2×min<unsigned>+1×min<int>（P211/P265）。ndiff 566→16，orig 610 / our 608；
// 残余仅 4.4 帧 0xdc vs ORIG 0xcc 的槽号整体 -0x10 平移 + 实参跨 call 落槽 vs
// ORIG callee-saved（N82/N95/N7 族）⇒ 语义完成、byte 层 N 封顶。
void CDeathTower::_beginTowerClearProcess(bool isLast)
{
    if (getTowerState() > 2)
        return;

    // [b28d-69] 声明序照 ORIG 槽（curStage -0x5c / playTime -0x40 预清 / memberCnt -0x3c）；
    // partyState/key/mgrIdx 均无槽往返 ⇒ 原码为表达式直用（P0），勿声明局部。
    unsigned int curStage;
    unsigned int playTime = 0;
    int memberCnt = m_playData.getStartMemberCnt();
    void* rankTable = WongWork::CDeathTowerRanking::getRankTable(memberCnt);

    _onClear(isLast);

    if (m_party->m_battleField.m_bloodState == 1)
    {
        setTowerState((ENUM_TOWER_STATE)5);
        m_party->SetEPLPState(2);

        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4d, 0x3c,
            _genTimerKey((TIMER_MESSAGE)0x4d), 0x4d);

        PacketGuard pkt;
        pkt.put_header(0, 0xfe);
        pkt.finalize(true);
        m_party->send_to_party(pkt);
    }
    else
    {
        setTowerState((ENUM_TOWER_STATE)3);

        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4b, 1,
            _genTimerKey((TIMER_MESSAGE)0x4b), 0x4b);
    }

    curStage = m_stage.getCurrentStage();
    if (isLast)
    {
        playTime = m_playData.getPlayTime();
    }
    else
    {
        playTime = m_playData.getLastPlayTime();
        int stageMinus1 = curStage - 1;
        int one = 1;
        curStage = std::max(one, stageMinus1);
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = 0;
        user = m_party->get_user(i);
        if (!user || !m_party->checkValidUser(i))
            continue;

        user->set_charac_party_bonus_exp(0);
        user->set_charac_member_bonus_exp(0);
        user->set_charac_fatigue_buf_bonus_exp(0);
        user->set_charac_seria_buf_bonus_exp(0);

        // [b28d-69] P183 地址重物化铁律：ORIG 每个使用点重发 getDungeon()->get_index()/
        // getCurCharacNo()（p167 差集 getDungeon 11/get_index 9/getCurCharacNo 6 实证），
        // 禁缓存指针或索引；isNewRecord 的 P184 死块（内层再测 isLast）照抄。
        int isNewRecord = 0;
        int result = _checkRenewMyRecord(
            m_dungeonMgr.getDungeon()->get_index(), memberCnt,
            user->getCurCharacNo(), curStage, playTime);
        if (isLast)
        {
            isNewRecord = (result == 1);
            if (_checkRenewTopRecord(m_dungeonMgr.getDungeon()->get_index(), memberCnt,
                                     curStage, playTime) && isLast)
                isNewRecord += 2;
        }

        int virtualRank = ((WongWork::CDeathTowerRanking*)rankTable)->getVirtualRank(
            WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                m_dungeonMgr.getDungeon()->get_index()),
            curStage, playTime);

        // ORIG 0x0846760e：notTower = (get_index() != 11000)；随后统计/记录上报
        char notTower = (char)(m_dungeonMgr.getDungeon()->get_index() != 0x2af8);
        GetInstanceDeathTowerPlaydataStatistic()->SendDeathTowerPlayInfo(
            i, notTower,
            (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
            curStage, m_playData.getStartMemberCnt());

        GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
            (DEATHTOWER_VALUE_STATISTIC_FIELD)1, notTower,
            (char)user->get_charac_level(), curStage);
        GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
            (DEATHTOWER_VALUE_STATISTIC_FIELD)0, notTower,
            (char)user->get_charac_level(), 1);

        if (result != 0)
        {
            DB_UpdateDeathTowerBestRecord::makeRequest(
                WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                    m_dungeonMgr.getDungeon()->get_index()),
                user->getCurCharacNo(), memberCnt,
                (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
                curStage, playTime);
            if (virtualRank > 0)
            {
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)0)->updateTowerRank(
                        user->getCurCharacNo(),
                        WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                            m_dungeonMgr.getDungeon()->get_index()),
                        memberCnt, virtualRank);
                ((WongWork::CDeathTowerRanking*)rankTable)->updateRanking(
                    WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                        m_dungeonMgr.getDungeon()->get_index()),
                    user->getCurCharacNo(), virtualRank);
            }
            ((WongWork::CDeathTowerRanking*)rankTable)->registBestRecord(
                WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                    m_dungeonMgr.getDungeon()->get_index()),
                user->getCurCharacNo(),
                (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
                curStage, playTime);
        }

        CPacketHandler handler;
        handler.makeDisplayRangking(
            memberCnt, user->getCurCharacNo(), curStage, playTime, (result == 1),
            m_dungeonMgr.getDungeon()->get_index(), virtualRank);
        user->Send(handler);

        // 奖励数量：ORIG _beginTowerClearProcess 0x0846791f..0x08467948 两套
        // int 表（limitOfStackableItemInTower 决定）：`mov 0xc(dm+(tier+stage)*4)`
        // = m_deathTower.m_rewardCardNum[stage]（+0x324 基）/
        //   m_illusionRewardCardNum[stage]（+0x964 基）。
        int rewardNumBase;
        if (m_dungeonMgr.getDungeon()->limitOfStackableItemInTower())
            rewardNumBase = G_CDataManager()->m_deathTower.m_rewardCardNum[curStage];
        else
            rewardNumBase =
                G_CDataManager()->m_deathTower.m_illusionRewardCardNum[curStage];
        // ORIG 三次 min：min<unsigned>(maxCardNum,cardNum) → min<int>(10,rewardItemNum)
        // → min<unsigned>(10,curStage)（p167 差集 2×min<unsigned>+1×min<int> 实证）。
        unsigned int cardNum = (unsigned int)rewardNumBase + (unsigned int)isNewRecord;
        unsigned int maxCardNum =
            (unsigned int)m_dungeonMgr.getDungeon()->getTowerMaxClearRewardItemNum();
        int rewardItemNum = std::min(maxCardNum, cardNum);
        rewardItemNum = std::min(10, rewardItemNum);

        m_playData.generateRewardItem(i, rewardItemNum, m_dungeonMgr,
                                      user->get_charac_level(),
                                      std::min(10u, curStage), m_party);

        unsigned int mobReward = 0;
        G_CDataManager()->get_mob_reward(user->get_charac_level(), mobReward);
        QuickParty::RewardData_Param rewardParam(m_party);
        QuickParty::RewardData_Result rewardResult;
        rewardResult.m_value00 = mobReward;
        m_party->set_random_reward((QuickParty::RandomBuffType)0, rewardParam,
                                   rewardResult);
        mobReward = rewardResult.m_value00;

        m_playData.generateRewardExp(i, curStage, mobReward, m_party, m_dungeonMgr);
    }
}
// [b28d-solo-CDeathTower] P167 CLEAR（ORIG calls=149 = OUR 149，0 missing/0 extra）。
// 本轮修 11 项语义缺口：GetPremiumInfoList 改按值返回版（ORIG 0x0846496e）；premium
// 循环三次独立 operator[]（P157）；dropItemIds 改 unsigned long 左值 push_back(const&)
// + const_iterator 具名对（0x08464d63）；掉落项存 stGenerateResult_t::m_dropItems
// （C1 0x08464ae8 / D1 x2）；dropList/result 声明提前到 a5 赋值前；PacketGuard 改嵌套
// CPacketHandler（C1 0x08469a8c）；put_binary 改 const char* 重载；emblemInfo 循环外
// 单次 init 复用；get_party_type 改 m_battleField.m_bloodState（真语义修正，
// 0x08464dXX `*(int*)(*(int*)this+0xcd8)!=1`）；去掉 dungeon 判空（P140/P171）。
// 剩余 ndiff 596 = 4.4 帧 0x56c vs ORIG 0x53c 的槽号整体平移 + 实参跨 call 落槽
// （N82/N95/N7 族）⇒ 语义完成、byte 层 N 封顶。
int CDeathTower::handleKillMonster(CUser* user, unsigned short monsterIdx, unsigned short a3, const MSG_MONSTER_DIE& msg, int* a5)
{
    if (m_stage.getState() != 1)
    {
        if (user)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2a, 0x13);
        }
        return 0;
    }

    // Monster kill checker (skill material time)
    stMapMonsterKillChecker_t* checker = (stMapMonsterKillChecker_t*)&m_timerKeys[7];
    bool bSkillMaterial = false;
    if (checker->getUseSkillMaterialTime() == 0)
    {
        unsigned int curTick = GlobalData::s_systemTime_.getCurTickCount();
        int result = checker->dieMonster(curTick, 1000);
        if (result > 2)
        {
            bSkillMaterial = true;
        }
    }

    if (bSkillMaterial)
    {
        bool isPremium = false;
        if (user && user->get_charac_level() >= 0x1e)
        {
            isPremium = true;
        }
        else if (user && user->get_charac_level() >= 0x19)
        {
            CUserPremium* premium = user->GetPremiumInfo();
            if (premium)
            {
                // [b28d-solo-CDeathTower] ORIG 0x0846496e 调按值返回版
                // _ZNK8WongWork12CUserPremium18GetPremiumInfoListEi（sret -0xdc，type=0x270f）。
                std::vector<SUserPremiumInfo> premiumList =
                    premium->GetPremiumInfoList(0x270f);
                for (int i = 0; i < (int)premiumList.size(); i++)
                {
                    // [b28d-solo-CDeathTower] P157/P183：ORIG 0x0846498f/ab/c7 三次
                    // 独立 operator[]（每字段重读，无 type 缓存）。
                    if (premiumList[i].m_type >= 0x17 &&
                        premiumList[i].m_type <= 0x1b &&
                        premiumList[i].m_state == 1)
                    {
                        isPremium = true;
                        break;
                    }
                }
            }
        }

        if (isPremium)
        {
            checker->initMonsterInfo();
            CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
            if (hackAnalyzer)
            {
                hackAnalyzer->addServerHackCnt(user, (WongWork::ENUM_HACK_TYPE)0xd6, 1, 0, 0);
            }
        }
    }

    // Kill monster in stage
    map_monster monster;
    int killResult = m_stage.killMonster(monsterIdx, monster);
    if (killResult == 0)
    {
        return 0;
    }

    // ORIG 0x08464ab4：调用 getMemberLevelGap()，返回值 ORIG 亦弃用（汇编仅存栈）
    int memberLevelGap = m_party->getMemberLevelGap();
    int winPoint = 0;
    std::vector<unsigned long> dropItemIds;
    std::list<map_item> dropList;
    // [b28d-solo-CDeathTower] ORIG 0x08464ae8：stGenerateResult_t 在 dropItemIds/
    // dropList 之后构造（p167：C1 ×1 / D1 ×2），掉落项存 result.m_dropItems。
    stGenerateResult_t result;
    // ORIG 0x08464aed：三个容器构造完成后才把 mobIndex 写回 a5（if (a5)）
    if (a5)
    {
        *a5 = monster.m_mobIndex;
    }

    if (monster.m_roleType < 5)
    {
        // Boss monster drop
        CMonster* monsterPtr = (CMonster*)G_CDataManager()->find_monster(monster.m_mobIndex);
        // [b28d-solo-CDeathTower] ORIG 0x08464c1a..c60：monsterPtr 非空后短路求值
        // getDungeon()->hasTowerItemDrop()（ORIG 无 dungeon 判空，P140/P171）。
        unsigned long dropId = 0xfffffffe;
        if (monsterPtr != 0 && m_dungeonMgr.getDungeon()->hasTowerItemDrop())
        {
            dropId = monsterPtr->deathTowerSpecifyItemDrop();
        }
        // [b28d-solo-CDeathTower] ORIG 0x08464d63：push_back 传左值 ⇒ const& 版。
        if (dropId != 0xfffffffe)
        {
            dropItemIds.push_back(dropId);
        }
    }
    else
    {
        // Normal monster drop
        CAICharacterList* aiCharList = G_CDataManager()->m_aiCharacterList;
        CAICharacter* aiChar = 0;
        if (aiCharList)
        {
            aiChar = aiCharList->get(monster.m_mobIndex);
        }
        if (aiChar)
        {
            // [b28d-solo-CDeathTower] ORIG 0x08464b15 段：无 dungeon 判空。
            if (m_dungeonMgr.getDungeon()->hasTowerItemDrop())
                aiChar->deathTowerSpecifyItemDrop(dropItemIds);
            if (monster.m_dropCnt == 100 || monster.m_dropCnt == 200)
            {
                winPoint = aiChar->getPvPWinPoint();
            }
        }
    }

    // Create items from drop ids（ORIG 0x08464c97 段：const_iterator 遍历 + empty 前置判定）
    if (!dropItemIds.empty())
    {
        // [b28d-solo-CDeathTower] ORIG 0x08464c97 段：begin/end 各转 const_iterator
        // 具名持有（local_e8/local_ec），再比较两个 const_iterator（p167 差集实证）。
        std::vector<unsigned long>::const_iterator it = dropItemIds.begin();
        std::vector<unsigned long>::const_iterator itEnd = dropItemIds.end();
        for (; it != itEnd; ++it)
        {
            int itemId = (int)*it;
            CItem* item = G_CDataManager()->m_itemList->find_item(itemId);
            if (item)
            {
                Inven_Item invenItem;
                // ORIG 0x08464ce1 第二次 operator*（P157 重读，非缓存 itemId）
                invenItem.m_addInfo = (int)*it;
                G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)1, invenItem, 1);
                unsigned int memberCnt = m_party->get_member_count();
                for (unsigned int j = 0; j < memberCnt; j++)
                {
                    result.m_dropItems.push_back(invenItem);
                }
            }
        }
    }

    // Exp rewards for party members
    for (int i = 0; i < 4; i++)
    {
        CUser* partyUser = m_party->get_user(i);
        if (!partyUser || !m_party->checkValidUser(i))
        {
            continue;
        }

        // [b28d-solo-CDeathTower] ORIG 0x08464dXX 段：`*(int*)(*(int*)this+0xcd8)!=1`
        // = m_party->m_battleField.m_bloodState != 1（不是 get_party_type，真语义修正）。
        if (m_party->m_battleField.m_bloodState != 1)
        {
            unsigned int mobExp = 0;
            int charLevel = partyUser->get_charac_level();
            G_CDataManager()->get_mob_reward(charLevel, mobExp);
            // ORIG 0x08464e69：getMonsterTotalExp 后经 set_random_reward 取回
            mobExp = m_party->getMonsterTotalExp(mobExp, false, 0, 1.0f, 1.0f, false);
            QuickParty::RewardData_Param rewardParam(m_party);
            QuickParty::RewardData_Result rewardResult;
            rewardResult.m_value00 = mobExp;
            m_party->set_random_reward((QuickParty::RandomBuffType)0, rewardParam,
                                       rewardResult);
            mobExp = rewardResult.m_value00;

            if (monster.m_roleType == 5)
            {
                // ORIG 0x08464f2c：+0x5198 = CDataManager::m_deathTower.m_apcExpWeight
                mobExp = (unsigned int)(G_CDataManager()->m_deathTower.m_apcExpWeight *
                                        (float)mobExp);
            }

            // ORIG 0x08464f80：+0x5e1c = m_deathTower.m_monsterExpWeight（无条件）
            mobExp = (unsigned int)(G_CDataManager()->m_deathTower.m_monsterExpWeight *
                                    (float)mobExp);

            // ORIG 0x08465000：STGainedEachExpData(m_exp=mobExp) 后
            // sumGainedEachExp 取回
            STGainedEachExpData expData;
            expData.m_exp = mobExp;
            mobExp = m_party->sumGainedEachExp(partyUser, expData);

            if (winPoint != 0)
            {
                // ORIG 0x08465017：GetRepeatEvent(0x27) 后直接调 getBonusRate
                // （不判空，与 ORIG 一致）；rate==0 → 原值，否则乘率取整
                CDeathTowerWinPointEvent* event =
                    (CDeathTowerWinPointEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x27);
                float bonusRate = event->getBonusRate();
                if (bonusRate == 0.0f)
                    partyUser->gainWinPoint(winPoint, (eWPAddReason)2);
                else
                    partyUser->gainWinPoint((int)((float)winPoint * bonusRate),
                                            (eWPAddReason)2);
            }

            // ORIG 0x08465090：calcHelpAbuseRatio(mobExp, false)
            partyUser->calcHelpAbuseRatio(mobExp, false);

            // ORIG 0x084650XX：expGain(-0x120)=0 → notiType(-0x34)=0xb → spGain(-0x124)=0
            int expGain = 0;
            int notiType = 0xb;
            if (monster.m_roleType == 5)
            {
                notiType = 0xc;
            }

            int spGain = 0;
            if (partyUser->gain_exp_sp(mobExp, expGain, spGain,
                                       (CUser::eExpAddReason)notiType,
                                       monster.m_mobIndex, false))
            {
                partyUser->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
            }

            // ORIG 0x0846515f：cUserHistoryLog（CUser 嵌套成员 m_historyLog，+0x79700）
            // KillMonster 日志；killValue1=msg+0xa1e / killValue2=msg+0xa1a /
            // a=+0xa24 / b=+0xa26；curExp=getCurCharacExp()
            partyUser->m_historyLog.KillMonster(
                monster.m_mobIndex, msg.m_killValue1, msg.m_killValue2,
                msg.m_killPairA24, msg.m_killPairA26, msg,
                static_cast<char>(monster.m_roleType), monster.m_roleType == 5,
                partyUser->getCurCharacExp(), (int)mobExp,
                (CUser::eExpAddReason)notiType);

            // ORIG 0x08465244：CheckQuestMonster(dungeonIndex, mobIndex, type)
            // type：roleType 5..8 → 2，否则 1
            {
                int questType = 1;
                if (monster.m_roleType > 4 && monster.m_roleType < 9)
                    questType = 2;
                // ORIG 0x08465244：无 dungeon 判空（P140/P171）
                partyUser->CheckQuestMonster(m_dungeonMgr.getDungeon()->get_index(),
                                             monster.m_mobIndex,
                                             (ENUM_QUEST_ENEMY_TYPE)questType);
            }
        }
    }

    // Drop items to stage and send packet（ORIG 0x084653XX：const_iterator 遍历 result.m_dropItems）
    std::vector<Inven_Item>::const_iterator it = result.m_dropItems.begin();
    std::vector<Inven_Item>::const_iterator itEnd = result.m_dropItems.end();
    for (; it != itEnd; ++it)
    {
        map_item item;
        item.m_count = 1;
        item.m_itemIndex = a3;
        item.m_createTick = GlobalData::s_systemTime_.getCurSec();
        item.m_item = *it;
        m_stage.dropItem(item);
        dropList.push_back(item);
    }

    // Send drop item packet to party
    // [b28d-solo-CDeathTower] p167：ORIG 用嵌套 CPacketHandler（C1 0x08469a8c / D1 ×2）。
    CPacketHandler packet;
    packet.put_header(0, 0x26);
    packet.put_short(monsterIdx);
    int itemCountPos = packet.get_index();
    packet.put_byte(0);  // placeholder for count
    // ORIG 0x084654XX：emblemInfo 在循环前声明并 init 一次，循环内复用。
    stAvatarEmblemInfo_t emblemInfo;
    emblemInfo.init();

    int itemCount = 0;
    for (std::list<map_item>::const_iterator it = dropList.begin(); it != dropList.end(); ++it)
    {
        const map_item& item = *it;
        packet.put_short(item.m_itemIndex);
        packet.put_int(item.m_item.m_addInfo);
        unsigned char itemAttr = item.m_item.GetItemAttr();
        packet.put_byte(itemAttr);

        if (!item.m_item.isAvatarItemType())
        {
            packet.put_int(item.m_item.get_add_info());
        }
        else
        {
            CEquipItem* equipItem = (CEquipItem*)G_CDataManager()->find_item(item.m_item.m_addInfo);
            if (!equipItem)
            {
                packet.put_int(item.m_item.get_add_info());
            }
            else
            {
                // ORIG 0x08465554：天数(m_fieldb 低字节) * 0x15180(86400s)；同 WarField 发包
                packet.put_int(equipItem->getAvatarPeriod(
                                   (unsigned char)item.m_item.m_fieldb) *
                               0x15180);
                emblemInfo.init();
                equipItem->getAvatarSocket(item.m_item.m_fieldb, emblemInfo);
            }
        }

        packet.put_short(item.m_item.m_fieldb);
        unsigned char amplifyType = item.m_item.m_amp.getAbilityType();
        packet.put_byte(amplifyType);
        unsigned short amplifyValue = item.m_item.m_amp.getAbilityValue();
        packet.put_short(amplifyValue);
        packet.put_packet(item.m_item);

        if (!item.m_item.isAvatarItemType())
        {
            packet.put_byte(0);
        }
        else
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            // [b28d-solo-CDeathTower] p167：ORIG 调 put_binary(char const*, int)。
            packet.put_binary((const char*)&emblemInfo, 0x1e);
        }

        packet.put_short(item.m_dropIndex);
        itemCount++;
    }

    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(itemCountPos, itemCount);
    packet.finalize(true);
    m_party->send_to_party(packet);

    // Check stage clear
    if (m_stage.checkClearStage())
    {
        _onFinishStage();
        int curStage = m_stage.getCurrentStage();
        int endStage = m_dungeonMgr.getEndStage();
        if (curStage == endStage)
        {
            _onPrepareFinishTower();
        }
    }

    return 1;
}
void CDeathTower::_onClear(bool isLast)
{
    int playTime = 0;   // ORIG +0x00c 头部 movl $0,-0x14（P197③ 声明=0）；
                        // /1000 为 signed magic 削减（imul/sar/sub @0x8467f2a）⇒ int
    if (isLast)
    {
        playTime = m_playData.getPlayTime();
    }
    else
    {
        playTime = m_playData.getLastPlayTime();
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = 0;              // ORIG +0x04d 循环体首 movl $0,-0xc（P197③）
        user = m_party->get_user(i);
        if (user == 0 || !m_party->checkValidUser(i))
            continue;
        // ORIG 二次 get_user 后 eax 直传 setChattingMessageCount 实参槽
        //（无 -0xc 回读；具名 user 复制生 [mov %eax,-0xc; mov -0xc,%eax]
        // 额外往返）。
        m_party->get_user(i)->setChattingMessageCount(0);
        // CUser +0x79700：cUserHistoryLog（public 成员 m_historyLog，CUser.h 2197）
        user->m_historyLog.DungeonClearInfo((int)isLast, playTime / 1000);
    }
}
void CDeathTower::_onFinishDeathTower()
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x8467f7d：循环体首 `movl $0,-0xc`（user 先置 0 再赋值，范式8）；
        // || 共享物化岛（范式40）：0x8467f9f je→mov$1@8467fbc；第二操作数
        // `!checkValidUser` 走 xor $0x1,%eax@8467fb5（范式1 值形）。
        CUser* user = 0;
        user = m_party->get_user(i);
        // [b28d-D13] P0 清绑（N96）：ORIG 0x8467fbc mov$1 岛无槽 ⇒ 表达式直用。
        if ((user == 0) || (int)!m_party->checkValidUser(i))
            continue;
        onLeaveUser(user);
    }
    m_party->ReturnToVillage();
    if (getTowerState() > 0)
    {
        _destroy();
    }
}

void CDeathTower::_onFinishStage()
{
    // [b28d-solo-CDeathTower] NDIFF=27（85/84）：局部槽 -0x10/-0xc vs -0x18 方向差 +
    // push %ebx（N93/N82）；P167 CLEAR。语义（onFinishStage/addPlayTime/<=0x176f hack
    // 判定/quest set_authen_data）全等。
    m_stage.onFinishStage();
    m_playData.addPlayTime(m_stage.getStageClearTime());

    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = m_party->get_user(i);
        if (user == 0 || !m_party->checkValidUser(i))
            continue;
        // [b28d-D13] P196 真语义修正：ORIG 0x08467d24 cmp $0x176f; setbe（无符号
        // 比较）⇒ 源码须为 unsigned 比较（int 版 4.4 产 setle，负数时语义分叉）。
        if ((unsigned int)m_stage.getStageClearTime() <= 0x176f)
        {
            CHackAnalyzer* hack = user->getHackAnalyzer();
            hack->addServerHackCnt(user, (WongWork::ENUM_HACK_TYPE)0x19a, 1u, 0u, 0u);
        }
        UserQuest* quest = user->getCurCharacQuestW();
        quest->set_authen_data((QUEST_CONDITION)1, m_stage.getStageMapIndex(), 0);
    }
}

void CDeathTower::_onPrepareFinishTower()
{
    // [b28d-D13] P0 清绑（N96）：key 改内联实参（P24），ORIG 0x8467e00 mov %eax,%ebx
    // 为 4.4 对内联实参自然升格 callee-saved（P200/P226）。
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4a, 5,
        _genTimerKey((TIMER_MESSAGE)0x4a), 0x4a);
}

void CDeathTower::_onStartDeathTower()
{
    // [b28d-solo-CDeathTower] ORIG 0x08467b5b：memberNames 0x100 字节内联清零
    //（mov $0x40,%edx + rep stos = 0x40 dwords，P116）；旧 0x40 尺寸为误读。
    char memberNames[0x100] = {0};
    // ORIG 0x08467b83：getMemberNames 填充（转发 _getMemberNames）
    m_party->getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x08467b94：循环体首 movl $0,-0xc（P197③），再 get_user 赋值。
        CUser* user = 0;
        user = m_party->get_user(i);
        // [b28d-solo-CDeathTower] P0 清绑（N96）：ORIG 0x08467bd3 mov$1 岛无槽。
        if ((user == 0) || (int)!m_party->checkValidUser(i))
            continue;
        // ORIG 0x08467bec：EnterDungeon 第 4 参 = m_party->m_battleField.m_bloodState
        //（+0xcd8 直读；非 get_party_type 虚调，p167 EXTRA 实证）。
        user->m_historyLog.EnterDungeon(
            m_dungeonMgr.getDungeon()->GetDungeonName(), 0, memberNames,
            m_party->m_battleField.m_bloodState);
        // [b28d-solo-CDeathTower] P183：ORIG 每使用点重调 getCurCharacQuestW（×2）
        // 与 getDungeon（×2）；UserQuest +0x75d8 = m_authenData。
        user->getCurCharacQuestW()->m_authenData.reset();
        user->getCurCharacQuestW()->set_authen_data(
            (QUEST_CONDITION)0, m_dungeonMgr.getDungeon()->get_index(), -1);
    }
}

void CDeathTower::_onStartStage()
{
    m_stage.onStartStage();
}

// ---- CStage ----
// ORIG W 0x82a74ca：MapInfo ctor（编译器生成）后 resetCurrentStage()。
CDeathTower::CStage::CStage() throw()
{
    resetCurrentStage();
}

// ORIG W 0x82a74ec：仅 MapInfo dtor（编译器生成）。
CDeathTower::CStage::~CStage() throw()
{
}

void CDeathTower::CStage::reset()
{
    // ORIG 0x84614ec：MapInfo::Clear(m_mapInfo)；m_assignItemIdx/m_dropItemCnt
    // = get_rand_int(60000)；m_state = 0。
    m_mapInfo.Clear();
    m_mapInfo.m_assignItemIdx = get_rand_int(60000);
    m_mapInfo.m_dropItemCnt = get_rand_int(60000);
    m_state = (ENUM_STAGE_STATE)0;
}

void CDeathTower::CStage::onStartStage()
{
    m_startTick = GlobalData::s_systemTime_.getCurTickCount();
}

void CDeathTower::CStage::onFinishStage()
{
    m_finishTick = GlobalData::s_systemTime_.getCurTickCount();
}

int CDeathTower::CStage::getStageClearTime()
{
    return m_finishTick - m_startTick;
}

int CDeathTower::CStage::getStageMapIndex()
{
    return m_mapInfo.m_mapIndex;
}

int CDeathTower::CStage::getCurrentStage()
{
    return m_currentStage;
}

void CDeathTower::_pickupItemMoney(CUser* user, int memberCnt, const map_item& item, int* money1, int* money2)
{
    // ORIG 0x8466b02 尾 0x8466cf8..cf0 无 mov %eax 置值 —— 返回语义 void（mangled 无返回类型）。
    int money = item.m_item.get_add_info();
    int base = money / memberCnt;
    int remainder = money % memberCnt;
    int total = base + remainder;
    // ORIG 0x8466b42/8466b45：base 出循环前再拷贝一份 -0x1c（独立槽），
    // 循环内三元的 else 分支读该拷贝（0x8466bb4）而非 base 原槽。
    int per = base;

    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x8466b54 循环体首 `movl $0,-0x14`：member 显式置 0 再赋值（范式8 dead store）。
        CUser* member = 0;
        member = m_party->get_user(i);
        if (!member || !m_party->checkValidUser(i))
            continue;

        int amount = (member == user) ? total : per;
        int taxed = amount;                                // ORIG -0xc（可能翻倍）
        if (item.m_count != 0)
        {
            money2[i] = member->getCurCharacInvenR()
                            ->GetCreatureMgrR()->GetExtraGold(amount);
            taxed = taxed + taxed;  // ORIG 0x8466bf9：mov/add/mov 三段寄存器往返（4.4 对 *= 出 shl 内存形）
        }

        money1[i] = member->getCurCharacInvenW()->gain_money(
            taxed, (eMoneyAddReason)4, true, 0);
        if (item.m_count != 0)
            member->saveTaxMoneyForUpperMember(taxed);
        if (money1[i] < taxed)
        {
            if (money1[i] < 0)
                money1[i] = 0;
            member->SendMoneyFullReason(ENUM_MONEY_FULL_REASON_0,
                                        (unsigned long)taxed,
                                        (unsigned long)money1[i]);
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)1, member, (unsigned int)money1[i]);
    }

}
CUser* CDeathTower::_pickupItem(CUser* user, char* countBuf, int slot, const map_item& item, bool a5, char a6, const CItem* citem)
{
    // [b28d-solo-CDeathTower] NDIFF=153（309/327）：P167 CLEAR（17/17）；残余 = 帧/槽号
    // 整体平移 + 实参跨 call 落槽（N82/N95）。routing/计数/随机数语义全等（ORIG 0x08466cfe）。
    // ORIG T 0x08466cfe（Ghidra 反编译语义逐条转写；countBuf = 调用点 nameBuf[4] 计数）
    CUser* result = user;
    if (slot > 1)
    {
        char isFromMap = (item.m_count == 1) ? 1 : 0;          // mVar1 == 1
        int rarity = citem->get_rarity();
        if (rarity > 3)
        {
            isFromMap = a5 ? 1 : 0;
        }
        bool bEpic = false;
        if (item.m_count == 0 || !citem->isEpicRoutingItem())
        {
            bEpic = false;
        }
        else
        {
            bEpic = true;
        }
        if (bEpic)
        {
            isFromMap = a5 ? 1 : 0;
        }
        if (isFromMap && citem->check_item_routing())
        {
            result = NULL;
            int routedTurn = m_playData.getLastRoutedTurn() + 1;
            if (slot <= routedTurn)
            {
                routedTurn = 0;
            }
            int local_24 = 0;
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = m_party->get_user(i);
                if (!member || !m_party->checkValidUser(i))
                    continue;
                if (!m_playData.isMemberAlive(i))
                    continue;

                CInventory* invenR = (CInventory*)member->getCurCharacInvenR();
                int ins = invenR->tryInsertItemIntoInventory(item.m_item);
                if (ins != -1)
                {
                    countBuf[i] += 1;
                    if (local_24 == routedTurn)
                    {
                        countBuf[i] += '(';
                        m_playData.setLastRoutedTurn(local_24);
                    }
                    bool bAdd = false;
                    if (!citem->is_stackable() && citem->GetItemType() == 10)
                    {
                        bAdd = citem->check_job_type(member->get_charac_job());
                    }
                    if (bAdd)
                    {
                        countBuf[i] += 0x1e;
                    }
                    countBuf[i] += get_rand_int(100 - countBuf[i]);
                    local_24 += 1;
                }
            }
            char maxC = 0;
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = m_party->get_user(i);
                if (!member || !m_party->checkValidUser(i))
                    continue;
                if (!m_playData.isMemberAlive(i))
                    continue;

                if (maxC < countBuf[i])
                {
                    maxC = countBuf[i];
                    result = member;
                }
                else if (countBuf[i] == maxC && countBuf[i] > 0)
                {
                    countBuf[i] -= 1;
                }
            }
        }
    }
    return result;
}

// ORIG T 0x0846966c：卡片统计（每卡 AddValueStatistic 0xb/0xc + 死亡之塔
// ValueStatistic 上报，字段 8/2/7/10；isNotTower/level 参与上报参数）。
void CDeathTower::deathTowerCardStatistic(int count, CUser* user, const Inven_Item* items)
{
    // [b28d-solo-CDeathTower] ORIG 0x08469683：get_index 结果直比（无 dungeonIdx 槽）；
    // 三元 false:true 复现 `cmp; sete; test; je; movb $0; jmp; movb $1`（P47/P225）。
    bool isNotTowerOfDespair =
        (m_dungeonMgr.getDungeon()->get_index() == 0x2af8) ? false : true;
    int level = user->get_charac_level();

    for (int i = 0; i < count; ++i)
    {
        const Inven_Item* item = &items[i];   // Inven_Item 打包尺寸 0x3d（等价于 items+i*0x3d）
        int itemId = item->m_addInfo;
        if (itemId == 0)
        {
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0xb, user,
                (unsigned int)item->get_add_info());
            GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                (DEATHTOWER_VALUE_STATISTIC_FIELD)8, (char)isNotTowerOfDespair,
                (char)level, (unsigned int)item->get_add_info());
        }
        else
        {
            CItem* citem = G_CDataManager()->find_item(itemId);
            if (citem)
            {
                int price = 0;
                G_Store()->GetSellItemPrice(
                    const_cast<Inven_Item&>(*item), (const CItem*)citem,
                    (short)count, false, price);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0xc, user, (unsigned int)price);
                if (citem->GetItemType() == 3)   // ORIG vptr+0xc 虚调用 GetItemType==3
                {
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)2, (char)isNotTowerOfDespair,
                        (char)level, 1);
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)7, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)price);
                }
                else
                {
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)10, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)citem->get_rarity());
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)7, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)price);
                }
            }
        }
    }
}
bool CDeathTower::_makeDropItem(CUser* user, char type, int slot, int count, map_item& item)
{
    // [b28d-solo-CDeathTower] NDIFF=318（489/510）：P167 CLEAR（41/41）；残差 = 帧槽号
    // 平移（type 槽 -0x5c vs -0x8c）+ Inven_Item 临时槽（N82/N93）。switch 4 路 + 校验 +
    // 删除/统计语义全等（ORIG 0x0846638c）。
    // ORIG T 0x0846638c（Ghidra 反编译语义逐条转写；参数槽：0x8=this,0xc=user,
    // 0x10=type,0x14=slot,0x18=count,0x1c=item&）
    Inven_Item invenItem;   // ORIG 调 _ZN10Inven_ItemC1Ev（0x80cb854）

    switch (type)
    {
    case 0: // money or equip（ORIG 判断 slot==0 为金钱）
        if (slot == 0)
        {
            // money item（ORIG +0x4d：m_count=0 / m_field1=2 / m_addInfo=0 /
            // set_add_info(count) / m_fieldb=0 / ResetItemAttr）
            item.m_count = 0;
            item.m_item.m_field1 = 2;
            item.m_item.m_addInfo = 0;
            item.m_item.set_add_info(count);
            item.m_item.m_fieldb = 0;
            item.m_item.ResetItemAttr();
        }
        else
        {
            const CInventory* inven = user->getCurCharacInvenR();
            invenItem = inven->GetInvenSlot(1, slot);
        }
        break;
    case 2: // cargo
    {
        const CCargo* cargo = user->getCurCharacCargoR();
        invenItem = cargo->get_cargo_slot(slot);
        break;
    }
    case 3: // avatar（ORIG GetInvenSlot space=0）
    {
        const CInventory* inven = user->getCurCharacInvenR();
        invenItem = inven->GetInvenSlot(0, slot);
        break;
    }
    case 7: // creature（ORIG GetInvenSlot space=3）
    {
        const CInventory* inven = user->getCurCharacInvenR();
        invenItem = inven->GetInvenSlot(3, slot);
        if (invenItem.m_field1 == 5)          // ITEM_TYPE_CREATURE_EQUIP → 拒绝
        {
            return false;
        }
        break;
    }
    default:
        return false;
    }

    // 公共校验 + 字段填充（ORIG +0x377 起）
    int sellPrice = 0;
    if (invenItem.m_addInfo != 0)             // +0x37e：itemId 非 0 才校验
    {
        CItem* citem = G_CDataManager()->find_item(invenItem.m_addInfo);
        if (!citem)
        {
            return false;
        }
        int attachType = citem->GetAttachType();
        switch (attachType)
        {
        case 1:
        case 2:
        case 5:
            return false;
        case 3:
            if (invenItem.m_field0 != 1)
            {
                return false;
            }
            break;
        }
        if (invenItem.m_upgradeSep.IsTradeRestriction())
        {
            return false;
        }
        if (invenItem.isEquipableItemType())
        {
            // ORIG 0x84666fa：CEquipItem+0x234 = m_itemType（见 CBossStage.cpp 同款）
            const CEquipItem* equip = static_cast<const CEquipItem*>(citem);
            if (equip->m_itemType == 0xb || equip->m_itemType < 10)
            {
                return false;
            }
        }
        // 字段填充（ORIG +0x44c）
        item.m_item.m_field0 = 0;
        item.m_item.m_field1 = invenItem.m_field1;
        item.m_item.m_addInfo = invenItem.m_addInfo;
        item.m_item.m_fieldd = invenItem.m_fieldd;
        item.m_item.m_amp = invenItem.m_amp;
        item.m_item.m_fieldb = invenItem.m_fieldb;
        item.m_item.m_field0 = invenItem.m_field0;
        if (invenItem.isEquipableItemType())
        {
            item.m_item.set_add_info(count);
        }
        else
        {
            item.m_item.set_add_info(invenItem.m_addInfo2);
        }
        item.m_item.SetItemAttr(invenItem.GetItemAttr());
        G_Store()->GetSellItemPrice(invenItem, citem, (short)count, false, sellPrice);
    }

    // 删除/扣费 + 统计（ORIG +0x535 起）
    switch (type)
    {
    case 0:
        if (slot == 0)
        {
            CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
            if (invenW->use_money(count, (eMoneySubReason)6, true) != 1)
            {
                return false;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x17, user, (unsigned int)count);
        }
        else
        {
            CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
            if (invenW->delete_item((INVEN_TYPE)1, slot, count,
                                    (eItemDelReason)6, true) != 1)
            {
                return false;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        }
        break;
    case 2:
    {
        CCargo* cargoW = user->getCurCharacCargoW();
        if (cargoW->delete_item(slot, count, (eItemDelReason)0x10) != 1)
        {
            return false;
        }
        break;
    }
    case 3:
    {
        CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
        if (invenW->delete_item((INVEN_TYPE)0, slot, 1,
                                (eItemDelReason)6, true) != 1)
        {
            return false;
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        break;
    }
    case 7:
    {
        CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
        if (invenW->delete_item((INVEN_TYPE)3, slot, 1,
                                (eItemDelReason)6, true) != 1)
        {
            return false;
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        break;
    }
    default:
        return false;
    }
    return true;
}

int CDeathTower::_checkRenewMyRecord(unsigned int dungeonIdx, unsigned int memberCnt, unsigned int a, unsigned int b, unsigned int c)
{
    // [b28d-D13] P0 清绑（N96）：ORIG 0x846806d mov %eax,%ebx 为纯 register 存储建议
    // 可复现的 callee-saved 升格（P231），无 asm 绑定。
    register unsigned int towerIdx =
        WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
    return ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(
                memberCnt))->checkRenewMyRecord(towerIdx, a, b, c);
}

int CDeathTower::_checkRenewTopRecord(unsigned int dungeonIdx, unsigned int memberCnt, unsigned int a, unsigned int b)
{
    // [b28d-D13] P0 清绑（N96）：ORIG 0x8468034 mov %eax,%ebx，同 _checkRenewMyRecord。
    register unsigned int towerIdx =
        WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
    return ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(
                memberCnt))->checkRenewTopRecord(towerIdx, a, b);
}

int CDeathTower::_checkStartGameCondition(unsigned char& flag)
{
    Inven_Item items[4];
    int params[4];
    memset(items, 0, sizeof(items));
    memset(params, 0, sizeof(params));

    // [b28d-solo-CDeathTower] ORIG 0x0846392d/0x0846396b：bool callee 负极性早返
    // （P292：call; xor $1; test %al; je）；P0：getDungeon 结果内联直入实参槽。
    if (!m_party->CheckEnterAncientDungeon(m_dungeonMgr.getDungeon(), flag,
                                           items, params))
    {
        return 0x11;
    }
    if (!m_party->UseAncientDungeonItems(m_dungeonMgr.getDungeon(), items,
                                         params))
    {
        return 1;
    }
    return 0;
}
int CDeathTower::handleStartGame(int dungeonIdx, char flag, ENUM_DUNGEON_TYPE dungeonType)
{
    // [b28d-solo-CDeathTower] NDIFF=13（440/439）：B24D 判词保留（try/catch(int)+throw
    // 8/0x16/cond/0x15/1 语义全对，P167 81/81 CLEAR）；残余 = 地址占位/EH typeinfo/
    // 个别 lowering。
    // [B24D判词 N] 语义正确：try/catch(int)+throw 8/0x16/cond/0x15/1+
    //   reset+initDungeonMgr+_checkStartGameCondition+stage/map 链全对；
    //   P167 调用点差集 81/81 CLEAR；diff=地址占位+EH typeinfo+个别 lowering。
    // ORIG 0x846398e：整体 try/catch(int)；checkFlag 槽 -0x21 于函数头清零
    // （0x846399c movb $0 早于首 if）；cond -0x20 先 `movl $0`（0x8463a28）再赋值。
    unsigned char checkFlag = 0;
    (void)flag;
    try
    {
        if (dungeonType == (ENUM_DUNGEON_TYPE)1)
        {
            throw 8;
        }
        m_dungeonMgr.reset();
        if ((int)!m_dungeonMgr.initDungeonMgr(dungeonIdx))  // ORIG 0x84639f5 xor$1 值形
        {
            throw 0x16;
        }
        int cond = 0;
        cond = _checkStartGameCondition(checkFlag);
        // [b28d-D13] P0 清绑（N96）+ N 封顶：ORIG 0x8463a48 cmpl+setne %al+test+je
        // 为 4.1 对 int!=0 的 bool 物化；4.4 恒折直 je（P168a③b 同族，跨版本 bool ABI），
        // register/P226 两形实测 ndiff 13/11 均劣于裸形 9 ⇒ 保持裸形。
        if (cond != 0)
        {
            throw cond;
        }
        m_stage.reset();
        m_stage.resetCurrentStage();
        CMap* map = m_dungeonMgr.getStageMap(m_stage.getCurrentStage());
        if (map == 0)
        {
            throw 0x15;
        }
        if ((int)!m_stage.consistMap(m_dungeonMgr, map, this))  // ORIG 0x8463b11 xor$1
        {
            throw 1;
        }
        m_playData.reset();
        m_playData.makeStartMemberInfo(m_party);
        for (int i = 0; i <= 3; ++i)
        {
            // ORIG 0x8463b7a：循环体首 `movl $0,-0x14` user dead init；get_user 先行、
            // || 共享物化岛 skip（0x8463bb9/0x8463bc0，范式40 同 _onFinishDeathTower）。
            CUser* user = 0;
            user = m_party->get_user(i);
            // [b28d-D13] P0 清绑（N96）：同 _onFinishDeathTower。
            if ((user == 0) || (int)!m_party->checkValidUser(i))
                continue;
            int minLevel = m_dungeonMgr.getDungeon()->get_min_level();
            if ((bool)(user->get_charac_level() < minLevel))  // ORIG 0x8463bf4 cmp+setl+test
            {
                throw 0xe;
            }
            user->set_state((ch_state)10);
            m_playData.setMemberAlive(i, true);
            // ORIG 0x8463c5f：0xcd8 具名字段直读（无 call）；块序（范式17）：
            // !=1 的 Increment(…,false) 为 fall-through@8463c6a，==1 块殿后@8463c88。
            if (m_party->m_battleField.m_bloodState != 1)
            {
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    dungeonIdx, false);
            }
            else
            {
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    dungeonIdx, true);
            }
        }
        m_party->gen_timer_key((TIMER_MESSAGE)0x2d);
        m_party->SetSelectedEPLPCmd(-1);
        // ORIG 0x08463ce9：party 内嵌 BF（+0xb24）getRandomBuffDungeonType(0) 结果
        // 免槽直入 call arg（范式37）。
        m_party->choose_random_buff((QuickParty::RandomBuffDungeon)
            reinterpret_cast<CPartyBattleFieldView*>(m_party)
                ->m_battleField.getRandomBuffDungeonType(0));

        CPacketHandler packet;  // ORIG -0x30：析构在 try 尾（0x8463e81），lifetime 覆盖后续全部
        packet.put_header(0, 0x8e);
        packet.put_int(dungeonIdx);
        packet.put_short(m_dungeonMgr.getEndStage());
        packet.put_byte(m_party->m_battleField.m_bloodState);    // ORIG 0x8463d69：0xcd8 直读
        // 块序（范式17）：非 NULL→get_random_buff_type 为 fall-through@8463d87，
        // NULL→put 0xb 岛殿后@8463da5。
        if (m_party != NULL)
        {
            packet.put_byte(m_party->get_random_buff_type());
        }
        else
        {
            packet.put_byte(0xb);
        }
        packet.finalize(true);
        m_party->send_to_party(packet);
        packet.clear();
        packet.put_header(0, 0x8f);
        m_stage.makeStagePacket(packet);
        packet.finalize(true);
        m_party->send_to_party(packet);
        _checkMemberReady();
        setTowerState((ENUM_TOWER_STATE)2);
        _onStartDeathTower();
    }
    catch (int e)  // ORIG 0x8463e91：type selector cmp $0x1 → begin_catch；e 槽 -0xc
    {
        if (e == 0x11)  // ORIG 0x8463eab：0x11 分支 PacketGuard@-0x3c 带 checkFlag
        {
            PacketGuard g;
            g.put_header(1, 0x10);
            g.put_byte(0);
            g.put_byte(0x11);
            g.put_byte((int)checkFlag);
            g.finalize(true);
            m_party->send_to_party(g);
        }
        else            // ORIG 0x8463f68：另一 PacketGuard@-0x48 带 e
        {
            PacketGuard g;
            g.put_header(1, 0x10);
            g.put_byte(0);
            g.put_byte(e);
            g.finalize(true);
            m_party->send_to_party(g);
        }
        _destroy();
        return 0;
    }
    return 1;
}

// ---- CDungeonMgr ----
void CDeathTower::CDungeonMgr::reset()
{
    // ORIG W 0x82a7498：m_pDungeon = 0；m_endStage = 0。
    m_pDungeon = 0;
    m_endStage = 0;
}

bool CDeathTower::CDungeonMgr::initDungeonMgr(int dungeonIdx)
{
    // ORIG 0x084605a2：m_pDungeon = G_CDataManager()->find_dungeon(dungeonIdx)；
    m_pDungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (m_pDungeon == 0)
        return false;
    // [b28d-D13] P0 清绑（N96）：ORIG 0x084605e0 mov %eax,%edx 为 4.4 对
    // getDeathTowerMaxStage() 返回值存 int 字段的自然中转，直接赋值即复现。
    m_endStage = m_pDungeon->getDeathTowerMaxStage();
    return true;
}

CMap* CDeathTower::CDungeonMgr::getStageMap(unsigned int stage)
{
    // ORIG 0x84605ee：mapIdx = m_pDungeon->getDeathTowerMapIndex(stage)；
    // map = G_CDataManager()->find_map(mapIdx)；为空返回 0，否则返回 map。
    int mapIdx = m_pDungeon->getDeathTowerMapIndex(stage);
    CMap* map = (CMap*)G_CDataManager()->find_map(mapIdx);
    if (map == 0)
        return 0;
    return map;
}

int CDeathTower::CDungeonMgr::getEndStage()
{
    // ORIG W 0x8469ab4：返回 m_endStage（+0x04）。
    return m_endStage;
}

int CDeathTower::handleFinishLoading(CUser* user)
{
    // ORIG 0x8466024：正向 && 短路（cmpl$-1;je ERR / call valid;test al;je ERR
    // 无 xor），ERR=return 0 尾岛，body fall-through（范式17）。
    int slot = m_party->GetMemberSlotNo(user);
    if (slot != -1 && m_party->checkValidUser(slot))
    {
        if (m_playData.checkEnterStartMap(slot))
        {
            user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);
        }
        m_playData.setMemberReady(slot, true);

        // ORIG 0x84660a3：get_member_count 结果免槽直入 arg（0x84660b1 `mov %eax,0x4(%esp)`）。
        if (m_playData.checkAllMemberReady(m_party->get_member_count()))
        {
            CPacketHandler packet;
            packet.makeFinishLoading();    // ORIG：全员 ready 后广播加载完成包
            m_party->send_to_party(packet);
        }
        return 1;
    }
    return 0;
}

int CDeathTower::handleDieUser(CUser* user)
{
    // [b28d-solo-CDeathTower] NDIFF=56（102/106）：循环变量双置 0 槽 -0xc vs -0x10 +
    // 槽号平移（N93/P197③）；P167 CLEAR。语义（alive 判定/死亡包/_checkMemberDie）全等。
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
        {
            if (!m_playData.isMemberAlive(i))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
                return 0;
            }
            m_playData.setMemberAlive(i, false);

            CPacketHandler packet;
            unsigned short uid = user->get_unique_id();
            packet.makeDieState(uid);    // ORIG：死亡状态包
            m_party->send_to_party(packet);

            if (_checkMemberDie())
            {
                _onFinishStage();
                _onPrepareFinishTower();
            }
            return 1;
        }
    }
    return 1;
}

// [b28d-solo-CDeathTower] NDIFF=21（130/131），P167 CLEAR：残差 = item 槽 -0x6c vs
// -0x74 的帧平移 + 相邻局部槽号整体错位（N82/P112 族；范式31 dead-local 已试，
// ORIG item@-0x6c 与 4.4 -0x74 差 8 属帧继承）。语义/调用点全等。
int CDeathTower::handleDropItem(CUser* user, int a2, int a3, char a4, int a5, int a6)
{
    if (a6 < 0)
    {
        return 0;
    }

    unsigned int padA;  // 范式31：试验推移 item 槽（ORIG item@-0x6c vs 4.4 -0x70）
    (void)padA;
    map_item item;
    CPacketHandler packet;

    if (!_makeDropItem(user, a4, a5, a6, item))
    {
        packet.makeDropItemError((ENUM_CMDPACKET)0x32, (ENUM_ERROR)0x11, a4);
        user->Send(packet);
        return 0;
    }

    int dropIdx = m_stage.dropItem(item);
    if (dropIdx == -1)
    {
        packet.makeDropItemError((ENUM_CMDPACKET)0x32, (ENUM_ERROR)0x16, a4);
        user->Send(packet);
        return 0;
    }

    unsigned short idx16 = (unsigned short)dropIdx;
    packet.makeDropItemNotify((unsigned short)user->get_unique_id(), a2, a3, idx16, item);
    m_party->send_to_party(packet);

    packet.clear();
    packet.makeDropItem(a4, a5, a6);
    user->Send(packet);
    return 1;
}

int CDeathTower::CStage::killMonster(int idx, map_monster& monster)
{
    // [b28d-solo-CDeathTower] NEAR 10（62/62）：const_iterator 槽序 N93/N98 封顶
    // （探针 11 形实测，4.4 恒后声明深槽）；P167 CLEAR。ORIG 0x08461708。
    // ORIG 0x8461708：find(idx)；命中则 monster = it->second、取
    // m_level(+0x10) 作返回值并 erase；未命中返回 0。
    // 注：ORIG movzbl 0x14(%pair) = pair+0x14 = map_monster+0x10 = m_level
    //（map_monster 为 pack(1)，second @ pair+4）。
    // ORIG 0x8461731/0x846176b：find/end 各经 _Rb_tree_const_iterator C1(iter&)
    //（0x81529e0 两次）⇒ const_iterator 具名；erase@0x8152a12 arg=&idx(key)。
    // ORIG 0x846170e：roleType 槽 -0xc 于函数头 `movl $0` ⇒ 声明先于迭代器（P0 时机）。
    int roleType = 0;
    std::map<int, map_monster>::const_iterator it = m_mapInfo.m_monsterMap.find(idx);
    std::map<int, map_monster>::const_iterator itEnd = m_mapInfo.m_monsterMap.end();
    if (it == itEnd)
    {
        return 0;
    }
    monster = it->second;
    roleType = (unsigned char)it->second.m_level;
    m_mapInfo.m_monsterMap.erase(idx);
    return roleType;
}

bool CDeathTower::CStage::peekItem(int idx, map_item& item)
{
    // [b28d-solo-CDeathTower] NEAR 9（59/59）：find/end 两 const_iterator 槽序
    // it=-0x28/itEnd=-0x20（ORIG）vs -0x24/-0x28（4.4）——N93/N98 封顶；P167 CLEAR。
    // ORIG 0x8461534：m_mapInfo.m_itemMap.find(idx)；命中拷贝 0x54 字节返回 true。
    // ORIG 0x8461569/0x8461593：find/end 结果各经 _Rb_tree_const_iterator C1(iter&)
    // 转换（0x8152fa2 两次 call）⇒ 源以 const_iterator 具名持有。
    std::map<int, map_item>::const_iterator it = m_mapInfo.m_itemMap.find(idx);
    std::map<int, map_item>::const_iterator itEnd = m_mapInfo.m_itemMap.end();
    if (it == itEnd)
    {
        return false;
    }
    item = it->second;
    return true;
}

int CDeathTower::handleLeaveUser(CUser* user)
{
    // ORIG 0x8465aca/0x8465ad1：i 双次置 0 直落 -0xc（范式8）；循环内
    // get_user==user 用 sete %al 直出（范式22，ORIG 0x8465b09）；
    // allDead 物化于 %eax 无槽（0x8465c39/0x8465c40 mov$1/$0 岛）。
    int i = 0;
    i = 0;
    for (; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            // ORIG 0x8465b06：cmp 0xc(%ebp),%eax; sete %al; test al; je skip（范式22）。
            // N96 清绑：具名 bool 槽化反劣（帧 0x20→0x30）⇒ call 结果直比
            // （P197④：结果寄存器直比出 sete 形）。
            if (m_party->get_user(i) == user)
            {
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            m_playData.setMemberAlive(i, false);
            }
        }
    }

    G_GameWorld()->out_from_dungeon(user);

    // [b28d-solo-CDeathTower] p167：ORIG 用嵌套 CPacketHandler（C1 ×1 / D1 ×2）。
    CPacketHandler packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, 0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);

    onLeaveUser(user);

    // N96 清绑：ORIG 0x8465c39/0x8465c40 mov$1/$0 岛物化于 %eax 无槽（P0 无具名
    // 局部）⇒ 短路条件内联直测。
    if (m_party->get_member_count() > 0 && _checkMemberDie())
    {
        _onFinishStage();
        _onPrepareFinishTower();
    }
    return 1;
}

// [b28d-solo-CDeathTower] NDIFF=164（252/249）。P167：MISSING _Unwind_Resume x4——
// ORIG 各 CPacketHandler 局部（try 内多个 return 路径）各带 landing pad，4.4 判同 TU
// callee nothrow 合并 cleanup（N91/N104 族：EH 块数量/相对序差异，D1 调用次数已匹配）。
// 语义/异常路由（catch(int) → makeCommonError(0x30,e)）与 ORIG 一致。
int CDeathTower::handleMoveMap()
{
    try
    {
        if (_checkMemberDie())
        {
            CPacketHandler packet;
            packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x16);
            m_party->send_to_party(packet);
            return 0;
        }

        if (m_stage.getState() < 2)
        {
            CPacketHandler packet;
            packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x13);
            m_party->send_to_party(packet);
            return 0;
        }

        unsigned int endStage = m_dungeonMgr.getEndStage();
        if (!m_stage.moveNextStage(endStage))
        {
            CPacketHandler packet;
            packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x16);
            m_party->send_to_party(packet);
            return 0;
        }

        CMap* map = m_dungeonMgr.getStageMap(m_stage.getCurrentStage());
        if (!map)
        {
            throw 0x15;
        }

        m_stage.reset();
        if (!m_stage.consistMap(m_dungeonMgr, map, this))
        {
            throw 1;
        }

        CPacketHandler packet;
        packet.put_header(0, 0x8f);
        m_stage.makeStagePacket(packet);
        packet.finalize(true);
        m_party->send_to_party(packet);

        _checkMemberReady();
        return 1;
    }
    catch (int e)
    {
        // ORIG：catch 后广播 makeCommonError(0x30, e)
        CPacketHandler packet;
        packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)e);
        m_party->send_to_party(packet);
        return 0;
    }
}

// [b28d-solo-CDeathTower] NDIFF=145（400/418），P167 CLEAR（44/44）：残差 = 4.4 帧
// 0x… 槽号整体平移 + _pickupItem/appendPickupItem 实参跨 call 落槽 vs ORIG
// callee-saved（N82/N95/N7 族）。语义（金钱/物品/avatar 三路 + 统计）全等。
int CDeathTower::handlePickupItem(CUser* user, int idx, bool a3, char a4)
{
    map_item item;
    CPacketHandler packet;

    if (!m_stage.peekItem(idx, item))
    {
        if (a4 > 3)
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x51);
        else
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
        user->Send(packet);
        return 0;
    }

    int memberCnt = m_party->get_member_count();
    if (memberCnt < 1)
    {
        packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
        user->Send(packet);
        return 0;
    }

    // ORIG：map_item+0x12 = Inven_Item.m_addInfo（item idx）判定金钱物品。
    if (item.m_item.m_addInfo == 0)
    {
        // Money item
        if (!m_stage.pickupItem(idx, item))
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
            user->Send(packet);
            return 0;
        }
        int moneyA[4] = {0};
        int moneyB[4] = {0};
        _pickupItemMoney(user, memberCnt, item, moneyA, moneyB);
        packet.makePickupItemHeader(user->get_unique_id(), (unsigned short)idx);
        packet.appendPickupMoney(moneyA, moneyB, m_party);
    }
    else
    {
        CItem* citem = G_CDataManager()->find_item(item.m_item.m_addInfo);
        if (!citem)
        {
            return 0;
        }

        int itemType = 0;
        if (citem->is_stackable())
        {
            if (citem->GetItemType() == 9)
            {
                itemType = 7;
            }
        }
        else
        {
            if (citem->IsCreatureItem())
            {
                itemType = 7;
            }
            else
            {
                if (citem->IsAvatarItem())
                {
                    itemType = 1;
                }
            }
        }

        char ipgNo[0x18] = {0};
        int nameBuf = 0;              // ORIG -0x4c：_pickupItem/appendPickupItem 的 char*
        CUser* targetUser = _pickupItem(
            user, (char*)&nameBuf, memberCnt, item, a3, a4, citem);
        if (!targetUser)
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x16);
            user->Send(packet);
            return 0;
        }

        int slot = -1;
        Inven_Item* invenItem = &item.m_item;
        if (invenItem->isAvatarItemType())
        {
            int characNo = targetUser->getCurCharacNo();
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5, characNo, ipgNo);
            slot = targetUser->addAvatarItem(
                item.m_item.m_addInfo, 0, (char)item.m_item.m_fieldb,
                ipgNo, (eAvatarItemAddReason)0x10);
        }
        else
        {
            CInventory* inven = (CInventory*)targetUser->getCurCharacInvenW();
            slot = inven->insertItemIntoInventory(*invenItem, (eItemAddReason)4, true, true);
        }

        if (slot == -1)
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)4);
            user->Send(packet);
            return 0;
        }

        if (citem->IsRandomOption())
        {
            statistc_proxy::add(
                targetUser, "log_random_option", "pick_cnt", 1, "%d,'%s'",
                targetUser->get_charac_10_level_section(),
                GlobalData::s_systemTime_.getCurDate());
        }

        if (!m_stage.pickupItem(idx, item))
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
            user->Send(packet);
            return 0;
        }

        packet.makePickupItemHeader(user->get_unique_id(), (unsigned short)idx);
        packet.appendPickupItem((char*)&nameBuf, targetUser->get_unique_id(),
                                slot, itemType);
    }

    m_party->send_to_party(packet);
    return 1;
}

int CDeathTower::handleSelectEPLPCommand(CUser* user, int cmd)
{
    // ORIG 0x8466208：slot 具名槽 -0xc（0x8466224）；&& 物化岛共享 mov$1/$0
    // （0x8466245/0x846624c）+ test/je 反判（范式22/40）；key/idx 缓存 ebx/esi。
    int slot = m_party->GetMemberSlotNo(user);
    // ORIG 0x8466227/0x8466241：正向 && 短路（call valid 结果直 test al; je ERR，
    // 无 xor），ERR=return 0 尾岛@84662c0，body fall-through（范式17）。
    if (slot != -1 && m_party->checkValidUser(slot))
    {
        m_party->SetEPLPState(3);
        // [b28d-D13] P0 清绑（N96）：key 内联实参（P24）。
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4e, cmd,
            _genTimerKey((TIMER_MESSAGE)0x4e), 0x4e);
        return 1;
    }
    return 0;
}
int CDeathTower::handleStageCommand(CUser* user, unsigned char cmd)
{
    if (m_party->getManager() != user)
    {
        return 0;
    }
    // ORIG 0x846617d：switch 下探（movzbl -0xc;cmp$1 je;cmp$2 je;jmp default@84661f8）。
    switch (cmd)
    {
    case 1:
        if ((int)!m_stage.setState((ENUM_STAGE_STATE)1))  // ORIG 0x84661a3：xor $0x1,%eax; test; je
        {
            return 1;
        }
        if (_checkMemberDie())
        {
            return 1;
        }
        _onStartStage();
        break;
    case 2:
        if ((int)!m_stage.setState((ENUM_STAGE_STATE)2))  // ORIG 0x84661ea
        {
            return 1;
        }
        break;
    default:
        return 0;  // ORIG 0x84661f8
    }
    return 1;
}

int CDeathTower::CStage::getState() { return m_state; }
bool CDeathTower::CStage::checkClearStage()
{
    // [b28d-solo-CDeathTower] ORIG 0x8461b3e：局部 map_monster（默认构造 local_360，
    // 带 EH cleanup）+ 每轮 operator= 拷贝（p167：C1 ×1 / operator= ×1 / D1 ×2 /
    // _Unwind_Resume ×1 实证）；m_dropCnt(+0x2c)==100 且 m_field13(+0x13)==0 → false。
    map_monster mob;
    for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
         it != m_mapInfo.m_monsterMap.end(); ++it)
    {
        mob = it->second;
        if (mob.m_dropCnt == 100 && mob.m_field13 == 0)
        {
            return false;
        }
    }
    return true;
}


bool CDeathTower::CStage::setState(ENUM_STAGE_STATE state)
{
    // ORIG 0x8461c5d：switch 下探形态（范式34）：mov 0xc,%eax; cmp$1 je;cmp$2 je;jmp default。
    switch (state)
    {
    case 1:  // STAGE_READY?（枚举值 1）：非 0 拒绝（ORIG 0x8461c6c）
        if (m_state != 0)
        {
            return false;
        }
        break;
    case 2:  // 非 1 拒绝（ORIG 0x8461c80）
        if (m_state != 1)
        {
            return false;
        }
        break;
    default:
        return false;  // ORIG 0x8461c95
    }
    m_state = state;
    return true;
}

bool CDeathTower::CStage::moveNextStage(unsigned int endStage)
{
    // ORIG 0x84617da：checkClearStage() 且 m_currentStage < endStage 时
    // ++m_currentStage 返回 true，否则 false。
    // [b28d-D13] P0 清绑（N96）：ORIG 0x84617eb xor $0x1 + mov$1/$0 物化由裸短路
    // 表达式自然产生，无绑定。
    if (((int)checkClearStage() ^ 1) || (unsigned int)m_currentStage >= endStage)
        return false;
    ++m_currentStage;
    return true;
}

bool CDeathTower::CStage::pickupItem(int idx, map_item& item)
{
    // [b28d-solo-CDeathTower] NEAR 9（65/65）：同 peekItem，const_iterator 双槽序
    // N93/N98 封顶；P167 CLEAR。ORIG 0x084615e4。
    // ORIG 0x84615e4：peek 语义 + erase。
    // ORIG 0x8461619/0x8461643：find/end 各经 _Rb_tree_const_iterator C1(0x8152fa2)
    // 转换 ⇒ const_iterator 具名；erase@0x8152c18 arg=&idx（key 版）。
    std::map<int, map_item>::const_iterator it = m_mapInfo.m_itemMap.find(idx);
    std::map<int, map_item>::const_iterator itEnd = m_mapInfo.m_itemMap.end();
    if (it == itEnd)
    {
        return false;
    }
    item = it->second;
    m_mapInfo.m_itemMap.erase(idx);
    return true;
}

int CDeathTower::CStage::dropItem(map_item& item)
{
    // ORIG 0x84616a8：idx = m_mapInfo.m_dropItemCnt++；item.m_itemIndex = idx；
    // MapInfo::Add_Item(m_mapInfo, item)（按值拷贝入参）；返回 idx。
    int idx = m_mapInfo.m_dropItemCnt++;
    item.m_itemIndex = idx;
    m_mapInfo.Add_Item(item);
    return idx;
}

// [b28d-69] P167 CLEAR（ORIG calls=134 = OUR 134）：补 m_mapInfo.m_mapIndex =
// map->get_index()（ORIG 0x0846063f）；去 aiList 缓存（P183：G_CDataManager 每使用点重调）；
// push_back 右值化（P270 EOj）；find 第三参 cast unsigned；getAppearancePoint /
// getStandardLevel / getItemGenerator 重调（P48 无 CSE）；mons/items 去 const
// （非 const begin/end + _List_const_iterator 转换 C1×2）；specialItems 改 const 指针
// （const at）；第四段循环条件改 m_party->m_battleField.m_bloodState!=1
//（消 get_party_type EXTRA，ORIG 0x0846136d 实证）+ _mapItem src 具名局部（C1 符号）。
bool CDeathTower::CStage::consistMap(const CDungeonMgr& dungeonMgr, const CMap* map,
                                     CDeathTower* tower)
{
    // ORIG 0x8460634：mobIdVec + apMap（拷贝自 aiList->getAPCount()）+
    // map_monster mob 局部，随后按三段填充：AI 布点 → AP 随机怪 → 脚本怪列表
    // → 特殊被动物品生成。tower 参数供第四段判 partyType（ORIG 0x8461371）。
    // [b28d-69] ORIG 0x0846063f：*(this+0x10)=m_mapInfo.m_mapIndex = map->get_index()；
    // G_CDataManager 每次使用点重调（p167 差集 -4 实证），禁缓存 aiList（P183）。
    m_mapInfo.m_mapIndex = map->get_index();
    std::vector<unsigned int> mobIdVec;
    std::map<int, int> apMap(G_CDataManager()->m_aiCharacterList->getAPCount());

    int mobIdx = 0;
    map_monster mob;
    const std::vector<STAICharacterMapArrangeData>* aiChars = map->getAICharacters();

    // ---- 第一段：图上 AI 布点 ----
    if (!aiChars->empty())
    {
        for (std::vector<STAICharacterMapArrangeData>::const_iterator it =
                 aiChars->begin(); it != aiChars->end(); ++it)
        {
            const STAICharacterMapArrangeData& data = *it;
            CAICharacter* aiChar =
                G_CDataManager()->m_aiCharacterList->get(data.m_type);
            if (!aiChar)
                return false;

            mobIdVec.push_back(aiChar->getIdx());

            if (aiChar->getAppearancePoint() != 0)
            {
                std::map<int, int>::iterator it2 =
                    apMap.find(aiChar->getAppearancePoint());
                if (it2 != apMap.end())
                    it2->second = std::max(it2->second - 1, 0);
            }

            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = mobIdx;
            mob.m_mobIndex = data.m_type;
            mob.m_instId = m_mapInfo.m_assignItemIdx;
            m_mapInfo.m_assignItemIdx++;
            mob.m_roleType = 5;
            mob.m_level = aiChar->getLevel();
            mob.m_dropCnt = data.m_camp;
            m_mapInfo.Add_Mob(mob);
            mobIdx++;
        }
    }

    // ---- 第二段：AP 随机怪（剩余点数预算扣减；ORIG 0x8460940..0x8460d28）----
    // ORIG 结构：do { while(1){ [a]剩余<minAP→结束; [b]随机取怪;
    // [c]find→已在 mobIdVec(本段或首段已布)→break; [d]新怪:入 vec/扣点/建怪 }
    // [e]统计 ≤剩余点数的可用 AP 总量; } while(total != 0);
    int apRandomPoint = map->getAPCRandomPoint();
    if (apRandomPoint != 0)
    {
        mobIdx = 0x40;
        for (;;)
        {
            if (G_CDataManager()->m_aiCharacterList->getMinAP() > apRandomPoint)
                break;                      // [a]

            CAICharacter* aiChar =
                G_CDataManager()->m_aiCharacterList->getByAP(
                    static_cast<unsigned int>(get_rand_int(apRandomPoint) + 1));
            if (!aiChar)
                return false;

            std::vector<unsigned int>::iterator fi = std::find(
                mobIdVec.begin(), mobIdVec.end(),
                (unsigned int)aiChar->getIdx());
            if (fi != mobIdVec.end())
            {
                // [e] 抽中已布过的怪：统计可用 AP 总量，为 0 才结束
                int total = 0;
                for (std::map<int, int>::const_iterator ci = apMap.begin();
                     ci != apMap.upper_bound(apRandomPoint); ++ci)
                    total += ci->second;
                if (total == 0)
                    break;
                continue;
            }

            // [d] 新怪：入已用表、扣减出现点数、建怪（m_dropCnt=100 ORIG 立即数）
            mobIdVec.push_back(aiChar->getIdx());
            std::map<int, int>::iterator it2 =
                apMap.find(aiChar->getAppearancePoint());
            if (it2 != apMap.end())
                it2->second = std::max(it2->second - 1, 0);
            apRandomPoint -= aiChar->getAppearancePoint();

            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = mobIdx;
            mob.m_mobIndex = aiChar->getIdx();
            mob.m_instId = m_mapInfo.m_assignItemIdx;
            m_mapInfo.m_assignItemIdx++;
            mob.m_roleType = 5;
            mob.m_level = aiChar->getLevel();
            mob.m_dropCnt = 100;
            m_mapInfo.Add_Mob(mob);
            mobIdx++;
        }
    }

    // ---- 第三段：脚本怪列表（ORIG 0x8460d28..0x8460f74）----
    mobIdx = 0;
    std::vector<int> namedVec;
    // ORIG 非 const 局部（按值拷贝）+ const_iterator 隐式转换（p167 差集
    // list<_mapMonster>::begin/end 非 const + _List_const_iterator C1×2 实证）。
    std::list<_mapMonster> mons = map->get_monster_list();
    for (std::list<_mapMonster>::const_iterator it = mons.begin();
         it != mons.end(); ++it)
    {
        const _mapMonster& src = *it;
        memset(&mob, 0, sizeof(mob));
        mob.m_mobId = mobIdx;

        // ORIG 0x8460dc5：flag04==0 → 直用 m_field08 低字节；
        // 否则全宽 int 加法后截断 uchar：0→1，>200→200，1..200 原样
        if (src.m_flag04 != 0)
        {
            unsigned int lv = (unsigned char)(dungeonMgr.getStandardLevel() +
                                              src.m_field08);
            if (lv == 0)
                lv = 1;
            else if (lv > 200)
                lv = 200;
            mob.m_level = static_cast<unsigned char>(lv);
        }
        else
        {
            mob.m_level = static_cast<unsigned char>(src.m_field08);
        }

        if (src.m_mobId == -1)
            mob.m_mobIndex =
                G_CDataManager()->m_monsterManager->get_levelrandom_mob(
                    mob.m_level);
        else
            mob.m_mobIndex = src.m_mobId;

        // ORIG 分别写 +0x11/+0x12 两字节 = m_field11 半字（小端拼装）
        mob.m_field11 = static_cast<short>(
            (static_cast<unsigned short>(
                static_cast<unsigned char>(src.m_levelAdd)) |
             (static_cast<unsigned short>(
                static_cast<unsigned char>(src.m_field14)) << 8)));
        mob.m_roleType = static_cast<char>(
            static_cast<unsigned char>(src.m_roleType));
        mob.m_instId = m_mapInfo.m_assignItemIdx;
        m_mapInfo.m_assignItemIdx++;

        if (mob.m_roleType == 0 && !mob.isNamedMonster())
            namedVec.push_back(mob.m_instId);

        mob.m_field13 = static_cast<unsigned char>(src.m_champion);
        mob.m_field14 = static_cast<unsigned char>(src.m_posX);
        mob.m_dropCnt = src.m_hpPct;
        m_mapInfo.Add_Mob(mob);
        mobIdx++;
    }

    // ---- 第四段：特殊被动物品生成（ORIG 0x8460fdd..0x846133f）----
    _mapItem src;
    const std::vector<stDungeonAssignItem_t>* specialItems =
        dungeonMgr.getSpecialPassiveObjectItems();
    stGenerateRefData_t refData1;
    stGenerateRefData_t refData2;
    dungeonMgr.getObjectDropItems();   // ORIG 取出未用（忠实保留）
    std::list<_mapItem> items = map->get_item_list();

    for (std::list<_mapItem>::iterator it = items.begin();
         it != items.end(); ++it)
    {
        // ORIG 0x0846136d：循环条件 tower->m_party->m_battleField.m_bloodState!=1
        //（单人则停止本图特殊被动物品生成）；ORIG 无 get_party_type 调用（P167 EXTRA）。
        if (tower->m_party->m_battleField.m_bloodState == 1)
        {
            break;
        }

        src = *it;
        const stDungeonAssignItem_t& assign =
            specialItems->at(static_cast<unsigned int>(src.m_objectIdx));

        if (assign.m_assignItemIdx != -1)
        {
            *reinterpret_cast<char*>(&refData1.m_rankBonusIndex) =
                static_cast<char>(assign.m_assignItemIdx);
        }
        else
        {
            // ORIG 0x084610c0..0x084610f5：getStandardLevel 三次重调（P48 无 CSE）
            if (dungeonMgr.getStandardLevel() < 1 ||
                dungeonMgr.getStandardLevel() > 0xc8)
                continue;
            *reinterpret_cast<char*>(&refData1.m_rankBonusIndex) =
                static_cast<char>(dungeonMgr.getStandardLevel());
        }
        // ORIG 0x84610fb..0x846111b：refData1+0x39 = src.m_field04 低字节
        //（+0x38/+0x39 即 m_rankBonusIndex 的低两字节，ORIG 均为 movb 单字节写：
        //  084610ad/084610f5 写 +0x38、0846110f 写 +0x39）
        *(reinterpret_cast<char*>(&refData1.m_rankBonusIndex) + 1) =
            static_cast<char>(src.m_field04);

        stGenerateResult_t result;
        G_CDataManager()->getItemGenerator()
            ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateRandomItem(refData1, result);
        G_CDataManager()->getItemGenerator()
            ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
            ->generateSpecificItem(refData2, result);

        for (std::vector<Inven_Item>::iterator d = result.m_dropItems.begin();
             d != result.m_dropItems.end(); ++d)
        {
            map_item drop;
            drop.m_count = 1;
            drop.m_itemIndex = m_mapInfo.m_dropItemCnt;
            m_mapInfo.m_dropItemCnt++;
            drop.m_standardLevel = static_cast<char>(src.m_field0c);
            drop.m_item = *d;
            m_mapInfo.Add_Item(drop);
        }
    }

    return true;
}

int CDeathTower::handleUseStackable(CUser* user, ENUM_ITEMSPACE itemSpace, unsigned short slot)
{
    const CInventory* inven = user->getCurCharacInvenR();
    const Inven_Item* item = inven->GetInvenRef(
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace), slot);
    if (!item)
    {
        return 0;
    }

    if (m_dungeonMgr.getDungeon()->limitOfStackableItemInTower() &&
        ((item->m_addInfo < 0x1964 || item->m_addInfo > 6999) &&
         item->m_addInfo != 0x18))
    {
        unsigned int itemId = (unsigned int)item->m_addInfo;
        // [b28d-solo-CDeathTower] ORIG 0x08466351 起为 4.1 二分决策树
        // （cmp$0x18 je / ja / cmp$0xa je / …）⇒ 源形是 switch（P46/P163），
        // case 按数值升序书写；case 0x18 是 ORIG 保留的不可达 case。
        switch (itemId)
        {
        case 0xa:
        case 0x18:
        case 0x33:
        case 0x28978a:
            break;
        default:
            return 0;
        }
    }
    return 1;
}

int CDeathTower::onTimer(TIMER_MESSAGE msg, unsigned int key)
{
    if (!_checkTimerKey(msg, key))
    {
        return 0;
    }

    if (getTowerState() == 0)
    {
        return 0;
    }

    // [b28d-solo-CDeathTower] ORIG 0x08462b02..0x08463630：线性 cmp/jne 链
    //（cmpl $0x49/0x4b/0x4c/0x4d/0x4e/0x4a,0xc(%ebp)）⇒ 源形是 if-else 链
    //（P17/P251：switch 在 4.4 生跳表+范围检查）。顺序照 ORIG：49,4b,4c,4d,4e,4a。
    if (msg == (TIMER_MESSAGE)0x49)
    {
        int memberCnt = m_party->get_member_count();
        if (memberCnt == 0)
        {
            _destroy();
            return 1;
        }

        bool someoneLeft = false;
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                if (!m_playData.isMemberReady(i))
                {
                    // ORIG 0x08462bca：makeFinishLoading 包后单发该用户再请离
                    CPacketHandler packet;
                    packet.makeFinishLoading();
                    user->Send(packet);
                    handleLeaveUser(user);
                    someoneLeft = true;
                }
            }
        }

        memberCnt = m_party->get_member_count();
        if (!someoneLeft || memberCnt < 1)
        {
            if (memberCnt == 0)
            {
                _destroy();
            }
        }
        else
        {
            for (int i = 0; i < 4; ++i)
            {
                m_playData.setMemberReady(i, true);
            }
            // ORIG 0x08462ca0：全员置 ready 后广播 makeFinishLoading
            CPacketHandler packet;
            packet.makeFinishLoading();
            m_party->send_to_party(packet);
        }
    }
    else if (msg == (TIMER_MESSAGE)0x4b)
    {
        if (getTowerState() < 3)
        {
            return 0;
        }
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                CPacketHandler packet;
                Inven_Item* item3 = m_playData.getRewardItem(3);
                int cnt3 = m_playData.getRewardItemCount(3);
                Inven_Item* item2 = m_playData.getRewardItem(2);
                int cnt2 = m_playData.getRewardItemCount(2);
                Inven_Item* item1 = m_playData.getRewardItem(1);
                int cnt1 = m_playData.getRewardItemCount(1);
                Inven_Item* item0 = m_playData.getRewardItem(0);
                int cnt0 = m_playData.getRewardItemCount(0);
                int exp = m_playData.getRewardExp(i);
                // ORIG 0x08462ede：makeDisplayReward(exp, 槽0..3 的 cnt/items)
                packet.makeDisplayReward(exp, cnt0, item0, cnt1, item1, cnt2,
                                         item2, cnt3, item3);
                Inven_Item* myItem = m_playData.getRewardItem(i);
                int myCnt = m_playData.getRewardItemCount(i);
                deathTowerCardStatistic(myCnt, user, myItem);
                user->Send(packet);
            }
        }
        setTowerState((ENUM_TOWER_STATE)4);
        int tkey = _genTimerKey((TIMER_MESSAGE)0x4c);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4c, 5, tkey, 0x4c);
    }
    else if (msg == (TIMER_MESSAGE)0x4c)
    {
        if (getTowerState() < 4)
        {
            return 0;
        }
        bool allHaveItem = true;
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                Inven_Item item;
                const CInventory* inven = (const CInventory*)user->getCurCharacInvenR();
                if (inven->GetInvenData(0x1057, item) < 0)
                {
                    allHaveItem = false;
                    break;
                }
            }
        }

        // ORIG 0x084630da：makeDisplayEPLP(allHaveItem) 后广播
        CPacketHandler packet;
        packet.makeDisplayEPLP(allHaveItem);
        m_party->send_to_party(packet);

        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                int rewardCnt = m_playData.getRewardItemCount(i);
                Inven_Item* rewardBase = m_playData.getRewardItem(i);
                for (int j = 0; j < rewardCnt; ++j)
                {
                    // ORIG：rewardBase 为成员奖励数组，逐 j 项发放（m_addInfo +0x02）
                    if (rewardBase[j].m_addInfo == -1)
                    {
                        continue;
                    }
                    int slot = -1;
                    if (rewardBase[j].m_addInfo == 0)
                    {
                        // 金钱奖励：金额 = rewardBase[j].get_add_info()（ORIG 三次调用）
                        CInventory* inven = (CInventory*)user->getCurCharacInvenW();
                        int gained = inven->gain_money(
                            rewardBase[j].get_add_info(), (eMoneyAddReason)5, true, 0);
                        if (gained < rewardBase[j].get_add_info())
                        {
                            user->SendMoneyFullReason(
                                (ENUM_MONEY_FULL_REASON)0,
                                rewardBase[j].get_add_info(), gained);
                        }
                        slot = 0;
                        CDungeon* dungeon = m_dungeonMgr.getDungeon();
                        int dungeonIdx = dungeon->get_index();
                        if (dungeonIdx == 11000)
                        {
                            GetInstanceCirculationStatistic()->AddCirculationStatistic((CIRCULATION_STATISTIC_FIELD)4, user, (unsigned int)gained);
                        }
                        else
                        {
                            GetInstanceCirculationStatistic()->AddCirculationStatistic((CIRCULATION_STATISTIC_FIELD)5, user, (unsigned int)gained);
                        }
                    }
                    else
                    {
                        CInventory* inven = (CInventory*)user->getCurCharacInvenW();
                        slot = inven->insertItemIntoInventory(rewardBase[j], (eItemAddReason)0xb, true, true);
                    }

                    if (slot >= 0)
                    {
                        // ORIG 0x08463394：GetItemSpace + makeUpdateItemList 单发
                        ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)rewardBase[j].GetItemSpace();
                        CPacketHandler updatePacket;
                        updatePacket.makeUpdateItemList(user, space, slot);
                        user->Send(updatePacket);
                    }
                }

                // ORIG 0x084635b3：calcHelpAbuseRatio(exp, false)；gain_exp_sp 另取
                user->calcHelpAbuseRatio(m_playData.getRewardExp(i), false);
                int a = 0, b = 0;
                if (user->gain_exp_sp(m_playData.getRewardExp(i), a, b,
                                      (CUser::eExpAddReason)2, 0, false))
                {
                    user->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
            }
        }

        setTowerState((ENUM_TOWER_STATE)5);
        m_party->SetEPLPState(2);
        int tkey = _genTimerKey((TIMER_MESSAGE)0x4d);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4d, 0x3c, tkey, 0x4d);
    }
    else if (msg == (TIMER_MESSAGE)0x4d)
    {
        if (m_party->GetEPLPState() != 2)
        {
            return 0;
        }
        if (m_party->GetSelectedEPLPCmd() == 3)
        {
            return 0;
        }
        _onFinishDeathTower();
    }
    else if (msg == (TIMER_MESSAGE)0x4e)
    {
        if (m_party->GetEPLPState() != 3)
        {
            return 0;
        }
        _onFinishDeathTower();
    }
    else if (msg == (TIMER_MESSAGE)0x4a)
    {
        if (getTowerState() < 2)
        {
            return 0;
        }
        if (m_party->get_member_count() < 1)
        {
            _destroy();
            return 1;
        }
        if (_checkMemberDie())
        {
            _beginTowerClearProcess(false);
        }
        else
        {
            int curStage = m_stage.getCurrentStage();
            int endStage = m_dungeonMgr.getEndStage();
            if (curStage == endStage)
            {
                _beginTowerClearProcess(true);
            }
        }
    }

    return 1;
}

Inven_Item* CDeathTower::CPlayData::getRewardItem(int idx)
{
    // ORIG 0x8469c06：&m_rewards[idx].m_items[0]
    return m_rewards[idx].m_items;
}

int CDeathTower::CPlayData::getRewardItemCount(int idx)
{
    // ORIG 0x8469bec：m_rewards[idx].m_itemCnt
    return m_rewards[idx].m_itemCnt;
}

int CDeathTower::CPlayData::getRewardExp(int idx)
{
    // ORIG 0x8469c1e：m_rewardExp[idx]（+0x9b8 起）
    return m_rewardExp[idx];
}

int CDeathTower::CPlayData::getLastRoutedTurn()
{
    return m_lastRoutedTurn;
}

void CDeathTower::CPlayData::setLastRoutedTurn(int turn)
{
    // ORIG 0x8469b4c：m_lastRoutedTurn（+0x00）= turn
    m_lastRoutedTurn = turn;
}

char* CDeathTower::CPlayData::getStartMemberInfo()
{
    return m_startMemberInfo;
}

CDungeon* CDeathTower::CDungeonMgr::getDungeon() const
{
    return m_pDungeon;
}

void CDeathTower::CStage::resetCurrentStage()
{
    m_currentStage = 1;
}

void CDeathTower::CStage::makeStagePacket(PacketGuard& packet)
{
    // [b28d-solo-CDeathTower] NDIFF=14（221/220）：实参/this 装载序（ORIG value-first，
    // P75 跨版本）+ short 扩展两段（P200）+ 迭代器临时槽；P167 CLEAR（40/40）。
    packet.put_short(getCurrentStage());
    packet.put_int(get_rand_int(0));
    packet.put_short(m_mapInfo.m_mapIndex);
    packet.put_byte(m_mapInfo.m_monsterMap.size());

    for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
         it != m_mapInfo.m_monsterMap.end(); ++it)
    {
        map_monster mob(it->second);   // ORIG 栈拷贝（C1ERKS）后逐字段写包
        packet.put_int(mob.m_mobId);
        packet.put_short(mob.m_instId);
        packet.put_int(mob.m_mobIndex);
        packet.put_byte(mob.m_level);
        packet.put_byte(mob.m_roleType);
        packet.put_byte(mob.m_field13);
        packet.put_byte(mob.m_field14);
    }
    // ORIG 0x84619ed 起：m_mapInfo.m_itemMap（+0x30）遍历。序列化格式与
    // CBossStage::SendMapInfoPacket 不同（此处 itemIndex 仅 short、m_addInfo
    // 为 int、endurance word），按 ORIG 逐字段转写。
    packet.put_byte((int)m_mapInfo.m_itemMap.size());
    for (std::map<int, map_item>::iterator it = m_mapInfo.m_itemMap.begin();
         it != m_mapInfo.m_itemMap.end(); ++it)
    {
        map_item item = it->second;        // ORIG 21 dword 栈拷贝
        packet.put_byte((int)(signed char)item.m_standardLevel);   // +0x01
        packet.put_short((int)(short)item.m_itemIndex);            // +0x04 低 2 字节
        packet.put_int(item.m_item.m_addInfo);                     // m_item+0x02
        packet.put_int(item.m_item.get_add_info());                // m_item+0x07
        packet.put_short((int)item.m_item.m_fieldb);               // m_item+0x0b（endurance）
        packet.put_byte((int)item.m_item.m_amp.getAbilityType());  // m_item+0x11
        packet.put_short((int)item.m_item.m_amp.getAbilityValue());
    }
}

void CDeathTower::CPlayData::reset()
{
    // ORIG 0x08461f18：m_playTime/m_lastPlayTime/m_lastRoutedTurn 清零；
    // 4 成员 alive/ready 清 0；m_rewards[4]（步长 0x268）逐个
    // stRewardItem_t::reset；m_rewardExp[4] 清 0；m_enterStartMap[4] 清 0。
    m_playTime = 0;
    m_lastPlayTime = 0;
    m_lastRoutedTurn = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_memberAlive[i] = 0;
        m_memberReady[i] = 0;
        m_rewards[i].reset();
        m_rewardExp[i] = 0;
    }
    for (int i = 0; i < 4; ++i)
        m_enterStartMap[i] = 0;
}

// ORIG 0x08461cb4：m_itemCnt=0；10 个 Inven_Item reset 后 itemIdx(+0x02) 置 -1。
void CDeathTower::CPlayData::stRewardItem_t::reset()
{
    m_itemCnt = 0;
    for (int i = 0; i < 10; ++i)
    {
        m_items[i].reset();
        m_items[i].m_addInfo = -1;
    }
}

int CDeathTower::CPlayData::getStartMemberCnt()
{
    return m_startMemberCnt;
}

void CDeathTower::CPlayData::makeStartMemberInfo(CParty* party)
{
    // [b28d-solo-CDeathTower] NDIFF=54（90/87）：循环/局部槽序 -0x10/-0x14/-0x18 互换
    // （N93/N98）；P167 CLEAR（9/9）。语义（strcpy+level/job/growType 写入）全等。
    // ORIG 0x08461d02：memset(m_startMemberInfo,0,0x5c)；遍历 4 个队伍槽，
    // 有效成员（get_user 非空且 checkValidUser==1）把名字 strcpy 到
    // m_startMemberInfo+count*0x17，level/job/growType 写到条目 +0x14/+0x15/+0x16；
    // 结束时 m_startMemberCnt = get_member_count()。
    memset(m_startMemberInfo, 0, sizeof(m_startMemberInfo));
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        CUserCharacInfo* info = (CUserCharacInfo*)party->get_user(i);
        bool invalid;
        if (info == 0)
            invalid = true;
        else
            invalid = party->checkValidUser(i) != 1;
        if (!invalid)
        {
            strcpy(m_startMemberInfo + count * 0x17,
                   info->getCurCharacName());
            m_startMemberInfo[count * 0x17 + 0x14] =
                (char)info->get_charac_level();
            m_startMemberInfo[count * 0x17 + 0x15] =
                (char)info->get_charac_job();
            m_startMemberInfo[count * 0x17 + 0x16] =
                (char)info->getCurCharacGrowType();
            ++count;
        }
    }
    m_startMemberCnt = party->get_member_count();
}

void CDeathTower::CPlayData::resetMemberReady()
{
    m_allReady = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_memberReady[i] = 0;
    }
}

void CDeathTower::CPlayData::setMemberAlive(int idx, bool alive)
{
    m_memberAlive[idx] = alive;
}

bool CDeathTower::CPlayData::isMemberAlive(int idx)
{
    return m_memberAlive[idx];
}

void CDeathTower::CPlayData::setMemberReady(int idx, bool ready)
{
    m_memberReady[idx] = ready;
}

bool CDeathTower::CPlayData::isMemberReady(int idx)
{
    return m_memberReady[idx];
}

bool CDeathTower::CPlayData::checkEnterStartMap(int idx)
{
    // ORIG 0x08461e4a：!m_enterStartMap[idx] 时置 true 并返回 true。
    if (!m_enterStartMap[idx])
    {
        m_enterStartMap[idx] = true;
        return true;
    }
    return false;
}

bool CDeathTower::CPlayData::checkAllMemberReady(int memberCnt)
{
    // ORIG 0x08461e7e：m_allReady 已置位直接 true；否则统计 m_memberReady，
    // 计满 memberCnt 时置 m_allReady=1 并返回 true（循环固定 4 槽）。
    if (m_allReady != 0)
        return true;
    int readyCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_memberReady[i])
        {
            ++readyCnt;
            if (readyCnt == memberCnt)
            {
                m_allReady = 1;
                return true;
            }
        }
    }
    return false;
}

void CDeathTower::CPlayData::addPlayTime(unsigned int time)
{
    m_lastPlayTime = m_playTime;
    m_playTime += time;
}

unsigned int CDeathTower::CPlayData::getPlayTime()
{
    return m_playTime;
}

unsigned int CDeathTower::CPlayData::getLastPlayTime()
{
    return m_lastPlayTime;
}

} // namespace WongWork
