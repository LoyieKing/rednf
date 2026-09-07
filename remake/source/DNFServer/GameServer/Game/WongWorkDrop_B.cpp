// ============================================================================
// WongWorkDrop_B.cpp — WongWork 掉落生成器批 B（LinkClosureB.WongWorkDrop）
//
// 本 TU 为唯一 owner，逐报告/objdump 忠实转写以下 8 个 ORIG 符号
// （WongWorkDrop_A.cpp 中对应 asm 桥在链接期由本 TU 的真定义消解，A 不改动）：
//   CMonsterDrop_Hell::setDropRatio(float)                    T 0x08535948
//     _ZN8WongWork17CMonsterDrop_Hell12setDropRatioEf
//   CSpecialMonsterDrop::setDropRatio(float)                  T 0x085359e8
//     _ZN8WongWork19CSpecialMonsterDrop12setDropRatioEf
//   CMonsterDrop_Hell::generateSpecificItem(ref,result)       T 0x08535726
//   CMonsterDrop_Hell::hellPartySpecificItem(ref,result)      T 0x08535b08
//   CSpecialMonsterDrop::_generateRandomItem(...)             T 0x08536882（0xAE9 字节）
//   GenerateRandomUpgrade::generateEquipGrade(int)            T 0x08533932
//   GenerateRandomUpgrade::generateUpgrade(ENUM_RARITY,int)   T 0x08533676
//   CSecialIMonsterDropItemGenRateTable::
//       chooseItem(type,rarity,level,diff,ddt)                T 0x08534fce
//
// 报告：docs/class_func_reports/WongWork__CMonsterDrop_Hell/{setDropRatio,
// generateSpecificItem,hellPartySpecificItem}.md、WongWork__CSpecialMonsterDrop/
// _generateRandomItem.md、WongWork__GenerateRandomUpgrade/{generateEquipGrade,
// generateUpgrade}.md；缺失段全部以 objdump df_game_r 兜底。
//
// ::roundFloat（W 0x0853abe5）已由 CClearReward.cpp 定义，本 TU 只 extern 引用。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include <utility>
#include <vector>

#include "CItemGenerator.h"
#include "CItemGeneratorMgr.h"
#include "CDataManager.h"
#include "CMonster.h"                    // IsHellMonster/generateIndependentItem/getMonsterScript
#include "ItemFarmingData.h"             // ItemFarmingData（ORIG W C1/C1ERKS/D1；农田权重条目）
#include "Inven_Item.h"
#include "STMonsterScript.h"            // m_independentDrops(+0x3ac)/m_catchItems(+0x70)
#include "MonsterIndependentDrop.h"     // CAICharacter 权威镜像（generateIndependentItem）

// drops 域公共依赖 ::roundFloat（ORIG W 0x0853abe5；定义 CClearReward.cpp，
// 全局命名空间符号 _Z10roundFloatf —— 放 WongWork ns 外保证调用 mangled 正确）。
extern int roundFloat(float v);

namespace WongWork
{

// ---------------------------------------------------------------------------
// 文件局部 asm 桥（只引用不定义；__asm__ 标签直链 ORIG mangled 名，参照
// WongWorkDrop_A.cpp 同型桥。本体归属：
//   CMonster/Special::_getGenItemProbIdx 有独立报告 → 归属批次未定；
//   CLuckPoint::{UseLuckPoint4int,sendItemDropRate} → 已由 CClearReward.cpp 提供；
//   CMonster/CAICharacter 头像掉落与捕获包装 → LinkClosureA；
//   CAutoMarket 两方法定义在 LinkClosureB_Misc.cpp（声明已在 CDataManager.h，
//   直接正常调用，不走本桥）。
// ---------------------------------------------------------------------------

namespace {

int WM_Hell_getGenItemProbIdx(CMonsterDrop_Hell* self, int value)
    __asm__("_ZN8WongWork17CMonsterDrop_Hell18_getGenItemProbIdxEi");
int WM_Special_getGenItemProbIdx(CSpecialMonsterDrop* self, int value)
    __asm__("_ZN8WongWork19CSpecialMonsterDrop18_getGenItemProbIdxEi");

void Luck_UseLuckPointM(::CLuckPoint* lp, int itemType, int rarity, int grade,
                        int seq)
    __asm__("_ZN10CLuckPoint12UseLuckPointEiiii");
void Luck_sendItemDropRate(::CLuckPoint* lp, char const* text)
    __asm__("_ZN10CLuckPoint16sendItemDropRateEPKc");

void Mon_generateDropAvatarItem(CMonster const* self, unsigned char a,
                                unsigned char b,
                                std::vector<Inven_Item>& out)
    __asm__("_ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE");
void Mon_generateCatchMonsterItem(CMonster const* self,
                                  std::vector<Inven_Item>& out)
    __asm__("_ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE");

unsigned char AI_IsHellMonster(CAICharacter const* self)
    __asm__("_ZNK12CAICharacter13IsHellMonsterEv");
void AI_generateDropAvatarItem(CAICharacter const* self, unsigned char a,
                               unsigned char b,
                               std::vector<Inven_Item>& out)
    __asm__("_ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE");

}  // namespace

// ---------------------------------------------------------------------------
// setDropRatio — ORIG T 0x08535948 / 0x085359e8（两者同型，报告逐条对应）：
//   vector 赋值 m_probsSub(+0x24) = m_probs(+0x18)，随后逐元素 j=0..4：
//   fild(v[j]) * ratio → roundFloat → 存回同一槽位（元素步长 8 ⇒ v[2+j]）。
// ---------------------------------------------------------------------------
namespace {

template <typename T>
inline void applyDropRatio(std::vector<T>& probsSub,
                           std::vector<T> const& probs, float ratio)
{
    probsSub = probs;                       // ORIG stl_vector::operator=
    for (size_t i = 0; i < probsSub.size(); ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            // ORIG: filds 整数槽 → fmuls ratio → fstps → call roundFloat → 存回
            probsSub[i].v[2 + j] =
                static_cast<int>(roundFloat(static_cast<float>(probsSub[i].v[2 + j]) *
                                            ratio));
        }
    }
}

}  // namespace

void CMonsterDrop_Hell::setDropRatio(float ratio)
{
    applyDropRatio(m_probsSub, m_probs, ratio);
}

void CSpecialMonsterDrop::setDropRatio(float ratio)
{
    applyDropRatio(m_probsSub, m_probs, ratio);
}

// ---------------------------------------------------------------------------
// generateSpecificItem — ORIG T 0x08535726（虚槽 +0x0c）
// b21 同族：u8_at/i8_at 全部改 CItemGenerator.h 成员直读（m_field35/m_rateSlot/
//   m_field3d/m_field3e_byte/m_field42_byte，+0x42 按 ORIG movsbl 取有符号），
//   消除 helper 的 EXTRA add+movzbl；残余寄存器分配/栈槽 N-86b + AE。
// ---------------------------------------------------------------------------
void CMonsterDrop_Hell::generateSpecificItem(stGenerateRefData_t const& ref,
                                             stGenerateResult_t& result)
{
    // probIdx = _getGenItemProbIdx((byte)ref[0x35])
    int idx = WM_Hell_getGenItemProbIdx(
        this, static_cast<int>(static_cast<unsigned char>(ref.m_field35)));
    if (idx < 0)
        return;

    stGenItemProb_t const& e = m_probsSub[idx];      // ORIG op[] on +0x24
    // 阈值 = v[2 + (byte)ref[0x36]] * *(int*)&ref /*m_dropRate*/ / 100
    int slot = static_cast<int>(static_cast<unsigned char>(ref.m_rateSlot));
    int threshold = e.v[2 + slot] * *reinterpret_cast<int const*>(&ref) / 100;
    if (m_random.generateNumber(0, 1000) <= threshold)
    {
        int luckRoll = m_random.generateNumber(1, 1000000);
        int basisRow = static_cast<int>(static_cast<signed char>(ref.m_field42_byte));
        // GetItemRarity(&m_rarityBasis[basisRow], luckRoll, dropType=2)
        int rarity =
            reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)->GetItemRarity(
                &m_rarityBasis[basisRow][0], luckRoll, 2);

        int itemIdx = m_table.chooseItem(eItemDropType_2,
                                         static_cast<ENUM_RARITY>(rarity),
                                         static_cast<int>(
                                             static_cast<unsigned char>(ref.m_field35)));
        if (itemIdx != -1)
        {
            CItem* item = G_CDataManager()->getItemList()->find_item(itemIdx);
            if (item != 0)
            {
                Inven_Item inv;
                inv.m_addInfo = itemIdx;
                G_CDataManager()->getItemList()->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
                int grade = item->get_grade();
                int itemRarity = item->get_rarity();
                Luck_UseLuckPointM(
                    reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint),
                    static_cast<int>(inv.m_field1), itemRarity, grade,
                    static_cast<int>(result.m_dropItems.size()));
                result.m_dropItems.push_back(inv);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// hellPartySpecificItem — ORIG T 0x08535b08
// b21 同族：如 generateSpecificItem 注，成员直读替换 u8_at/i8_at（+0x3d 有符号 movsbl）。
// ---------------------------------------------------------------------------
void CMonsterDrop_Hell::hellPartySpecificItem(stGenerateRefData_t const& ref,
                                              stGenerateResult_t& result)
{
    int extraCount = 0;

    // aiIdx = ref.m_rankBonusIndex(+0x38)；list = CDataManager.m_aiCharacterList(+0x869c)
    unsigned int aiIdx = (unsigned int)ref.m_rankBonusIndex;
    CAICharacterList* list =
        G_CDataManager()->m_aiCharacterList;          // +0x869c（头注释实证）
    CAICharacter* ai = list->get(aiIdx);   // ORIG get(idx)：map find
    if (ai != 0)
    {
        extraCount =
            AI_IsHellMonster(ai) ? 0
                                 : static_cast<int>(
                                       static_cast<signed char>(ref.m_field3d));
        ai->generateIndependentItem(ref, result.m_dropItems);
        AI_generateDropAvatarItem(ai,
                                  static_cast<unsigned char>(ref.m_rateSlot),
                                  ref.m_field3e_byte,
                                  result.m_dropItems);
    }

    CMonster* mon = static_cast<CMonster*>(G_CDataManager()->find_monster(
        *reinterpret_cast<int const*>(reinterpret_cast<char const*>(&ref) +
                                      0x38)));
    if (mon != 0)
    {
        extraCount =
            mon->IsHellMonster() ? 0
                                 : static_cast<int>(
                                       static_cast<signed char>(ref.m_field3d));

        STMonsterScript* script = static_cast<STMonsterScript*>(
            mon->getMonsterScript());               // ORIG inline this+0x60
        if (!script->m_independentDrops.empty())    // +0x3ac
            mon->generateIndependentItem(ref, result.m_dropItems);

        Mon_generateDropAvatarItem(mon,
                                   static_cast<unsigned char>(ref.m_rateSlot),
                                   ref.m_field3e_byte,
                                   result.m_dropItems);

        STMonsterScript* script2 = static_cast<STMonsterScript*>(
            mon->getMonsterScript());
        bool hasCatch = !script2->m_catchItems.empty() &&   // +0x70
                        ref.m_field45 != 0;                 // byte ref+0x45
        if (hasCatch)
            Mon_generateCatchMonsterItem(mon, result.m_dropItems);
    }

    // ORIG 0x08535b08 以 movzbl 读 +0x40 仅测低字节（非 dword 非空判据）。
    if (ref.m_field40_byte != 0)
    {
        for (int i = 0; i < extraCount; ++i)
            generateSpecificItem(ref, result);         // ORIG vtable 槽 +0xc 直呼
    }
}

// ---------------------------------------------------------------------------
// _generateRandomItem — ORIG T 0x08536882（CSpecialMonsterDrop，报告分段转写；
// 生成率三表偏移 0xa8/0xf8/0x15c 与 loadScript 写入证据一致）。
// ---------------------------------------------------------------------------
void CSpecialMonsterDrop::_generateRandomItem(unsigned char type, char role,
                                              unsigned char dungeonLv,
                                              unsigned char difficulty,
                                              unsigned char partyPos,
                                              int bourgeoisRate,
                                              unsigned char hellFlag,
                                              int dropRate,
                                              stGenerateResult_t& result,
                                              bool& done)
{
    int probIdx = WM_Special_getGenItemProbIdx(this, static_cast<int>(dungeonLv));
    if (probIdx < 0)
        return;

    stGenItemProb_t t = m_probsSub[probIdx];         // ORIG 拷贝 7 ints 到栈
    int ppIndex = static_cast<int>(partyPos) - 1;    // subb $1 一次执行（ORIG +0xb4）

    for (int i = 1; i <= 4; ++i)
    {
        float factor = 1.0f;
        int fallbackTable[6];                        // ORIG 栈 -0x7c..-0x64（24B 清零）
        memset(fallbackTable, 0, sizeof(fallbackTable));
        int* tablePtr = &m_rarityBasis[i][0];
        double boost = 0.0;

        if (i == 2)
        {
            CAutoMarketConditionsControl* amc =
                G_CDataManager()->GetAutoMarketContitionsControl();
            factor = amc->GetItemDropRate(probIdx);
            boost = 0.0;
            if (factor != 0.0f)
                boost = 1.0f / factor - 1.0f;
            if (amc->ChangeItemRarity(&m_rarityBasis[i][0], fallbackTable, 6,
                                      boost))
                tablePtr = fallbackTable;
        }

        // prob = trunc( v[2+i] × factor × dungeonBonus[i][difficulty](0xf8)
        //             × monsterTypeBonus[i][role](0x15c)
        //             × partyBonus[i][ppIndex](0xa8) ) —— ORIG 乘法顺序
        float probAcc = static_cast<float>(t.v[2 + i]) *
                        factor *
                        m_dungeonBonus[i][difficulty] *
                        m_monsterTypeBonus[i][static_cast<int>(role)] *
                        m_partyBonus[i][ppIndex];
        int probValue = static_cast<int>(probAcc);   // fldcw truncate ≡ (int)
        int dropValue = probValue * dropRate / 100;  // 魔数除法 = signed /100

        int randValue = m_random.generateNumber(0, 10000);
        if (!(randValue <= dropValue))               // ORIG jl skip
            continue;

        int luckRoll = m_random.generateNumber(1, 1000000);
        int rarity =
            reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)->GetItemRarity(
                tablePtr, luckRoll, i);

        CItemGeneratorMgr* mgr = G_CDataManager()->getItemGenerator();
        mgr->onTryGenerateRandomItem(CItemGeneratorMgr::eGenerateType_0,
                                     static_cast<eItemDropType_t>(i),
                                     static_cast<ENUM_RARITY>(rarity));

        int itemIdx =
            m_table.chooseItem(static_cast<eItemDropType_t>(i),
                               static_cast<ENUM_RARITY>(rarity),
                               static_cast<int>(type),
                               static_cast<ENUM_DUNGEON_DIFFICULTY>(difficulty),
                               static_cast<ENUM_ITEM_DROP_DUNGEON_TYPE>(hellFlag));
        if (itemIdx == -1)
            continue;

        CItem* item = G_CDataManager()->getItemList()->find_item(itemIdx);
        if (item == 0)
            continue;

        if (i == 2)
        {
            char buf[256];                            // ORIG -0x1b9 帧 256B 清零
            memset(buf, 0, sizeof(buf));
            sprintf(buf,
                    "special monster drop rate : item_id(%d), mon_lv(%d), "
                    "role(%d), dungeon_lv(%d), dungeon_diff(%d), member_cnt(%d), "
                    "bourgeois_rate(%d), drop_rate(%d), rand_value(%d), "
                    "drop_value(%d)",
                    itemIdx, static_cast<int>(type), static_cast<int>(role),
                    static_cast<int>(dungeonLv), static_cast<int>(difficulty),
                    static_cast<int>(static_cast<unsigned char>(ppIndex)),
                    bourgeoisRate, dropRate, randValue, dropValue);
            Luck_sendItemDropRate(
                reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint), buf);
        }

        // ORIG 同型四连 + UseLuckPoint：Inven_Item ctor / 写 item idx /
        // create_item(1,&inv,1) / get_grade+get_rarity+UseLuckPoint / push_back。
        Inven_Item inv;                                // ORIG 内联 C2
        inv.m_addInfo = itemIdx;                       // ORIG 写对象首 +0x02
        G_CDataManager()->getItemList()->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
        int grade = item->get_grade();
        int itemRarity = item->get_rarity();
        Luck_UseLuckPointM(
            reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint),
            static_cast<int>(inv.m_field1), itemRarity, grade,
            static_cast<int>(result.m_dropItems.size()));
        result.m_dropItems.push_back(inv);
        done = true;                                  // movb $1,(p10)
    }
}

// ---------------------------------------------------------------------------
// GenerateRandomUpgrade — 升级随机源（m_rand = new CMTRand(0x9c8)）。
// 比率/农田表位于 CDataManager 未建模字段（ORIG lea 0x528(%eax)/lea 0x540(%eax)
// 直接证据；本批禁改 CDataManager.h，故以裸基址偏移访问【裸偏移保留】）：
//   +0x528 map<int, ItemFarmingData>{ vector<pair<int,int>> @0 ; int active @0xc }
//     （布局依 copy ctor 0853ac12：vector op= 后拷 +0xc int，sizeof 0x10）
//   +0x540 vector<pair<int,float>>（level → 比率系数）
// ---------------------------------------------------------------------------
namespace {

typedef std::map<int, ItemFarmingData> FarmingMap;   // 元素步长与 ORIG map 一致

}  // namespace

// ---- generateEquipGrade — ORIG T 0x08533932 -------------------------------
// grades {0x5aa7,0xfa6,0x6a23,0x5df0,0x7270}；weights 全 1000，total=5000；
// w[0] 截断缩放后 total 修正为 5000-w0+w0'，randInt(&total) 落累计权重槽。
int GenerateRandomUpgrade::generateEquipGrade(int levelDelta)
{
    static const int grades[5] = { 0x5aa7, 0xfa6, 0x6a23, 0x5df0, 0x7270 };
    int weights[5] = { 1000, 1000, 1000, 1000, 1000 };
    int total = 5000;

    // P167/§4.1：ORIG 直接 G_CDataManager()+0x540 取 rates，无 equipGradeRates helper
    std::vector<std::pair<int, float> > const* rates =
        reinterpret_cast<std::vector<std::pair<int, float> > const*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x540);
    if (rates->empty())
        return 0x5aa7;
    if (rates->empty())                      // ORIG 双重空检照抄
        return 0x5aa7;

    // P167/§4.1：ORIG 内联三元+循环查表，无 lookupGradeRatio helper
    float factor = 1.0f;
    int last = static_cast<int>(rates->size()) - 1;
    if ((*rates)[0].first < levelDelta)
        factor = (*rates)[0].second;
    else if ((*rates)[last].first > levelDelta)
        factor = (*rates)[last].second;
    else
    {
        for (int i = 0; i <= last; ++i)
        {
            if ((*rates)[i].first == levelDelta)
                factor = (*rates)[i].second;
        }
    }

    int weightOld = weights[0];
    weights[0] = static_cast<int>(static_cast<float>(weights[0]) * factor);
    total = total - weightOld + weights[0];

    int roll = m_rand->randInt(static_cast<const unsigned long&>(total));

    int acc = 0;
    for (int i = 0; i <= 4; ++i)
    {
        acc += weights[i];
        if (acc >= roll)                     // acc < roll 继续（jl）
            return grades[i];
    }
    return 0x5aa7;                           // 全 miss 兜底（ORIG 同样返回首个档位）
}

// ---- generateUpgrade — ORIG T 0x08533676 ----------------------------------
int GenerateRandomUpgrade::generateUpgrade(ENUM_RARITY rarity, int levelDelta)
{
    CDataManager* dm = G_CDataManager();
    FarmingMap* farming = reinterpret_cast<FarmingMap*>(
        reinterpret_cast<char*>(dm) + 0x528);

    FarmingMap::const_iterator it = farming->find(static_cast<int>(rarity));
    if (it == farming->end())
        return 0;
    if (it->second.m_active == 0)              // pair.second + ItemFarmingData+0xc
        return 0;

    // P167/§4.1：ORIG 直接 G_CDataManager()+0x540 取 rates，无 helper
    std::vector<std::pair<int, float> > const* rates =
        reinterpret_cast<std::vector<std::pair<int, float> > const*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x540);
    float factor = 1.0f;
    if (!rates->empty())
    {
        // P167/§4.1：内联三元+循环查表（无 lookupGradeRatio helper）
        int last = static_cast<int>(rates->size()) - 1;
        if ((*rates)[0].first < levelDelta)
            factor = (*rates)[0].second;
        else if ((*rates)[last].first > levelDelta)
            factor = (*rates)[last].second;
        else
        {
            for (int i = 0; i <= last; ++i)
            {
                if ((*rates)[i].first == levelDelta)
                    factor = (*rates)[i].second;
            }
        }
    }

    ItemFarmingData entry = it->second;         // ORIG ItemFarmingData copy ctor

    int weightOld = entry.m_vecWeight[0].second;
    entry.m_vecWeight[0].second = static_cast<int>(static_cast<float>(weightOld) * factor);
    entry.m_active = entry.m_active - weightOld + entry.m_vecWeight[0].second;

    int roll = m_rand->randInt(static_cast<const unsigned long&>(entry.m_active));

    int acc = 0;
    for (size_t i = 0; i < entry.m_vecWeight.size(); ++i)
    {
        acc += entry.m_vecWeight[i].second;
        if (roll <= acc)
            return entry.m_vecWeight[i].first;
    }
    return 0;                                // ORIG 未命中返回 0
}

// ---- CSecialIMonsterDropItemGenRateTable::chooseItem — ORIG T 0x08534fce ---
// type==2（random_option 槽）先试 m_optHandle.choose_item(rarity,level)，命中
// 即返；否则按 diff/ddt 选块（ORIG 槽址 = type*0x7110+rarity*0x12d8+level*24，
// type 调用方传 1..4 ⇒ m_slots[type-1]，m_slots 自块 +0x7110 起）：
// empty→-1；roll=m_rand(+0x211fb0)->randInt(maxKey)；
// lower_bound==end 回退 --it 返 second。
int CSecialIMonsterDropItemGenRateTable::chooseItem(
    eItemDropType_t type, ENUM_RARITY rarity, int level,
    ENUM_DUNGEON_DIFFICULTY dungeonDiff,
    ENUM_ITEM_DROP_DUNGEON_TYPE dropDungeonType)
{
    // ORIG 0x08534fce：type==2 先试 random_option 槽；否则按 diff/ddt 选块；
    // 整段 try 包裹，catch(...) 走 cMyTrace（pretty/0x2e5/5）+ 日志后返回 -1。
    int result = -1;
    try
    {
        if (type == eItemDropType_2)
        {
            int randomOptionIdx =
                static_cast<int>(m_optHandle.choose_item(rarity, level));
            if (randomOptionIdx != -1)
                return randomOptionIdx;
        }
        if (m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].empty())
            return -1;
        std::map<unsigned int, int>::iterator lastIter =
            m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].end();
        --lastIter;
        unsigned long maxKey = lastIter->first;
        unsigned long roll = static_cast<unsigned long>(
            m_rand->randInt(maxKey));
        std::map<unsigned int, int>::iterator lb =
            m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].lower_bound(roll);
        if (lb == m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].end())
            --lb;
        result = lb->second;
    }
    catch (...)
    {
        cMyTrace t("int WongWork::CSecialIMonsterDropItemGenRateTable::chooseItem("
                   "WongWork::eItemDropType_t, ENUM_RARITY, int, "
                   "ENUM_DUNGEON_DIFFICULTY, ENUM_ITEM_DROP_DUNGEON_TYPE)", 0x2e5, 5);
        t("[%s][%d]",
          "int WongWork::CSecialIMonsterDropItemGenRateTable::chooseItem("
          "WongWork::eItemDropType_t, ENUM_RARITY, int, "
          "ENUM_DUNGEON_DIFFICULTY, ENUM_ITEM_DROP_DUNGEON_TYPE)", 0x2e5);
    }
    return result;
}


 // ---------------------------------------------------------------------------
// _getGenItemProbIdx —— ORIG T 0x08535a88（Hell）/ T 0x0853736e（Special），
// objdump 逐指令转写：遍历 m_probs(+0x24)，命中条件 v[0]<=value && v[1]>=value
// （cmp (elem),value jg 跳过；cmp elem1,value jge 命中）；i>=size 返回 -1。
// ---------------------------------------------------------------------------
int CMonsterDrop_Hell::_getGenItemProbIdx(int value)
{
    std::vector<stGenItemProb_t>& probs = m_probsSub;   // +0x24
    int i = 0;
    // ORIG：size() 与 i 作无符号比较（seta/setbe），不做 int 强转。
    while (probs.size() > i)
    {
        stGenItemProb_t& e = probs[i];
        if (e.v[0] <= value && e.v[1] >= value)
            return i;
        ++i;
    }
    return (probs.size() <= i) ? -1 : i;
}

int CSpecialMonsterDrop::_getGenItemProbIdx(int value)
{
    std::vector<stGenItemProb_t>& probs = m_probsSub;   // +0x24
    int i = 0;
    // ORIG：size() 与 i 作无符号比较（seta/setbe），不做 int 强转。
    while (probs.size() > i)
    {
        stGenItemProb_t& e = probs[i];
        if (e.v[0] <= value && e.v[1] >= value)
            return i;
        ++i;
    }
    return (probs.size() <= i) ? -1 : i;
}

}  // namespace WongWork
