// ============================================================================
// CClearReward.cpp — WongWork::CClearReward 清关奖励生成器实现
// （ORIG 报告 docs/class_func_reports/WongWork__CClearReward/ 13 方法逐段转写）
//
// 覆盖符号（唯一定义点；ctor/loadScript 归 DropLayoutDeps.cpp）：
//   setDropRatio            T 0x085385c8
//   getGoldCardCommission   T 0x085385b2
//   getGoldCardCreateRate   W 0x085bf000
//   getRewardItemRatePerMapMaxCount W 0x085bf018
//   _getGenItemProbIdx      T 0x08538694
//   generateRandomItem      T 0x0853873a（虚槽 +0x08）
//   _generateRandomItem     T 0x085387c0
//   _generateSpecificItem   T 0x08538bf8
//   generateSpecificItem    T 0x08538c84（虚槽 +0x0c）
//   generatePremiumGoldCardItem T 0x08538a92
//   generateGoldCardBlankItem   T 0x08539166
//   generatePcRoomCardBlankItem T 0x0853900c
//   power_war_dungeon_gold_clear_reward T 0x08538e64
// 依赖符号（ORIG T 无所属 TU；一并落于此避免双定义）：
//   WongWork::CItemGenRateTable::chooseItem     T 0x085349a4
//   WongWork::CItemGeneratorMgr::onTryGenerateRandomItem T 0x085342c6
//   ::CLuckPoint::GetItemRarity                 T 0x08550be4
//   ::CLuckPoint::GetLuckPoint                  T 0x085509da
//   ::roundFloat                                W 0x0853abe5
// ============================================================================
#include "CItemGenerator.h"
#include "CItemGeneratorMgr.h"
#include "GameRand.h"

#include "CDataManager.h"
#include "CDungeon.h"
#include "ServerParameterScript.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "GameMasterCmd.h"   // CGameMasterCharacter::sendServerState（sendItemDropRate 虚发）
#include <math.h>

// 全局 ::CLuckPoint ctor —— ORIG T 0x08550884 含 m_users.clear()。
CLuckPoint::CLuckPoint()
{
    m_users.clear();
}

namespace WongWork
{
// ============================================================================
// stGenerateRefClearReward_t —— generateRandomItem 调用方写入外层 ref+0x34 起
// 的清关奖励上下文视图。ORIG mangled _ZN8WongWork26stGenerateRefClearReward_tE；
// 成员偏移全部来自 _generateRandomItem/_generateSpecificItem 反汇编访问：
//   +0x00 u8 命中基准值 / +0x01 u8 等级档【推断命名，布局实证】
//   +0x04 float 加成率 / +0x08 int dropProb 组 / +0x0c CDungeon*
// sizeof == 0x10（m_dungeon 止，ORIG 访问不超过 +0xf 实证）
// ============================================================================
struct stGenerateRefClearReward_t
{
    unsigned char m_value;      // +0x00
    unsigned char m_level;      // +0x01
    char m_pad02[2];            // +0x02..03
    float m_bonusRate;          // +0x04
    // +0x08：ORIG 0x085387d8 `mov 0x8(%eax),%edx` 单段直落（与
    // _getGenItemProbIdx 第二形参同型）⇒ 字段即 eClearRewardDropProb_t（P254）。
    eClearRewardDropProb_t m_dropProb;
    CDungeon* m_dungeon;        // +0x0c
};

// §4.3 裸偏移清零：调用方上下文区在 stGenerateRefData_t 内的起点由**具名字段**
// m_field34（CItemGenerator.h:210）经 offsetof 推导，不再是字面量 0x34；
// 视图尺寸 0x10 由下方断言锁定（本文件上方布局注释：ORIG 访问不超过 +0xf）。
// 取「常量相加」而非「直取字段地址」：ORIG 该处是基址 + 立即数两步物化，
// `&ref.m_field34` 会被 4.4 折成单条 lea，实测 generateRandomItem ndiff 2→14
// 倒退，故保留相加形（常量仍由具名字段推导，无裸偏移字面量）。
static const int kRefClearRewardCtx =
    static_cast<int>(offsetof(stGenerateRefData_t, m_field34));
static_assert(sizeof(stGenerateRefClearReward_t) == 0x10,
              "clear-reward ctx size drift");

// ORIG 0x08538ad8 段 `call G_CDataManager; mov -0xc(%ebp),%edx; add $0x1264,%edx;
// mov 0x14(%eax,%edx,4),%eax` 实证：六档稀有度概率表以 CDataManager +0x14 为基址、
// 索引 i+0x1264（scaled ×4）。以本域具名视图表达（§4.3：禁裸偏移字面量）；
// 基址字节偏移 0x14 按元素折算进索引（0x14/4=5）——4.4 会把成员 disp 强度归约成
// 额外 add，折进索引后与 ORIG 同为「add 常量 + scaled disp」两条（探针 p6 f1/f2）。
static const int kRarityProbIdx = 0x1264 + 0x14 / 4;   // = 0x1269
struct CDataManagerRarityProbView
{
    int m_probs[kRarityProbIdx + 6];   // 六档概率 @ dm+0x14，索引 i+0x1264
};

// power_war_dungeon_gold_clear_reward / generateSpecificItem 的 ref+0x34 上下文
// （ORIG 0x08538e77 / 0x08538c9x 段访问实证：+1 金币档位、+2 队伍档位、
// +8/+0xa/+0xe 三个 ushort 分量）。与 _generateRandomItem 的
// stGenerateRefClearReward_t 同起点但消费面不同，故各立具名视图（§4.3）。
struct stPowerWarRefCtx_t
{
    unsigned char m_field00;       // +0x00（generateSpecificItem 的 getGoldGenTable 档位）
    unsigned char m_levelGold;     // +0x01（m_dungeonGoldBonus 档位）
    unsigned char m_levelParty;    // +0x02（m_partyBonus 档位）
    char m_pad03;                  // +0x03
    // +0x04：ORIG 0x08538eaa `mov 0x4(%eax),%ebx` 单段直落 ⇒ 字段与
    // GetExpBonusWeight 形参（eRankBonusIndex）同型（P211 档③）。
    eRankBonusIndex m_rankBonusIndex;
    unsigned short m_field08;      // +0x08
    unsigned short m_field0a;      // +0x0a
    char m_pad0c[2];               // +0x0c..0x0d
    unsigned short m_field0e;      // +0x0e
};
static_assert(sizeof(stPowerWarRefCtx_t) == 0x10,
              "power-war ref ctx size drift");

// m_common->getGoldGenTable() 返回的两元素表（ORIG 0x08538ee9 `mov (%eax),%eax`
// / 0x08538f90 `mov 0x4(%eax),%eax` 实证）。指针下标在 4.4 会被拆成
// `add $0x4; mov (%eax)` 两步，具名成员访问折单条 disp。
struct stGoldGenTable_t
{
    int m_base;    // +0x00（base = m_base*0xaf/1000）
    int m_range;   // +0x04（GetRandomRange 半径）
};

void CClearReward::setDropRatio(float ratio)
{
    for (int grp = 0; grp <= 5; ++grp)      // ORIG 0x08538687 setle（有符号 <=5）
    {
        m_workProbs[grp] = m_scriptProbs[grp];
        for (size_t i = 0; i < m_workProbs[grp].size(); ++i)
        {
            // ORIG 085385c8：roundFloat(value * ratio) 回写工作副本。
            // 单次 operator[] 后经具名引用读两处（P157 例外；P167 原报 EXTRA ×1）。
            stGenItemProb_t& prob = m_workProbs[grp][i];
            prob.v[2] =
                static_cast<int>(roundFloat(static_cast<float>(prob.v[2]) * ratio));
        }
    }
}

int CClearReward::getGoldCardCommission(int n)
{
    return m_goldCardCost[n];
}

float CClearReward::getGoldCardCreateRate()
{
    return m_goldCardCreateRate;
}

int CClearReward::getRewardItemRatePerMapMaxCount(int mapCount) const
{
    // P196：`if (mapCount <= 9) return arr[mapCount]; return 0;` 使数组访问 inline fall-through，
    // ORIG `cmpl $0x9; jg(返0岛)` + 内联 mov。
    if (mapCount <= 9)
        return m_rewardRatePerMap[mapCount];
    return 0;
}

// [b28d-solo-CClearReward] verdict=AE：循环体基址改数组指针形
// `(*(m_workProbs + drop))[i]`，复现 ORIG `lea 0x60(%eax),%ecx; …;
// lea (%ecx,%eax,1),%edx`（下标形 4.4 恒折 add 链；探针 /tmp/b28d_ccr/p5.cpp f1↔f2）。
int CClearReward::_getGenItemProbIdx(int value, eClearRewardDropProb_t drop)
{
    // [b28d-63] ORIG 0x08538694：基址 `this+drop*0xc+0x60` 每次重算（无具名
    // vector 引用局部，P0/P183）；循环体单次 operator[] 存 -0xc 后读两字段
    // （P157 例外）；尾部 size()<=i ⇒ mov $0xffffffff 岛。
    size_t i = 0;
    while (i < m_workProbs[drop].size())
    {
        // [b28d-solo-CClearReward] ORIG 0x085386a3 循环体基址形：
        // `mov 0x8(%ebp),%eax; lea 0x60(%eax),%ecx; …; lea (%ecx,%eax,1),%edx`
        // —— 数组名退化指针 + 下标（`*(m_workProbs+drop)`）才产「先算基址常量再
        // 合并索引」；下标形 `m_workProbs[drop][i]` 在 4.4 恒折 `add $0x60,%eax;
        // add this,%eax`（探针 /tmp/b28d_ccr/p5.cpp f1↔f2 实证）。
        const stGenItemProb_t& prob = (*(m_workProbs + drop))[i];
        if (value >= prob.v[0] && value <= prob.v[1])
            break;
        ++i;
    }
    if (m_workProbs[drop].size() <= i)
        return -1;
    return static_cast<int>(i);
}

void CClearReward::generateRandomItem(stGenerateRefData_t const& ref,
                                      stGenerateResult_t& result)
{
    // [b27 封顶] 唯一残差 = ref+0x4(m_field04) float 读：ORIG 4.1 disp 单步
    // `mov 0x4(%eax),%edx`，4.4 对 cast 指针/具名字段/偏移表达式均折
    // `add $0x4;mov (%eax)` 两步（探针 35/36/37），N81 同族不可造。
    if (m_random.generateNumber(1, 10000) < m_dropKindProb[0])
    {
        _generateRandomItem(*reinterpret_cast<stGenerateRefClearReward_t const*>(
                                reinterpret_cast<char const*>(&ref) +
                                kRefClearRewardCtx),
                            *reinterpret_cast<int const*>(&ref),
                            result,
                            ref.m_field04f);
    }
    else
    {
        _generateSpecificItem(*reinterpret_cast<stGenerateRefClearReward_t const*>(
                                  reinterpret_cast<char const*>(&ref) +
                                  kRefClearRewardCtx),
                              result);
    }
}
// [b28d-solo-CClearReward] verdict=DIFF（N 封顶）：
// 已修 —— m_dropProb 枚举化（P254 单段直落）、数组指针形 (*(m_workProbs+drop))[idx]
// （P30/P183 钥匙，探针 p5 f1/f2）、dropRoll 内联（P0）、thr 改 A*(1.0f-dropRate)
// 书写序（P243）、find_item 具名结果（P115）。
// 残余：①ORIG 把 e.v[0]/v[1]/v[2] 三字段各存槽（-0x3c/-0x38/-0x34）而我方只存
// e 地址（-0x40）——「三具名 int 局部（v0/v1 后续未用）」不可由合法源形反推；
// ②帧 0xa4 vs 0x9c（push edi/esi）+ 全帧槽号平移（N82）；③itemIdx/found/循环变量
// 槽位连锁。
void CClearReward::_generateRandomItem(stGenerateRefClearReward_t const& refClear,
                                       int param, stGenerateResult_t& result,
                                       float dropRate)
{
    // 第一参 = refClear.m_value（命中基准）；档位 = refClear.m_dropProb。
    int idx = _getGenItemProbIdx(
        static_cast<int>(refClear.m_value),
        refClear.m_dropProb);
    if (idx < 0)
        return;

    // ORIG 0x085387e5：`lea 0x60(%eax),%ebx; …; lea (%ebx,%eax,1),%edx` —— 数组名
    // 退化指针 + 下标形（同 _getGenItemProbIdx；探针 p5 f1/f2 实证）。
    const stGenItemProb_t& e = (*(m_workProbs + refClear.m_dropProb))[idx];
    // ORIG 两处均 fistpl 截断（fnstcw/fldcw 0xc 截断模式），非 round；
    // 保持 (int) 强转截断语义。ORIG 的 x87 序：先算整数因子 A，再 fld1/fsubs
    // 得 (1.0f-dropRate) 并 fmulp（P243 书写序 ⇒ A * (1.0f-dropRate)）。
    int thr = static_cast<int>(
        static_cast<float>(
            (static_cast<int>(refClear.m_bonusRate * e.v[2]) +
             m_dungeonDropBonus[refClear.m_level]) *
            (param / 100)) *
        (1.0f - dropRate));

    // ORIG 0x085388ca：generateNumber(0,10000) 结果**不落槽**（`cmp -0x2c(%ebp),%eax;
    // setge %al; test; jne` 直判）⇒ 内联实参（P0）。
    if (m_random.generateNumber(0, 10000) >= thr)
        return;

    int dropType = 1;   // 初值（未命中即 1）
    int cum = 0;
    int roll = m_random.generateNumber(2, 10000);
    for (int i = 1; i <= 4; ++i)
    {
        cum += m_dropItemTypeProb[i - 1];
        if (roll < cum)
        {
            dropType = i;
            break;
        }
    }
    int rarityRoll = m_random.generateNumber(3, 1000000);
    // ORIG：CLuckPoint::GetItemRarity(this=result+0xc)——全局类与本域 CLuckPoint
    // 前 0xc（vector<CUser*>）布局一致，按 ORIG 同址处理。
    typedef ::CLuckPoint GlobalLuckPoint;
    int rarity = reinterpret_cast<GlobalLuckPoint*>(&result.m_luckPoint)
                     ->GetItemRarity(m_rarityBasis, rarityRoll, dropType);
    G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
        static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
        static_cast<eItemDropType_t>(dropType),
        static_cast<ENUM_RARITY>(rarity));
    int itemIdx = m_table.chooseItem(static_cast<eItemDropType_t>(dropType),
                                     static_cast<ENUM_RARITY>(rarity),
                                     refClear.m_value);
    if (itemIdx == -1)
        return;
    // ORIG `mov %eax,-0x10(%ebp); cmpl $0x0,-0x10(%ebp); je` ⇒ 具名结果局部（P115）。
    CItem* found = G_CDataManager()->m_itemList->find_item(itemIdx);
    if (found == 0)
        return;
    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    result.m_dropItems.push_back(inv);
}

void CClearReward::_generateSpecificItem(stGenerateRefClearReward_t const& refClear,
                                         stGenerateResult_t& result)
{
    // ORIG generateNumber 结果不落槽（P0 内联右值，P134 嵌套实参形）：
    // mov 0xc(%ebp),%edx; mov 0xc(%edx),%edx; mov %eax,0x4(%esp); mov %edx,(%esp)。
    // ORIG je 直落统一 leave 出口：if(!=-2){主体} 形消除 4.4 jmp/nop 中继
    //（P63 早退共享出口，探针实证）。
    int itemIdx = refClear.m_dungeon->get_clear_item(m_random.generateNumber(4, 100));
    if (itemIdx != -2)
    {
        Inven_Item inv;
        inv.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
        result.m_dropItems.push_back(inv);
    }
}

void CClearReward::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    // [b28d-solo-CClearReward] verdict=AE。与 power_war_dungeon_gold_clear_reward 同族修形：
    // ORIG 0x08538c9x 段 rc 具名指针局部（-0x24），fA/fB/rankIdx/sp/sum/baseCount
    // 全部无槽（表达式内联，P0/P183）；ushort 与档位经具名视图 disp 访问。
    unsigned char const* rc =
        reinterpret_cast<unsigned char const*>(&ref) + kRefClearRewardCtx;
    float weight =
        (m_partyBonus[reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty - 1] *
         m_dungeonGoldBonus[reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelGold]) *
        (1.0f + G_CDataManager()->m_serverParameter.GetExpBonusWeight(
                    reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_rankBonusIndex));
    stGoldGenTable_t* goldTable = reinterpret_cast<stGoldGenTable_t*>(
        m_common->getGoldGenTable(reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field00));
    int base = (goldTable->m_base * 0xaf) / 1000;
    // ORIG 0x08538cfx：sum 无槽（表达式内联到 (sum*base)/2）。
    int gold = static_cast<int>(
        static_cast<float>(
            (static_cast<int>(reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field08 +
                              (reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field0e * 2 +
                               reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field0a) * 2) *
             base) / 2) *
        weight);
    static const float kGoldDivisor = 2.0f;   // _DAT_08c9a6d8 = 0x40000000 = 2.0f
    // ORIG 循环条件每轮重读 rc[2]（`movzbl 0x2(%eax),%eax`）⇒ baseCount 不具名。
    for (int i = 0; i < reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty; ++i)
    {
        // piece/randSpan 均无槽（FPU 链连续 + GetRandomRange 内联实参）。
        int amount =
            (static_cast<int>((static_cast<float>(gold) / kGoldDivisor /
                               static_cast<float>(static_cast<int>(
                                   reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty))) *
                              weight) *
             (GetRandomRange(-goldTable->m_range, goldTable->m_range) + 100)) / 100;
        Inven_Item inv;
        inv.m_field1 = static_cast<unsigned char>(2);
        inv.m_addInfo = 0;
        inv.set_add_info(amount);
        result.m_dropItems.push_back(inv);
    }
}

void CClearReward::power_war_dungeon_gold_clear_reward(
    stGenerateRefData_t const& ref, stGenerateResult_t& result, int level)
{
    // [b28d-solo-CClearReward] verdict=AE。ORIG 0x08538e6e..0x08538ea4：rc 具名指针局部（-0x20）；
    // fA/fB **无槽**（`flds 0xc(%eax,%edx,4)` 直读成员，P0/P183），书写序照 P243
    // （先 flds 左操作数 = m_partyBonus）；rankIdx/sp 亦无槽（内联表达式）。
    unsigned char const* rc =
        reinterpret_cast<unsigned char const*>(&ref) + kRefClearRewardCtx;
    // ctx 视图不落具名指针槽（ORIG 仅 rc 一个指针槽 -0x20）。
    float weight =
        (m_partyBonus[reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty - 1] *
         m_dungeonGoldBonus[reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelGold]) *
        (1.0f + G_CDataManager()->m_serverParameter.GetExpBonusWeight(
                    reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_rankBonusIndex));
    stGoldGenTable_t* goldTable = reinterpret_cast<stGoldGenTable_t*>(
        m_common->getGoldGenTable(static_cast<unsigned char>(level)));
    int base = (goldTable->m_base * 0xaf) / 1000;
    // ORIG 0x08538f0b：sum 无槽（表达式内联到 (sum*base)/2），ushort 经视图 disp 访问。
    // ORIG 0x08538f61：piece 亦无槽（FPU 链连续，末尾 fistpl 截断），故一并内联。
    int gold = static_cast<int>(
        static_cast<float>(
            (static_cast<int>(reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field08 +
                              (reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field0e * 2 +
                               reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_field0a) * 2) *
             base) / 2) *
        weight);
    static const float kGoldDivisor = 2.0f;   // _DAT_08c9a6d8 = 2.0f
    int amount =
        (static_cast<int>((static_cast<float>(gold) / kGoldDivisor /
                           static_cast<float>(static_cast<int>(
                               reinterpret_cast<stPowerWarRefCtx_t const*>(rc)->m_levelParty))) *
                          weight) *
         (GetRandomRange(-goldTable->m_range, goldTable->m_range) + 100)) / 100;
    Inven_Item inv;
    inv.m_field1 = static_cast<unsigned char>(2);
    inv.m_addInfo = 0;
    inv.set_add_info(amount);
    result.m_dropItems.push_back(inv);
}

// [b28d-solo-CClearReward] verdict=NEAR（N1 封顶）：102=102。唯一残差 = 概率表
// 基址常量折叠——ORIG `add $0x1264,%edx; mov 0x14(%eax,%edx,4),%eax`（两步）vs
// 4.4 折 `add $0x1269,%edx; mov (%eax,%edx,4),%eax`（一步；探针 p6 f1/f2/f6 穷举）。
void CClearReward::generatePremiumGoldCardItem(
    stGenerateRefData_t const& ref, stGenerateResult_t& result,
    ENUM_RARITY rarity, unsigned int defaultItem)
{
    // [b28d-solo-CClearReward] ORIG 0x08538a99 函数头两条初始化
    // （`movl $0x2,-0x20(%ebp)` / `movl $0x0,-0x1c(%ebp)`）⇒ 原码 dropType/rarityIdx
    // 均为函数头具名局部（P0 槽往返律），调用点读 -0x20/-0x1c 而非内联立即数。
    eItemDropType_t dropType = eItemDropType_2;
    ENUM_RARITY rarityIdx = static_cast<ENUM_RARITY>(0);
    if (static_cast<int>(rarity) < 0)
    {
        int roll = m_random.generateNumber(3, 10000);
        for (int i = 0; i < 6; ++i)
        {
            // 视图 + 内联索引：ORIG `mov 0x14(%eax,%edx,4),%eax` 单条 scaled 装载
            // （指针局部会多占槽并使基址链改形，P0）。
            if (roll <= reinterpret_cast<CDataManagerRarityProbView*>(
                            G_CDataManager())->m_probs[i + kRarityProbIdx])
            {
                rarityIdx = static_cast<ENUM_RARITY>(i);
                break;
            }
        }
    }
    else
    {
        rarityIdx = rarity;
    }
    G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
        static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
        dropType, rarityIdx);
    int itemIdx = m_table.chooseItem(
        dropType, rarityIdx,
        static_cast<unsigned char>(ref.m_field34));   // §4.3 具名化（+0x34）
    if (itemIdx == -1)
        itemIdx = static_cast<int>(defaultItem);
    // ORIG `mov %eax,-0x14(%ebp); cmpl $0x0,-0x14(%ebp); je` ⇒ 原码为具名结果局部
    // 后内存判空（P115），非调用表达式直比（后者 4.4 产 sete/test 物化）。
    CItem* found = G_CDataManager()->m_itemList->find_item(itemIdx);
    if (found == 0)
        return;
    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    result.m_dropItems.push_back(inv);
}

// [b28d-solo-CClearReward] verdict=AE：stGoldCardBlankItemInfo_t::v 由 int[3]
// 改 unsigned int[3]（CItemGenerator.h:790）—— v[1]→Inven_Item::m_addInfo 单段
// 直落、v[2]→CItemList::create_item(...,int) 两段 eax→ebx 中转（P211 档③）。
int CClearReward::generateGoldCardBlankItem(Inven_Item& item)
{
    if (m_goldBlanks.empty())
        return 0;
    int cum = 0;
    int pick = get_rand_int(m_goldCardBlankTotal);
    // [b28d-63] ORIG 0x085391a9：const_iterator begin/end 头部各求值一次 +
    // operator!=/operator++/operator->（P167 原报 EXTRA operator[] ×4）。
    std::vector<stGoldCardBlankItemInfo_t>::const_iterator it = m_goldBlanks.begin();
    std::vector<stGoldCardBlankItemInfo_t>::const_iterator endIt = m_goldBlanks.end();
    while (it != endIt)
    {
        cum += it->v[0];
        if (pick < cum)
        {
            if (it->v[1] == -1)
                return 0;
            item.m_addInfo = it->v[1];
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), item, it->v[2]);
            return 1;
        }
        ++it;
    }
    return 0;
}

// [b28d-solo-CClearReward] verdict=AE：同 generateGoldCardBlankItem（v 无符号化）。
int CClearReward::generatePcRoomCardBlankItem(Inven_Item& item)
{
    if (m_pcRoomBlanks.empty())
        return 0;
    int cum = 0;
    int pick = get_rand_int(m_pcRoomBlankTotal);
    // [b28d-63] 同 generateGoldCardBlankItem：ORIG 0x0853904f 起 const_iterator 遍历。
    std::vector<stGoldCardBlankItemInfo_t>::const_iterator it = m_pcRoomBlanks.begin();
    std::vector<stGoldCardBlankItemInfo_t>::const_iterator endIt = m_pcRoomBlanks.end();
    while (it != endIt)
    {
        cum += it->v[0];
        if (pick < cum)
        {
            if (it->v[1] == -1)
                return 0;
            item.m_addInfo = it->v[1];
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), item, it->v[2]);
            return 1;
        }
        ++it;
    }
    return 0;
}

// ---- 依赖：CItemGenRateTable::chooseItem（ORIG T 0x085349a4）----
// [b28d-solo-CClearReward] verdict=NEAR（N82 封顶）：182=182，助记符序列全等
// （`setne` 物化经 P225 while+return 形复现）；仅余槽号/寄存器分配差
// （ORIG 迭代器 -0x38 / result -0x10，我方 -0x28 / -0x18；帧同 0x60）。
int CItemGenRateTable::chooseItem(eItemDropType_t type, ENUM_RARITY rarity, int level)
{
    // [b28d-solo-CClearReward] 逐条对齐 ORIG 0x085349a4：
    //  ①首判 `cmpl $0x2,0xc(%ebp)` 是 **type==2**（0xc=第一形参），且 choose_item
    //    第一参压 `lea 0x10(%ebp)`（形参 rarity 的引用）；itemIdx 预置 -1 再赋值
    //    （P198① 预清两句式）。
    //  ②`call size; test; sete; test; je` ⇒ 源形 `size()==0`（P197④），非 empty()。
    //  ③迭代器先默认构造（C1Ev）后赋 end()（P50 复活）；`it--` 为**后置**
    //    （ORIG 调 operator--(int)，带 0 哑元 + sret 临时）。
    //  ④槽址每次重算（三处 imul 链），故不缓存 map 引用（P183）。
    //  ⑤try 仅覆盖 lower_bound..it->second（ORIG .gcc_except_table 08534b11..08534b8b）；
    //    catch 用 __PRETTY_FUNCTION__ 恢复 `_ZZ...E19__PRETTY_FUNCTION__` 局部静态
    //    （P139/P118），格式串 "[%s][%d]"、行号 0x279。
    if (type == eItemDropType_2)
    {
        int itemIdx = -1;
        itemIdx = static_cast<int>(m_optHandle.choose_item(rarity, level));
        // P225：ORIG `cmpl $0xffffffff,-0xc; setne %al; test %al,%al; je` 的
        // 4.1 bool 物化形由 while+return 复现（普通 if 在 4.4 恒直跳 je）。
        while (itemIdx != -1)
        {
            return itemIdx;
        }
    }
    if (m_slots[type][rarity][level].size() == 0)
        return -1;
    std::map<unsigned int, int>::iterator it;
    it = m_slots[type][rarity][level].end();
    it--;
    unsigned int maxKey = it->first;
    unsigned int roll = static_cast<unsigned int>(m_rand->randInt(maxKey));
    int result = -1;
    try
    {
        it = m_slots[type][rarity][level].lower_bound(roll);
        if (it == m_slots[type][rarity][level].end())
            it--;
        result = it->second;
    }
    catch (...)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x279, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x279);
    }
    return result;
}

// ---- 依赖：CItemGeneratorMgr::onTryGenerateRandomItem（ORIG T 0x085342c6）----
void CItemGeneratorMgr::onTryGenerateRandomItem(eGenerateType_t genType,
                                                eItemDropType_t dropType,
                                                ENUM_RARITY rarity)
{
    // ORIG T 0x085342c6 索引 = (dropType*6 + genType*30 + rarity)*4 ⇒ 行主序
    // [genType][dropType][rarity]，即权威 CItemGeneratorMgr::m_dropStat（与
    // m_head 同起点的 union 视图；`mov (%esi,%eax,4)`/`mov %esi,(%ebx,%eax,4)`
    // 均 4 字节 ⇒ unsigned int 宽一致，未对齐/越界风险随具名化消除）。
    ++m_dropStat[genType][dropType][rarity];
}

}  // namespace WongWork

// ORIG T 0x08550954：m_users 按幸运点取最大者下标（空表 0）。
unsigned int CLuckPoint::_GetMaxLuckPointUserIndex()
{
    int maxPoint = 0;
    unsigned int best = 0;
    for (unsigned int i = 0; i < m_users.size(); ++i)
    {
        if (maxPoint < m_users[i]->GetCurCharacLuckPoint())       // [0x855098d setg]
        {
            maxPoint = m_users[i]->GetCurCharacLuckPoint();       // [0x85509ae 双 call P48]
            best = i;
        }
    }
    return best;
}

// ORIG T 0x08550b14：最大幸运点用户执行扣点，成功记 m_points[seq]=序号。
// [b28c-lane17] idx 由 unsigned int 改 int：①ORIG 0x08550b41 首判是 `cmpl $0x0,-0x10`
// 有符号内存直比（cast 形在 4.4 反产 mov+test）；②0x08550b70 的 `m_users[idx]`
// 点为「值先」装载序（P218：int 下标→size_t 实参转换中转即复现），
// 尾段 m_points[seq]=idx 的 int 直存不变。
bool CLuckPoint::UseLuckPoint(int itemType, int rarity, int grade, int seq)
{
    if (m_users.empty())
    {
        return false;                                   // [0x8550925 guard]
    }
    int idx = (int)_GetMaxLuckPointUserIndex();
    if (idx < 0 || m_users.size() <= (unsigned int)idx) // [js / ja 两段]
    {
        return false;
    }
    CUser* user = m_users[idx];
    if (user == 0)
    {
        return false;                                   // [0x8550b87]
    }
    if (!UseLuckPoint(user, itemType, rarity, grade))   // [0x8550baf xor $1]
    {
        return false;
    }
    m_points[seq] = idx;                                // [0x8550bc2 operator[](p4)]
    return true;
}

// ORIG T 0x08550ef0：m_users 中 getCurCharacR 非空者经 GM 虚发文本。
void CLuckPoint::sendItemDropRate(char const* msg)
{
    for (unsigned int i = 0; i < m_users.size(); ++i)   // [seta 无符号循环]
    {
        CUser* user = m_users[i];
        if (user != 0)                                  // [0x8550f16 两判]
        {
            if (user->getCurCharacR())                  // [0x8550f22 setne 物化]
            {
                user->GetGameMasterCharacter()->sendServerState(user, msg);  // [0x8550f50 虚槽+0]
            }
        }
    }
}

// ============================================================================
// 全局 ::CLuckPoint 幸运点修正（ORIG T 0x085509da / 0x08550be4）
// ============================================================================
// ORIG T 0x08550e50：m_points[idx]（operator[]）得用户序号；越界返 0，
// 否则 m_users[序号]（ORIG 以 int 承载指针值，调用方 m_ownerId 为 int）。
int CLuckPoint::GetUserGainItem(int idx)
{
    if (idx < 0 || static_cast<unsigned int>(idx) >= m_points.size())
    {
        return 0;
    }
    const int userSeq = m_points[idx];
    if (userSeq < 0 || static_cast<unsigned int>(userSeq) >= m_users.size())
    {
        return 0;
    }
    return reinterpret_cast<int>(m_users[userSeq]);
}

int CLuckPoint::GetLuckPoint()
{
    // [b28c-lane17] ORIG 0x085509e7：call empty 后**无** `xor $1`，直接
    // `test %al; jne 成功块` ⇒ 源形是「empty 则跳过」（负极守卫 + 空 then），
    // 非 `if (empty()) return 0;`（后者 4.4 产 xor+je 反极性）；
    // 尾部分支失败路径直落 `mov $0`（return 0），故成功分支补显式 return。
    // [b28c-lane17] ORIG 0x085509e7：`call empty; xor $1; test %al; jne 体`
    // + 尾 `mov $0` 岛 ⇒ 取反守卫 `if (!empty()) { 体 } return 0;`
    // （fresh 实测：本形 54=54 AE；裸 `if(empty()) return 0` 与
    //  `if (empty()) {体}` 包裹两候选均缺 xor 段且 54/53 DIFF）。
    if (!m_users.empty())
    {
    int sum = 0;
    for (size_t i = 0; i < m_users.size(); ++i)
    {
        if (m_users[i] != 0)
            sum += reinterpret_cast<CUserCharacInfo*>(m_users[i])->GetCurCharacLuckPoint();
    }
    // ORIG 0x085509da：divl（无符号除法，int/size_t 提升）；强制 int 会生成 idivl。
    return sum / m_users.size();
    }
    return 0;   // 空集共用尾块（ORIG +0x9f mov $0,%eax）
}

// [b28d-solo-CClearReward] verdict=DIFF（N 封顶）：
// 已修 —— 拆嵌套 if 消 `&&` 常量岛（P225 推广反向）、luck/sp 内联（P0）、
// 0.01f 右结合复现 `fildl A; fildl B; flds 0.01; fmulp; fmulp`（P243）。
// 残余：①x87 槽组整体平移（ORIG -0x4c/-0x50 vs 我方 -0x3c/-0x40，帧同 0x74）——
// 4.1 临时槽与具名槽穿插 vs 4.4 分区排布（N82附）；②unique/rare 两分支的 (int)
// 截断槽号连锁；③循环下标 int vs size_t 的 seta/setb 形（N 族）。
int CLuckPoint::GetItemRarity(int* table, int roll, int dropType)
{
    int acc[6] = {0};

    // [b28d-solo-CClearReward] ORIG 0x08550c0e 两守卫**直跳**（`jne` + `call empty;
    // xor $1; test; je`，无 && 整体物化）⇒ 拆嵌套 if（P225 推广反向；`&&` 单表达式
    // 在 4.4 会多一段 mov$1/jmp/mov$0 常量岛）。luck 无槽（ORIG `mov %eax,%ebx` 内联
    // 实参）、sp 无槽（`lea 0x68(%eax),%edx` 内联）⇒ 均按 P0 内联。
    if (dropType == 2)
    {
        if (!m_users.empty())
        {
        int lp = 0;
        G_CDataManager()->m_serverParameter.getLuckPoint(GetLuckPoint(), lp);
        int neg = -GetTenThousandPercentage(table[2] - table[1], lp);
        int unique = 0;
        int rare = 0;
        for (size_t i = 0; i < m_users.size(); ++i)
        {
            if (m_users[i] != 0)
            {
                CParty* party = reinterpret_cast<CParty*>(m_users[i]->GetParty());
                if (party != 0)
                {
                    unique = party->Get_party_overlapped_drop_ratio_unique();
                    rare = party->Get_party_overlapped_drop_ratio_rare();
                    break;
                }
            }
        }
        if (unique != 0)
        {
            // ORIG 0x08550dxx：`fildl A; fildl B; flds 0.01f; fmulp; fmulp`
            // ⇒ A*(B*0.01f) 右结合（P243 书写序）。
            int v = static_cast<int>(
                static_cast<float>(table[3] - table[2]) *
                           (static_cast<float>(unique) * 0.01f));
            acc[2] -= v;
            if (table[3] > 999999)
                acc[2] = 0;
        }
        if (rare != 0)
        {
            int v = static_cast<int>(
                static_cast<float>(table[2] - table[1]) *
                           (static_cast<float>(rare) * 0.01f));
            neg -= v;
        }
        if (table[1] != 0)
            acc[0] = table[0] * neg / table[1];
        acc[1] = neg;
        }
    }
    int ret = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (roll <= table[i] + acc[i])
        {
            ret = i;
            break;
        }
    }
    return ret;
}

// 全局 ::roundFloat（ORIG W 0x0853abe5：call round 后 fnstcw/fldcw(0xc 截断)+fistpl
// 取整为 int，直接 mov %eax 返回——返回类型为 int，调用点均为 call 后直接用 eax）
int roundFloat(float v)
{
    return static_cast<int>(round(static_cast<double>(v)));
}
