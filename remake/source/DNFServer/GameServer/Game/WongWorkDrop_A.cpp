// ============================================================================
// WongWorkDrop_A.cpp — G1 Drop 家族（WongWorkA 批次）逐报告忠实转写
//
// 实现 16 个 ORIG 符号（唯一 owner = 本 TU）：
//   CWorldDrop::generateRandomItem              T 0x085383b8
//   CDungeonDrop::generateSpecificItem          T 0x08539c3e
//   CObjectDrop::generateRandomItem             T 0x0853818a
//   CObjectDrop::generateSpecificItem           T 0x08538234
//   CMonsterDrop::setDropRatio                  T 0x08535cf8
//   CMonsterDrop::_getBossGoldDrop              T 0x08535dc8
//   CMonsterDrop::_getGenItemProbIdx            T 0x085372ee
//   CMonsterDrop::_generateRandomItem           T 0x08535ed2
//   CMonsterDrop::_generateMonseterSpecificItem T 0x08536fce（ORIG 拼写如此）
//   CMonsterDrop::generateRandomItem            T 0x08536da8
//   CMonsterDrop::generateSpecificItem          T 0x08537070
//   stFirstMobHuntingWeight_t::stFirstMobHuntingWeight_t W 0x0853a6ae
//
// 跨 TU 依赖（P6 清桥后全部为真实成员/函数调用，本 TU 无 __asm__ 桥）：
//   CMonsterDrop_Hell::{setDropRatio,generateSpecificItem,hellPartySpecificItem}
//   CSpecialMonsterDrop::{setDropRatio,_generateRandomItem}
//   CAutoMarketConditionsControl::{GetItemDropRate,ChangeItemRarity}
//   ::CLuckPoint::{UseLuckPoint(成员4int),sendItemDropRate}
//   GenerateRandomUpgrade::{generateEquipGrade,generateUpgrade}
//   CMonster::{drop_item_index(ji),generateDropAvatarItem,generateCatchMonsterItem}
//   CAICharacter::generateDropAvatarItem
//   ::roundFloat 0x0853abe5（定义于 CClearReward.cpp）
//
// stGenerateRefData_t +0x34..+0x47 为调用方上下文 blob（头注释实证）；本文件
// 按字节偏移读取并注明语义【推断】，不改任何共享布局。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include <algorithm>

#include "CItemGenerator.h"
#include "Inven_Item.h"
#include "GameRand.h"
#include "CDataManager.h"
#include "CMonster.h"
#include "STMonsterScript.h"
#include "MonsterIndependentDrop.h"
#include "CDungeon.h"

namespace WongWork
{


}  // namespace WongWork

// ORIG W 0x0853abe5 ::roundFloat —— 定义于 CClearReward.cpp（drops 域公共依赖，
// 全局命名空间符号 _Z10roundFloatf；声明须置于全局域保证调用 mangled 匹配）
extern int roundFloat(float v);

namespace WongWork
{

// ---------------------------------------------------------------------------
// 文件局部 asm 桥（只引用不定义）
// ---------------------------------------------------------------------------
namespace {

// b28d-solo-WongWorkDrop_A：CWorldDrop ORIG 具名指针局部 ctx = &ref+0x34
// 的字节视图（ctx[0]=type/ctx[1]=level）。结构体指针视图使 4.4 出
// `movzbl 0x1(%eax)` 单条 disp（裸 ctx[1] 恒产 add $1 + disp 0，P30/N4）。
struct StRefCtxView
{
    unsigned char m_type;    // +0x34
    unsigned char m_level;   // +0x35
};

// CMonsterDrop::generateSpecificItem 低难度块 ORIG 具名指针 ctx = &ref+0x34
//（-0x20 槽）：ctx[4]=+0x38 int / ctx[8]=+0x3c / ctx[9]=+0x3d。
struct StRefCtx2
{
    unsigned char m_pad[4];      // +0x34..+0x37
    int m_rankBonusIndex;        // +0x38
    unsigned char m_difficulty;  // +0x3c
    unsigned char m_count;       // +0x3d
    unsigned char m_field3e;     // +0x3e
    unsigned char m_field3f;     // +0x3f
};

}  // namespace

// ===========================================================================
// CWorldDrop::generateRandomItem — ORIG T 0x085383b8
// b21 同族：+0x34/+0x35 改成员直读（m_field34/m_field35，语义同 u8_at）。
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=DIFF(N121)：p167 CLEAR；残余仅 4.4
// epilogue-threading 尾岛 2×`jmp+nop`（ORIG 无岛），break/goto 尾标签/return
// 三形实测不消 ⇒ N121 封顶。已修：ctx 具名指针局部（-0x20）、prod/100 去负数
// 特判、pick 复用 roll 槽、push 内联、int i + size() 有符号比较、slot 预清 0。
// [round4 L3CUser 复核 2026-09-10] ae_insn 残差仅 2 处：①`movl $&_ZL14gUnicodeBuffer
//   +0xe174,0x8(%esp)` 我方为 .o 未解析重定位（AE/N132 等价）；②尾 2×`jmp+nop`
//   为我方多出（ORIG 无岛）⇒ N121 维持，指令数 123/127 与判词一致。
// [A28 round5 复核 2026-09-10] 重跑 clean 口径：orig=123 / ours=125，残差仍仅尾部
//   2×`jmp`（ORIG `jne <T>; leave; ret` 直落 ⇒ 无岛）⇒ N121（4.4 epilogue-threading
//   尾岛不可消）维持封顶；p167 调用点 CLEAR。
// [A28 round5 · C05③ P223 实测 2026-09-10 19:42] 按 Main 转发的「单尾标签串消早返
//   中继岛」范式，把三处早返改为 `goto end;` + 尾 `end:;`（跨 goto 的 slot/threshold/acc
//   一律声明与赋值分两句）：**实测无效**，orig=123/ours=125 与残差 2×`jmp` 逐字不变
//   （CWorldDrop 的岛来自 4.4 自身 epilogue-threading，非早返平铺产物）⇒ 已回退为
//   `return;` 形，N121 维持。C05①② 对本 TU 其余件亦无命中（残差无 `cmp mem` 类、
//   无「失败出口落函数尾」布局差）。
void CWorldDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                    stGenerateResult_t& result)
{
    // [b28d-solo-WongWorkDrop_A] ORIG 形：具名指针局部 ctx = &ref+0x34
    //（-0x20 槽），ctx[0]/ctx[1] 多次直读；threshold 直接 prod/100（无负数
    // 特判分支，ORIG 无 js）；pick 复用 roll 槽；push 展开为内联体。
    // ctx 基址取具名字段 m_field34（§4.3/P82；+0x35 = m_field35 字节视图）。
    StRefCtxView const* ctx =
        reinterpret_cast<StRefCtxView const*>(&ref.m_field34);
    int roll = m_random.generateNumber(0, 100000);     // CGenerateRandomNumber(this+8)

    if (ctx->m_level > 199)                            // ORIG movzbl cmp $0xc7 ja
        return;

    WorldDropInfo* slot = 0;                           // ORIG 预清 movl $0,-0x18
    if (ctx->m_type == 1)                              // warArea 分支
        slot = &m_warAreaDrop[ctx->m_level];
    else
        slot = &m_worldDrop[ctx->m_level];
    // threshold = first × dropRate / 100（ORIG imul 0x51eb851f 魔数除法）
    int threshold = (slot->m_first * ref.m_dropRate) / 100;
    if (roll > threshold)                              // jg 退出
        return;

    roll = m_random.generateNumber(1, slot->m_total);  // 复用 roll 槽
    int acc = 0;                                       // local_14
    for (int i = 0; static_cast<int>(slot->m_chunks.size()) > i; ++i)
    {
        acc += slot->m_chunks[i].m_rate;               // 元素+0x04 权重
        if (roll < acc)
        {
            Inven_Item inv;                            // 元素+0x00 idx
            inv.m_addInfo = slot->m_chunks[i].m_itemIdx;
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
            result.m_dropItems.push_back(inv);
            return;
        }
    }
}

// ===========================================================================
// CObjectDrop::generateRandomItem — ORIG T 0x0853818a
// b21 同族：u8_at/i8_at → m_field34/m_field35/m_rateSlot/m_pad37 成员直读（语义不变）。
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=AE：去 type 具名局部（P0，ORIG 两次独立
// `movzbl 0x34` 直读；帧 -4、done/n 槽 -0x21/-0x20 → -0x1d/-0x1c 全等）。
void CObjectDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                     stGenerateResult_t& result)
{
    bool done = false;                                  // local_21
    for (int n = 0; n < static_cast<signed char>(ref.m_pad37); ++n)
    {
        // [b28d-solo-WongWorkDrop_A] P0：ORIG 两次独立 movzbl 0x34 直读，
        // 无 type 具名局部（去掉后帧 -4、槽号 -0x21/-0x20 → -0x1d/-0x1c）。
        _generateRandomItem(static_cast<unsigned char>(ref.m_field34), 0,
                            static_cast<unsigned char>(ref.m_field34),
                            static_cast<unsigned char>(ref.m_field35),
                            static_cast<unsigned char>(ref.m_rateSlot),
                            0,                          /* context3c = NULL */
                            ref.m_dropRate, result, done);
    }
}

// ===========================================================================
// CObjectDrop::generateSpecificItem — ORIG T 0x08538234
// b21 同族：i8_at(0x39) → static_cast<signed char>(m_field39)（语义不变）。
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=AE：去 props 指针缓存（P183，ORIG 每次
// begin/end 重读 ref+0x34）+ pushCreatedItem 内联 + m_field34l 具名 list 视图。
void CObjectDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                       stGenerateResult_t& result)
{
    // 调用方在 ref+0x34 dword 写入 list<pair<int,int>> 配置指针
    //（§4.3 具名化：m_field34l 指针视图，CItemGenerator.h union）
    // [b28d-solo-WongWorkDrop_A] P183/P0：ORIG begin/end 调用点各自重读
    // ref+0x34（无 props 缓存槽，迭代器槽 -0x20/-0x1c → -0x1c/-0x18）；
    // pushCreatedItem 展开为 ORIG 内联体（O0 下 helper 恒产 call）。
    for (int n = 0; n < static_cast<signed char>(ref.m_field39); ++n)
    {
        int pick = m_random.generateNumber(2, 10000);
        int acc = 0;
        for (std::list<std::pair<int, int> >::const_iterator it =
                 ref.m_field34l->begin();
             it != ref.m_field34l->end(); ++it)
        {
            acc += it->second;                          // +4 权重
            if (pick < acc)
            {
                Inven_Item inv;
                inv.m_addInfo = it->first;
                G_CDataManager()->m_itemList->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
                result.m_dropItems.push_back(inv);
                break;
            }
        }
    }
}

// ===========================================================================
// CDungeonDrop::generateSpecificItem — ORIG T 0x08539c3e
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=AE：去 props 缓存（P183）+ acc 先声明
// （-0x10 深）/pick 后（-0xc）+ pushCreatedItem 内联 + m_field34v 具名 vector
// 视图（复现 ORIG 单条 `mov 0x34(%eax),%edx`）。
void CDungeonDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    // 调用方在 ref+0x34 dword 写入 vector<pair<int,int>> 配置指针
    //（§4.3 具名化：基址取自具名字段 m_field34）
    // [b28d-solo-WongWorkDrop_A] P183/P0：ORIG begin/end 调用点各自重读
    // ref+0x34（无 props 指针缓存槽）；acc 先声明（-0x10 深）/pick 后（-0xc）；
    // pushCreatedItem helper 展开为 ORIG 内联体（O0 下 helper 恒产 call）。
    int acc = 0;
    int pick = m_random.generateNumber(1, 10000);
    for (std::vector<std::pair<int, int> >::const_iterator it =
             ref.m_field34v->begin();
         it != ref.m_field34v->end();
         ++it)
    {
        acc += it->second;                             // +4 权重
        if (pick < acc)
        {
            Inven_Item inv;
            inv.m_addInfo = it->first;
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
            result.m_dropItems.push_back(inv);
            return;
        }
    }
}

// ===========================================================================
// CMonsterDrop
// ===========================================================================

// ---- _getGenItemProbIdx — ORIG T 0x085372ee ----
// [b28d-solo-WongWorkDrop_A] verdict=AE：循环内 return 改「命中 break +
// 循环外 size()<=i 复核」（消 4.4 mov$1/mov$0 bool 岛）；元素具名引用 e；
// unsigned i 使 size() 比较恒内存直比（P225/P194 族）。
// [b28d-solo-WongWorkDrop_A] 修法：循环内 return i 改为「命中 break + 循环外
// size()<=i 复核」两段（消 4.4 的 mov$1/mov$0 bool 岛，复现 ORIG
// setbe;test;je 尾块）；元素经具名引用 e 单次 operator[]（ORIG -0xc 槽往返）；
// i 用 unsigned int 使 size() 比较恒内存直比（P225/P194 族）。
int CMonsterDrop::_getGenItemProbIdx(int value)
{
    unsigned int i = 0;
    for (; m_probsSub.size() > i; ++i)                 // +0x24 工作副本表
    {
        stGenItemProb_t& e = m_probsSub[i];
        if (e.v[0] <= value && e.v[1] >= value)
            break;
    }
    if (m_probsSub.size() <= i)
        return -1;
    return i;
}

// ---- _getBossGoldDrop — ORIG T 0x08535dc8 ----
// [b28d-solo-WongWorkDrop_A] verdict=AE：真语义 bug 修（首参 ORIG 为
// m_conditionRate[type].value + .rate，旧码误取 m_goldQuantity[2*type]）；
// m_conditionRateW[4][2] 二维视图复现 disp32 折叠、m_goldQuantityS[2] 结构体
// 数组视图复现 `add $K` + scaled4 折叠；正向 if(type<4) 包裹（P222）。
// [b28d-solo-WongWorkDrop_A] 真语义 bug 修正：ORIG 首参用
// m_conditionRate[type].value + m_conditionRate[type].rate（0x25938a/0x259388
// 两半字），旧码误取 m_goldQuantity[2*type]；且 ORIG 无 probSlot/baseSlot/
// goldLo/goldHi 具名局部（P0 寄存器直传），tier 是唯一局部（-0xc）。
// 分支形：`ja <尾部 mov $0>` 失败块在函数尾 ⇒ 正向 if(type<4){…return true}
// return false（P222 正向包裹）。
bool CMonsterDrop::_getBossGoldDrop(unsigned char type, unsigned short& outGold,
                                    unsigned short& outCount)
{
    if (type < 4)
    {
        int tier = 0;
        // ORIG：value 先物化 %ebx 跨 generateNumber 保活，实参和式
        // value + rate 各自独立重读（无局部）。
        if (static_cast<int>(m_conditionRateW[type][1]) >
            m_random.generateNumber(
                0, static_cast<int>(m_conditionRateW[type][1]) +
                       static_cast<int>(m_conditionRate[type].rate)))
            tier = 1;                                  // setg：value > roll → 高档

        outGold = static_cast<unsigned short>(
            m_random.generateNumber(0, m_goldQuantityS[tier].m_hi -
                                           m_goldQuantityS[tier].m_lo) +
            m_goldQuantityS[tier].m_lo);
        outCount = m_goldVolume[tier];                 // (tier+0x12c9d0)*2 → 步长2
        return true;
    }
    return false;
}

// ---- setDropRatio — ORIG T 0x08535cf8 ----
// [b28d-solo-WongWorkDrop_A] verdict=AE：外层单次 operator[] 取具名元素引用 e
// （P157 例外）+ e.w[r]（+0x08 起五档视图）复现 ORIG `mov 0x8(%eax,%edx,4)`。
void CMonsterDrop::setDropRatio(float ratio)
{
    m_probsSub = m_probs;                              // +0x24 ← +0x18（operator=）
    for (size_t i = 0; i < m_probsSub.size(); ++i)
    {
        // [b28d-solo-WongWorkDrop_A] ORIG 外层单次 operator[] 取具名元素
        //（-0x10 槽），内层两次索引直读同一元素（P157 例外：引用缓存）；
        // 直写 m_probsSub[i].v[2+r] 会每轮多两次 operator[] 调用。
        stGenItemProb_t& e = m_probsSub[i];
        for (int r = 0; r < 5; ++r)
        {
            // v[2+r]（元素 +8 + r*4）：五档稀有度权重 ×ratio 后回写
            e.w[r] =
                static_cast<int>(roundFloat(static_cast<float>(e.w[r]) * ratio));
        }
    }
    m_special.setDropRatio(ratio);                     // Special (+0x4706c) 先
    m_hell.setDropRatio(ratio);                        // Hell (+0x238ac) 后
}

// ---- _generateRandomItem — ORIG T 0x08535ed2 ----
// [b28d-solo-WongWorkDrop_A] verdict=DIFF(N82/N78 封顶)：p167 CLEAR 50=50
//（0 missing/0 extra）；gap=19 全为跨版本 codegen——N78（ORIG altTable 清零
// rep stos 对齐剥离慢路 vs 4.4 简单形）、N82（帧 0x24c vs 0x25c：ORIG buf
// 与 goldItem 同址复用，全帧槽号 +0x10 平移）、N85（x87 fdivs/fucompp-fnsahf
// vs fdivp/fucomip）。真语义 bug 修 6 处（见下）。
// [round4 L3CUser 复核 2026-09-10] 帧 0x24c vs 我方 0x25c（+0x10）、槽号
//   -0x21c/-0x1fc/-0x204/-0x208 vs -0x22c/-0x20c/-0x214/-0x218 恒定平移 ⇒
//   判据⑪ B-FRAME（整帧平移）⇒ N126/N82，声明重排无效；gap -19 为此族固有。
// [A28 round5 复核 2026-09-10] 重跑 p167：CLEAR 50=50（仅 Inven_Item C1↔C2 同址
//   别名，P93 良性）；orig 665 / ours 646（raw 670/652）；帧 0x24c vs 0x25c。
//   ⇒ 维持 N78（altTable `rep stos` 对齐剥离慢路）+ N82/N126（整帧 +0x10）封顶。
// 参数（自 wrapper）【推断命名】：
//   type       = ref+0x34 怪物等级档
//   role       = ref+0x35（signed char；==3 走首领金币路径）
//   dungeonLv  = ref+0x36 地下城等级档
//   difficulty = ref+0x37 难度档
//   partyPos   = ref+0x38 组队档位（-1 后作加成列下标）
//   context3c  = *(int*)&ref[0x3c]（人数上下文，clip [0,100]）
//   dropRate   = ref.m_dropRate（×value/100）
void CMonsterDrop::_generateRandomItem(unsigned char type, char role,
                                       unsigned char dungeonLv,
                                       unsigned char difficulty,
                                       unsigned char partyPos, int context3c,
                                       int dropRate,
                                       stGenerateResult_t& result, bool& done)
{
    // [b28d-solo-WongWorkDrop_A] ORIG 首参取第 3 形参 dungeonLv（-0x204 槽）；
    // 权重行一次 operator[] 后 7 个连续 mov（结构整体拷贝 -0x90..-0x78）。
    int idx = _getGenItemProbIdx(dungeonLv);
    if (idx < 0)
        return;

    stGenItemProb_t t = m_probsSub[idx];

    unsigned char partyCol = static_cast<unsigned char>(partyPos - 1);

    // 基础概率（ORIG fildl t.v[2] 先入栈，再 flds dungeon/monster/party 逐次
    // fmulp；dungeonBonus 索引用第 4 形参 difficulty）
    int chance = static_cast<int>(
        static_cast<float>(t.v[2]) *
        *reinterpret_cast<float const*>(&m_dungeonBonus[0][0] + difficulty) *
        *reinterpret_cast<float const*>(&m_monsterTypeBonus[0][0] + role) *
        *reinterpret_cast<float const*>(&m_partyBonus[0][0] + partyCol));

    // member_cnt clip：max(context,0)→min(100)；>50 打递减折扣
    context3c = std::min<int>(100, std::max<int>(0, context3c));
    if (context3c > 50)
    {
        float decay = 0.01f + 0.002f * static_cast<float>(context3c - 50);
        chance = static_cast<int>((1.0f - decay) * static_cast<float>(chance));
    }
    chance = (chance * dropRate) / 100;                // imul/sar 除法

    // ORIG：setl 不成立时**跳过金币段**继续 done 检查（非 early return）
    if (m_random.generateNumber(0, 10000) < chance)
    {
        // 金币（m_common->getGoldGenTable(type)：+0 基准 / +4 浮动幅度）
        int* gt = m_common->getGoldGenTable(type);
        int goldAmt = gt[0] + (gt[0] * GetRandomRange(-gt[1], gt[1])) / 100;

        Inven_Item goldItem;
        goldItem.m_field1 = static_cast<unsigned char>(2); // 字节 +0x01=金币类型
        goldItem.m_addInfo = 0;                            // uStack_1ef 清零
        goldItem.set_add_info(std::max<int>(1, goldAmt));  // ORIG 无条件先设一次

        if (role == 3)
        {
            unsigned short cnt = 0;                        // local_9a
            unsigned short volume = 0;                     // local_9c
            if (!_getBossGoldDrop(partyCol, cnt, volume))
            {
                result.m_dropItems.push_back(goldItem);
            }
            else
            {
                goldAmt = (static_cast<int>(volume) * goldAmt) / 100;
                goldItem.set_add_info(std::max<int>(1, goldAmt));
                for (int i = 0; i < static_cast<int>(cnt); ++i)
                    result.m_dropItems.push_back(goldItem);
            }
        }
        else
        {
            result.m_dropItems.push_back(goldItem);
        }
    }

    if (done)                                          // *(param_9) 非 0 → 结束
        return;

    // ---- 稀有装备主循环 tier 1..4 ----
    for (int tier = 1; tier <= 4; ++tier)
    {
        float marketFactor = 1.0f;                     // local_50（tier==2 被 rate 覆盖）
        int altTable[6] = {0};                         // ORIG rep stos 对齐剥离慢路

        int* rarityTable = &m_rarityBasis[tier][0];    // this + tier*0x18 + 0x30
        if (tier == 2)
        {
            // ORIG 两次独立 G_CDataManager()->GetAutoMarketContitionsControl()；
            // ChangeItemRarity 第 4 实参是立即数 6（非 tier，P97）
            marketFactor =
                G_CDataManager()->GetAutoMarketContitionsControl()->GetItemDropRate(
                    idx);
            double extra = 0.0;
            if (marketFactor != 0.0f)
                extra = 1.0 / marketFactor - 1.0;
            if (G_CDataManager()
                    ->GetAutoMarketContitionsControl()
                    ->ChangeItemRarity(&m_rarityBasis[tier][0], altTable, 6,
                                       extra))
                rarityTable = altTable;
        }

        float weight = static_cast<float>(t.v[2 + tier]);
        if (tier == 2)
            weight *= static_cast<float>(m_firstMobWeight) * m_firstMobWeightRate;

        int rollChance = static_cast<int>(
            (*reinterpret_cast<float const*>(&m_partyBonus[tier][0] + partyCol) *
             *reinterpret_cast<float const*>(&m_monsterTypeBonus[tier][0] + role) *
             *reinterpret_cast<float const*>(&m_dungeonBonus[tier][0] + difficulty) *
             weight * marketFactor)) * dropRate / 100;

        int randValue = m_random.generateNumber(0, 10000);
        if (rollChance < randValue)
            continue;

        int rarRoll = m_random.generateNumber(1, 1000000);
        int rarity = reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
                         ->GetItemRarity(rarityTable, rarRoll, tier);

        G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
            static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
            static_cast<eItemDropType_t>(tier),
            static_cast<ENUM_RARITY>(rarity));

        int itemIdx =
            m_table.chooseItem(static_cast<eItemDropType_t>(tier),
                               static_cast<ENUM_RARITY>(rarity), type);
        if (itemIdx == -1)
            continue;

        if (tier == 2)
        {
            // [A28 round5 · P312 实测反例 2026-09-10 19:55] 曾按 ORIG 0x085360xx 段
            //   （-0x1ed 起 0x100 字节对齐剥离慢路 rep stos）把本数组改为
            //   `char buf[256] = {0};`：**反劣**（ours 646→680，+34；4.4 的零初始化
            //   形与本处槽位/帧配合后长于 ORIG）⇒ 已回滚；维持无初始化形并封 N78。
            char buf[256];                             // -0xed 起 0x100 日志缓冲
            sprintf(buf,
                    "monster drop rate : item_id(%d), mon_lv(%d), role(%d), "
                    "dungeon_lv(%d), dungeon_diff(%d), member_cnt(%d), "
                    "bourgeois_rate(%d), drop_rate(%d), rand_value(%d), "
                    "drop_value(%d)",
                    itemIdx, static_cast<int>(type), static_cast<int>(role),
                    static_cast<int>(dungeonLv),
                    static_cast<int>(difficulty),
                    static_cast<int>(partyCol), context3c, dropRate, randValue,
                    rollChance);
            reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
                ->sendItemDropRate(buf);
        }

        CItem* item = G_CDataManager()->m_itemList->find_item(itemIdx);
        if (item == 0)
            continue;

        Inven_Item equip;                              // local_f1 缓冲
        equip.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), equip, 1);

        if (tier == 2)
        {
            // 非宠物(vtbl+0x14)且非头像(vtbl+0x10)才进入装备升档分支
            if (!item->IsCreatureItem() && !item->IsAvatarItem())
            {
                int grade = item->get_grade();
                equip.set_add_info(
                    m_upgrade.generateEquipGrade(type - grade));
            }
            // [b28d-solo-WongWorkDrop_A] 真语义 bug 修正：ORIG 0x0853676f
            // `test %al,%al; je <false>` + `hasAbility xor $1; je <false>` ⇒
            // 进入条件 = checkItemUpgradePossible()!=0 && hasAbility()!=1
            //（旧码写反为 !check && !hasAbility，极性整段互换）
            if (item->checkItemUpgradePossible() && !equip.m_amp.hasAbility())
            {
                int grade = item->get_grade();
                int upLv = m_upgrade.generateUpgrade(
                    static_cast<ENUM_RARITY>(item->get_rarity()), type - grade);
                if (upLv > 0)
                    equip.SetUpgrade(static_cast<unsigned char>(upLv));
            }
        }

        // ORIG 无 seq 局部：size() 结果留 %esi 跨 get_grade/get_rarity 保活
        reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
            ->UseLuckPoint(static_cast<int>(equip.m_field1),
                           item->get_rarity(), item->get_grade(),
                           static_cast<int>(result.m_dropItems.size()));
        result.m_dropItems.push_back(equip);
    }
}

// ---- _generateMonseterSpecificItem — ORIG T 0x08536fce ----
void CMonsterDrop::_generateMonseterSpecificItem(CMonster const* monster,
                                                 char idxType,
                                                 stGenerateResult_t& result)
{
    // ORIG 0x08536fce：itemIdx 两段式预置 -2（movl $-2,-0xc 后重赋）；
    // roll 无槽——generateNumber 结果 %eax 直传 0x8(%esp)（P0）；
    // pushCreatedItem helper 遮蔽 ORIG 内联体（Wide21B makeMemberLoginTable
    // 同案），本点展开：Inven_Item C1 → m_addInfo 槽回读 → itemList
    // create_item(1, inv, 1) → vector push_back(inv)。
    int itemIdx = -2;
    itemIdx = monster->drop_item_index(
        static_cast<unsigned int>(idxType), m_random.generateNumber(2, 10000));
    // ORIG cmpl -0xc;setne %al;test %al,%al;je 统一出口（无 bool 栈槽）——
    // P225 while+break 物化形（P0/N96：原 register asm("eax") 绑定已清除）。
    while (itemIdx != -2)
    {
        Inven_Item inv;
        inv.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
        result.m_dropItems.push_back(inv);
        break;
    }
}

// ---- generateRandomItem — ORIG T 0x08536da8（虚槽 +0x08）----
// b21 verdict=DIFF（N 残余）：158/158 指令数全等；u8_at/i8_at helper 在 O0 产 EXTRA call
//   （每个读调用 ~+6 insn，共 ~45），改用 stGenerateRefData_t 具名字节成员直读（m_field34..
//   m_field41，CItemGenerator.h union 字节视图）后消除；memcpy(&m_firstMobWeight,...) 小常数
//   复制产 call → m_firstMobHunt.m_weight/m_field04 直赋。残余仅 1) `cmp $0x3; jg` vs
//   `ja`（movzbl 已知非负，GCC 选无符号跳转，语义全等）2) 跳转/调用目标地址。语义完成。
void CMonsterDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                      stGenerateResult_t& result)
{
    // ORIG movzbl +0x35; cmp $0x3,%al; jg（movzbl 零扩展 + 8 位有符号比较；
    //   探针实证 (signed char)(unsigned char)x > 3 形）
    if (static_cast<signed char>(static_cast<unsigned char>(ref.m_field35)) > 3)
        return;

    bool done = false;                                  // local_25

    if (ref.m_field40_byte == 0)                        // hell 旗标低字节为 0
    {
        for (int n = 0; n < static_cast<char>(ref.m_field39); ++n)
        {
            m_special._generateRandomItem(
                static_cast<unsigned char>(ref.m_field34),
                static_cast<char>(ref.m_field35),
                static_cast<unsigned char>(ref.m_rateSlot),
                static_cast<unsigned char>(ref.m_pad37),
                ref.m_field38,
                ref.m_field3c,   // §4.3 具名化：+0x3c int 视图（CItemGenerator.h:224）
                ref.m_field41, ref.m_dropRate, result, done);
        }
    }

    for (int n = 0; n < static_cast<char>(ref.m_field39); ++n)
    {
        // forceTag 决定本轮首杀权重分量回退：[0]→hunt[0]，[1]→hunt[+4]
        if (ref.m_forceTag.operator[](0))
            m_firstMobWeight = m_firstMobHunt.m_weight;   // ORIG mov (m_firstMobHunt)→m_firstMobWeight
        else if (ref.m_forceTag.operator[](1))
            m_firstMobWeight = m_firstMobHunt.m_field04;  // ORIG mov (hunt+4)→m_firstMobWeight
        m_firstMobWeightRate = ref.m_field1c;           // dword ← ref+0x1c

        _generateRandomItem(static_cast<unsigned char>(ref.m_field34),
                            static_cast<char>(ref.m_field35),
                            static_cast<unsigned char>(ref.m_rateSlot),
                            static_cast<unsigned char>(ref.m_pad37),
                            ref.m_field38,
                            ref.m_field3c,   // §4.3 具名化：+0x3c int 视图
                            ref.m_dropRate, result, done);

        m_firstMobWeight = 1;
        m_firstMobWeightRate = 1.0f;                    // 0x3f800000 位还原
    }
}

// ---- generateSpecificItem — ORIG T 0x08537070（虚槽 +0x0c）----
// [b28d-solo-WongWorkDrop_A] verdict=AE：ORIG 布局重写（difficulty>4 高难度块
// 直落、jle 跳后部低难度块；旧早返形相反）+ 低难度块具名 ctx 指针（-0x20，
// +0x38 find_monster / +0x3d 循环上界 / +0x3c 末次判定）+ m_field43/
// m_field44_byte bool 化（P248 档② xor $1 形）。真语义 bug：find_monster
// 旧码取 ref.m_field35，ORIG 取 +0x38（m_rankBonusIndex）。
// b21 同族：u8_at/i8_at 改成员直读；+0x43/+0x44 改为 ORIG 语义 (byte!=1)&&(byte!=1)（原 ==0 误）。
//   残余：字节比较 codegen（ORIG xor$0x1 vs cmpb，N-86b 全同）+ AE。
void CMonsterDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    if (ref.m_field41 != 0)
    {
        // hell 旗标非零：地狱派对专用掉落
        m_hell.hellPartySpecificItem(ref, result);
        return;
    }

    // [b28d-solo-WongWorkDrop_A] ORIG 布局：`jle <低难度块>`（低难度块在
    // 函数后部 +0xfd），difficulty>4 高难度块直落；旧早返形（jg 跳高难度）
    // 与 ORIG 相反。
    if (static_cast<signed char>(ref.m_field3c_byte) > 4)
    {
        // 高难度：地狱派对普通 specific 循环 + AI Character 掉落
        for (int n = 0; n < static_cast<signed char>(ref.m_field3d); ++n)
            m_hell.generateSpecificItem(ref, result);

        // §4.3 具名化：dword@+0x38 上/index = m_rankBonusIndex（P0 内联实参，
        // ORIG 值留 %ebx 跨 G_CDataManager 调用，无 aiIdx 具名槽）
        CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(
            static_cast<unsigned int>(ref.m_rankBonusIndex));
        if (ai == 0)
            return;

        ai->generateIndependentItem(ref, result.m_dropItems);
        ai->generateDropAvatarItem(static_cast<unsigned char>(ref.m_rateSlot),
                                   ref.m_field3e_byte, result.m_dropItems);
        return;
    }

    // 低难度：ORIG 具名指针局部 ctx（-0x20）——find_monster/循环上界/
    // 末次 difficulty 判定均经 ctx 偏移读（+0x38/+0x3d/+0x3c）。
    StRefCtx2 const* ctx =
        reinterpret_cast<StRefCtx2 const*>(&ref.m_field34);
    CMonster* monster = static_cast<CMonster*>(
        G_CDataManager()->find_monster(ctx->m_rankBonusIndex));
    if (monster == 0)
        return;

    int unusedItemIdx = -1;        // ORIG movl $0xffffffff,-0x18（死局部，P171）
    (void)unusedItemIdx;

    // ORIG 无 script 具名槽：每次 getMonsterScript() 直链 empty()（P0/P183）
    if (!static_cast<STMonsterScript*>(monster->getMonsterScript())
             ->m_independentDrops.empty())
        monster->generateIndependentItem(ref, result.m_dropItems);

    monster->generateDropAvatarItem(static_cast<unsigned char>(ref.m_rateSlot),
                                    ref.m_field3e_byte, result.m_dropItems);

    if (!static_cast<STMonsterScript*>(monster->getMonsterScript())
             ->m_catchItems.empty() &&
        ref.m_field45 != 0)
        monster->generateCatchMonsterItem(result.m_dropItems);

    // ORIG 0x08537254 movzbl 0x43; xor $0x1; je → 两 bool 字段取反（P248 档②）
    if (!ref.m_field43 && !ref.m_field44_byte)
    {
        for (int c = 0; c < static_cast<signed char>(ctx->m_count); ++c)
            _generateMonseterSpecificItem(monster, 0, result);
    }
    if (ctx->m_difficulty != 0)
        _generateMonseterSpecificItem(monster,
                                      static_cast<char>(ctx->m_difficulty),
                                      result);
}

// ---- CMonsterDrop::stFirstMobHuntingWeight_t ctor — ORIG W 0x0853a6ae ----
WongWork::CMonsterDrop::stFirstMobHuntingWeight_t::stFirstMobHuntingWeight_t() throw()
{
    m_field04 = 1;
    m_weight = m_field04;                              // [+4]=1 后拷回 [+0]
}

}  // namespace WongWork
