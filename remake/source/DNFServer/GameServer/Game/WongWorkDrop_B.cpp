// ============================================================================
// [b28d-38 判词] 本 TU 8 件最终状态（p167 全部 CLEAR）：
//  _getGenItemProbIdx(Hell/Special) 0x08535a88/0x0853736e DIFF（P183 去 probs 引用缓存；
//   残差 N4/P30 v[0]<=value 的 cmp 极性/disp 折叠）
//  setDropRatio(Hell/Special) 0x08535948/0x085359e8 DIFF（helper call→物理内联 P165，
//   语义补全 14→53 条；残差 N4 v[2+j] 的 +2 并入索引不可折叠）
//  generateSpecificItem 0x08535726 DIFF（m_itemList 具名成员直访消 getItemList call；gap 6）
//  hellPartySpecificItem 0x08535b08 DIFF（byte 层 N 封顶）
//  chooseItem 0x08534fce DIFF（size()==0 / it-- 后置 / 先声明后赋值 P50；N88 残差）
//  _generateRandomItem 0x08536882 DIFF（AMC 链式重发 + 花括号零初始化 P253；gap 11）
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

// ORIG 调用点 `call; test %al,%al; je`（无 setne）⇒ 返回类型 bool（P292）。
// 桥的返回类型不入 mangled，改 bool 不破坏 ABI；ORIG 体为 bool 字段 movzbl 直返。
bool AI_IsHellMonster(CAICharacter const* self)
    __asm__("_ZNK12CAICharacter13IsHellMonsterEv");
bool Mon_IsHellMonster(CMonster const* self)
    __asm__("_ZNK8CMonster13IsHellMonsterEv");
void AI_generateDropAvatarItem(CAICharacter const* self, unsigned char a,
                               unsigned char b,
                               std::vector<Inven_Item>& out)
    __asm__("_ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE");

// stGenerateRefData_t +0x34 起调用方上下文区的组件视图：ORIG 0x08535726/0x08535b08
// 均有具名指针局部（`mov 0xc(%ebp),%eax; add $0x34,%eax; mov %eax,-0x2c(%ebp)`）并
// 经它读 +0x35/+0x36/+0x38（P161 组件视图；裸偏移禁止）。布局照 +0x34..+0x3b。
struct stGenerateRefCtx_t
{
    char m_field34;                 // +0x34
    char m_field35;                 // +0x35
    char m_rateSlot;                // +0x36
    char m_pad37;                   // +0x37
    int  m_rankBonusIndex;          // +0x38
};

}  // namespace

// ---------------------------------------------------------------------------
// setDropRatio — ORIG T 0x08535948 / 0x085359e8（两者同型，报告逐条对应）：
//   vector 赋值 m_probsSub(+0x24) = m_probs(+0x18)，随后逐元素 j=0..4：
//   fild(v[j]) * ratio → roundFloat → 存回同一槽位（元素步长 8 ⇒ v[2+j]）。
// ---------------------------------------------------------------------------
void CMonsterDrop_Hell::setDropRatio(float ratio)
{
    // [b28d-solo-WongWorkDrop_B] AE：51=51 逐条同形（残差仅 .o 重定位标签，
    // 见 P162/P264；compare_tu clean + p167 CLEAR）。修法 = 头内 w[5] 视图
    // 复现 ORIG disp=8 折叠（P30），POD 零布局/零符号影响。
    // [b28d-38] ORIG 逐条：vector op= 后双层循环（P165 两份物理独立块，禁 helper
    // call——4.4 -O0 对 inline 模板仍发 out-of-line call，always_inline 展开则形参槽
    // 物化 +6 条）。内层缓存 `e` 引用（ORIG -0x10 槽 + `0x8(%eax,%edx,4)` 直读）。
    m_probsSub = m_probs;                       // ORIG stl_vector::operator=
    for (size_t i = 0; i < m_probsSub.size(); ++i)
    {
        stGenItemProb_t& e = m_probsSub[i];
        for (int j = 0; j < 5; ++j)
        {
            // ORIG: fildl 整数槽 → fmuls ratio → fstps → call roundFloat → 存回
            // w[j] = v[2+j] 视角（CItemGenerator.h 匿名 union；ORIG disp=8 折叠）。
            e.w[j] = static_cast<int>(roundFloat(static_cast<float>(e.w[j]) * ratio));
        }
    }
}

void CSpecialMonsterDrop::setDropRatio(float ratio)
{
    // [b28d-solo-WongWorkDrop_B] AE：51=51（同 Hell 版；残差仅重定位标签）。
    // [b28d-38] 同 CMonsterDrop_Hell::setDropRatio（ORIG 两份同型物理独立块）。
    m_probsSub = m_probs;
    for (size_t i = 0; i < m_probsSub.size(); ++i)
    {
        stGenItemProb_t& e = m_probsSub[i];
        for (int j = 0; j < 5; ++j)
        {
            e.w[j] = static_cast<int>(roundFloat(static_cast<float>(e.w[j]) * ratio));
        }
    }
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
    // [b28d-solo-WongWorkDrop_B] 语义完成 DIFF（157/156；p167 CLEAR 16/16）。
    // 修法 = ctx 指针局部(-0x2c) + 4 平铺早退 nop 岛 + e.w[] disp 折叠。
    // 残差 = UseLuckPoint 实参寄存器角色互换（ORIG rarity→edx/m_field1→eax，
    // 4.4 反向），N7 封顶（cast/内联实参多形穷举均不翻）。
    // [b28d-38] ORIG 0x08535726 逐条：ctx 指针局部(-0x2c) + 四个平铺早退
    // （每个早退生成 nop 中继岛，末尾 return 直落函数尾）。
    stGenerateRefCtx_t const& ctx =
        *reinterpret_cast<stGenerateRefCtx_t const*>(&ref.m_field34);

    int idx = WM_Hell_getGenItemProbIdx(
        this, static_cast<int>(static_cast<unsigned char>(ctx.m_field35)));
    if (idx < 0)
        return;

    stGenItemProb_t const& e = m_probsSub[idx];      // ORIG op[] on +0x24
    // 阈值 = e.w[(byte)ctx.m_rateSlot] * ref.m_dropRate / 100（ORIG disp=8 折叠）
    int threshold =
        e.w[static_cast<unsigned char>(ctx.m_rateSlot)] * ref.m_dropRate / 100;
    if (m_random.generateNumber(0, 1000) > threshold)
        return;

    int luckRoll = m_random.generateNumber(1, 1000000);
    int basisRow = static_cast<int>(static_cast<signed char>(ref.m_field42_byte));
    // GetItemRarity(&m_rarityBasis[basisRow], luckRoll, dropType=2)
    int rarity =
        reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)->GetItemRarity(
            &m_rarityBasis[basisRow][0], luckRoll, 2);

    int itemIdx = m_table.chooseItem(eItemDropType_2,
                                     static_cast<ENUM_RARITY>(rarity),
                                     static_cast<unsigned char>(ctx.m_field35));
    if (itemIdx == -1)
        return;

    // [b28d-38] P167：ORIG 无 getItemList() call（头内 inline 在 4.4-O0 发
    // out-of-line call，ORIG 是 `mov 0xc(%eax),%eax` 直读 m_itemList）⇒ 具名成员直访。
    CItem* item = G_CDataManager()->m_itemList->find_item(itemIdx);
    if (item == 0)
        return;

    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    // 实参内联（右→左求值：size→get_grade→get_rarity→m_field1，ORIG esi/ebx 缓存）
    Luck_UseLuckPointM(reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint),
                       inv.m_field1, item->get_rarity(),
                       item->get_grade(),
                       static_cast<int>(result.m_dropItems.size()));
    result.m_dropItems.push_back(inv);
}

// ---------------------------------------------------------------------------
// hellPartySpecificItem — ORIG T 0x08535b08
// b21 同族：如 generateSpecificItem 注，成员直读替换 u8_at/i8_at（+0x3d 有符号 movsbl）。
// ---------------------------------------------------------------------------
void CMonsterDrop_Hell::hellPartySpecificItem(stGenerateRefData_t const& ref,
                                              stGenerateResult_t& result)
{
    // [b28d-solo-WongWorkDrop_B] AE：149=149 逐条同形（残差仅重定位标签）。
    // 修法 = ctx 视图替裸偏移 + aiIdx/list 内联实参 + IsHellMonster 桥 bool 化
    // （P292 `call;test%al;je` 判据）+ script 去具名局部 + hasCatch 短路内联。
    int extraCount = 0;

    // ORIG 0x08535b08：aiIdx 经 %ebx 跨 G_CDataManager() 保活、list 直取，
    // 无具名局部（P0/P24 内联实参）；m_aiCharacterList 在 +0x869c。
    CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(
        static_cast<unsigned int>(ref.m_rankBonusIndex));
    if (ai != 0)
    {
        // ORIG je/jmp 分支形（三目会改块布局）
        if (AI_IsHellMonster(ai))
            extraCount = 0;
        else
            extraCount =
                static_cast<int>(static_cast<signed char>(ref.m_field3d));
        ai->generateIndependentItem(ref, result.m_dropItems);
        AI_generateDropAvatarItem(ai,
                                  static_cast<unsigned char>(ref.m_rateSlot),
                                  ref.m_field3e_byte,
                                  result.m_dropItems);
    }

    // ORIG 此处才绑定 ctx 指针局部（-0x14），并以其 +4 作 find_monster 实参。
    stGenerateRefCtx_t const& ctx =
        *reinterpret_cast<stGenerateRefCtx_t const*>(&ref.m_field34);
    CMonster* mon = static_cast<CMonster*>(
        G_CDataManager()->find_monster(ctx.m_rankBonusIndex));
    if (mon != 0)
    {
        if (Mon_IsHellMonster(mon))
            extraCount = 0;
        else
            extraCount =
                static_cast<int>(static_cast<signed char>(ref.m_field3d));

        // ORIG 无 script 具名局部（getMonsterScript 结果直接 +0x3ac 后调 empty）
        if (!static_cast<STMonsterScript*>(mon->getMonsterScript())
                 ->m_independentDrops.empty())     // +0x3ac
            mon->generateIndependentItem(ref, result.m_dropItems);

        Mon_generateDropAvatarItem(mon,
                                   static_cast<unsigned char>(ref.m_rateSlot),
                                   ref.m_field3e_byte,
                                   result.m_dropItems);

        // ORIG 短路物化无槽（mov $1/$0 岛 + 尾 test），具名 bool 会多槽+回读。
        if (!static_cast<STMonsterScript*>(mon->getMonsterScript())
                 ->m_catchItems.empty() &&
            ref.m_field45 != 0)                 // +0x70
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
    // [b28d-solo-WongWorkDrop_B] 语义完成 DIFF（364/370）。
    // 修法 = --partyPos 直接改形参槽（ORIG `subb $1,-0x1dc`）+ 花括号零初始化
    // （P214③/P253）+ 去 boost 声明处初始化 + e.w 视图 + 内联实参。
    // 残差 N 封顶：①帧/全槽号平移 0x10（0x21c vs 0x22c，N82/N93 类）；
    // ②x87 比较 lowering——ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp
    //   （N85；P230 提示 -march=i486 可消，属路由变更，已上报 Main）。
    int probIdx = WM_Special_getGenItemProbIdx(this, static_cast<int>(dungeonLv));
    if (probIdx < 0)
        return;

    stGenItemProb_t t = m_probsSub[probIdx];         // ORIG 拷贝 7 ints 到栈
    // ORIG 直接对形参 spill 槽做 `subb $0x1,-0x1dc(%ebp)`（非另起 int 局部）。
    --partyPos;

    for (int i = 1; i <= 4; ++i)
    {
        float factor = 1.0f;
        // [b28d-38] P214③/P253：ORIG 内联清零（无 memset call）⇒ 花括号零初始化。
        int fallbackTable[6] = {0};                  // ORIG 栈 -0x7c..-0x64（24B 清零）
        int* tablePtr = &m_rarityBasis[i][0];
        double boost;

        if (i == 2)
        {
            // [b28d-38] P183/P167：ORIG 无 amc 具名局部，两次
            // `G_CDataManager()->GetAutoMarketContitionsControl()` 各重发一次调用。
            factor = G_CDataManager()->GetAutoMarketContitionsControl()
                         ->GetItemDropRate(probIdx);
            boost = 0.0;
            if (factor != 0.0f)
                boost = 1.0f / factor - 1.0f;
            if (G_CDataManager()->GetAutoMarketContitionsControl()
                    ->ChangeItemRarity(&m_rarityBasis[i][0], fallbackTable, 6,
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
                        m_partyBonus[i][partyPos];
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

        CItem* item = G_CDataManager()->m_itemList->find_item(itemIdx);   // [b28d-38] 同上
        if (item == 0)
            continue;

        if (i == 2)
        {
            // [b28d-38] P253：ORIG `rep stos`（0x40 dwords）⇒ 花括号零初始化。
            char buf[256] = {0};                      // ORIG -0x1b9 帧 256B 清零
            sprintf(buf,
                    "special monster drop rate : item_id(%d), mon_lv(%d), "
                    "role(%d), dungeon_lv(%d), dungeon_diff(%d), member_cnt(%d), "
                    "bourgeois_rate(%d), drop_rate(%d), rand_value(%d), "
                    "drop_value(%d)",
                    itemIdx, static_cast<int>(type), static_cast<int>(role),
                    static_cast<int>(dungeonLv), static_cast<int>(difficulty),
                    static_cast<int>(partyPos),
                    bourgeoisRate, dropRate, randValue, dropValue);
            Luck_sendItemDropRate(
                reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint), buf);
        }

        // ORIG 同型四连 + UseLuckPoint：Inven_Item ctor / 写 item idx /
        // create_item(1,&inv,1) / get_grade+get_rarity+UseLuckPoint / push_back。
        Inven_Item inv;                                // ORIG 内联 C2
        inv.m_addInfo = itemIdx;                       // ORIG 写对象首 +0x02
        G_CDataManager()->m_itemList->create_item(
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
    // [b28d-solo-WongWorkDrop_B] 语义完成 DIFF（208/204；p167 CLEAR 18/18）。
    // 修法 = result 声明/赋值位置照 ORIG（randInt 后 `movl $-1,-0x20`、正常/catch
    // 两路共享尾返回）+ randomOptionIdx 两句式预清（P198①）+ __PRETTY_FUNCTION__（P139）。
    // 残差 N 封顶：①ORIG `cmpl $-1;setne;test;je` 物化 vs 4.4 直 je（N-88 族）；
    // ②迭代器/临时槽号整体平移 0x10（N82/N93）。
    // ORIG 0x08534fce：type==2 先试 random_option 槽；否则按 diff/ddt 选块；
    // 整段 try 包裹，catch(...) 走 cMyTrace（pretty/0x2e5/5）+ 日志后返回 -1。
    // result 声明在 try 外、赋值在 randInt 之后（ORIG `movl $0xffffffff,-0x20`
    // 位置实证），正常/catch 两路共享尾部 `mov -0x20,%eax` 返回。
    int result;
    try
    {
        if (type == eItemDropType_2)
        {
            // ORIG 先 `movl $0xffffffff,-0x1c` 再 call（P198① 两句式预清）。
            int randomOptionIdx = -1;
            randomOptionIdx =
                static_cast<int>(m_optHandle.choose_item(rarity, level));
            if (randomOptionIdx != -1)
                return randomOptionIdx;
        }
        // [b28d-38] P167：ORIG 调 map::size()（无 empty）；P28 增量侧：ORIG 用
        // 后置 it--（mmEi，带临时槽）；P50 复活：ORIG 有 iterator::C1Ev ⇒ 先声明后赋值。
        if (m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].size() == 0)
            return -1;
        std::map<unsigned int, int>::iterator lastIter;
        lastIter = m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].end();
        lastIter--;
        unsigned long maxKey = lastIter->first;
        unsigned long roll = static_cast<unsigned long>(
            m_rand->randInt(maxKey));
        // ORIG 在 randInt 之后才 `movl $0xffffffff,-0x20`（result 槽）。
        result = -1;
        std::map<unsigned int, int>::iterator lb =
            m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].lower_bound(roll);
        if (lb == m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].end())
            lb--;
        result = lb->second;
    }
    catch (...)
    {
        // P139：ORIG 传 __PRETTY_FUNCTION__ 局部静态（_ZZ...E19__PRETTY_FUNCTION__）。
        cMyTrace t(__PRETTY_FUNCTION__, 0x2e5, 5);
        t("[%s][%d]", __PRETTY_FUNCTION__, 0x2e5);
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
    // [b28d-solo-WongWorkDrop_B] AE：44=44 逐条同形（残差仅重定位标签；
    // p167 CLEAR 3/3）。修法 = for+break 形（jg 跳 break / jge 跳循环外 nop 岛）
    // + unsigned i（4.4 对 unsigned 局部恒内存直比 `cmp -0x10(%ebp),%eax`）。
    // [b28d-38] ORIG 形 = `for (i=0; size()>i; ++i) { e=m_probsSub[i];
    // if (e.v[0]<=value && e.v[1]>=value) break; }`：jg 跳过 break（条件假）、
    // jge 跳循环外 nop 岛（命中）；尾部 size()<=i 再查一次 ⇒ setbe;test;je。
    // i 用 unsigned：4.4 对 unsigned 局部与 size() 比较恒内存直比（ORIG
    // `cmp -0x10(%ebp),%eax`；int 局部会多一条 `mov -0x10,%edx` 中转）。
    unsigned int i = 0;
    for (; m_probsSub.size() > i; ++i)
    {
        stGenItemProb_t& e = m_probsSub[i];
        if (e.v[0] <= value && e.v[1] >= value)
            break;
    }
    if (m_probsSub.size() <= i)
        return -1;
    return i;
}

int CSpecialMonsterDrop::_getGenItemProbIdx(int value)
{
    // [b28d-solo-WongWorkDrop_B] AE：44=44（同 Hell 版；残差仅重定位标签）。
    // [b28d-38] ORIG 形同 CMonsterDrop_Hell 版（两份物理独立块）。
    unsigned int i = 0;
    for (; m_probsSub.size() > i; ++i)
    {
        stGenItemProb_t& e = m_probsSub[i];
        if (e.v[0] <= value && e.v[1] >= value)
            break;
    }
    if (m_probsSub.size() <= i)
        return -1;
    return i;
}

}  // namespace WongWork
