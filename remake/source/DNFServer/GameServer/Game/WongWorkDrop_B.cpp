// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkDrop_B/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// P6 清桥后本 TU 无 __asm__ 桥：下述符号全部为真实成员/函数调用。
//   CMonsterDrop_Hell/CSpecialMonsterDrop::_getGenItemProbIdx → 自身成员内直呼；
//   CLuckPoint::{UseLuckPoint4int,sendItemDropRate} → 定义 CClearReward.cpp；
//   CMonster/CAICharacter 头像掉落、捕获包装与 IsHellMonster → CMonster.h /
//   MonsterIndependentDrop.h 权威声明（IsHellMonster 返回 bool，P292 判据）。
// ---------------------------------------------------------------------------

namespace {

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
#include "WongWorkDrop_B/setDropRatio.inc"
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

#include "WongWorkDrop_B/setDropRatio_1.inc"
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
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N7）→ 试=新口径 n1_v 157/156 gap=+1 blocks=5 + G1 具名 `unsigned char mf35` 局部（槽平移 −4 使 ndiff 19→62、hunk 4→18 ⇒ 回滚）；残差=choose_item/create_item 实参寄存器角色互换（ORIG call 果→%edx、inv.m_field1→%eax；4.4 反向）+ 二次 movzbl 折入 ⇒ cast/内联/掩码多形穷举不翻 ⇒ 维持。
#include "WongWorkDrop_B/generateSpecificItem.inc"
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

    int idx =
        _getGenItemProbIdx(static_cast<int>(static_cast<unsigned char>(ctx.m_field35)));
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
    // [N] P248 类型旋钮试（rarity 由 int 改 ENUM_RARITY）⇒ 读数不变（157/156），
    // 4.4 对同宽枚举与 int 发射一致；回滚。
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
    reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
        // [M3S2WinDrop=N] 试 `(int)(unsigned char)inv.m_field1` 复现 ORIG 0x0853591e 的
        // 二次 `movzbl %al,%eax`：读数不变（157/156）—— 4.4 把二次扩展折进首条，
        // 仍 `movzbl S(%ebp),%edx; movzbl %dl,%edx`。残余 1 条 = ORIG `mov %eax,%edx`
        // （载入用 %eax、旧值挪 %edx）vs 4.4 载入用 %edx，属寄存器分配 N。
        ->UseLuckPoint(inv.m_field1, item->get_rarity(), item->get_grade(),
                       static_cast<int>(result.m_dropItems.size()));
    result.m_dropItems.push_back(inv);
}

// ---------------------------------------------------------------------------
// hellPartySpecificItem — ORIG T 0x08535b08
// b21 同族：如 generateSpecificItem 注，成员直读替换 u8_at/i8_at（+0x3d 有符号 movsbl）。
// ---------------------------------------------------------------------------
#include "WongWorkDrop_B/hellPartySpecificItem.inc"
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
        if (ai->IsHellMonster() != 0)
            extraCount = 0;
        else
            extraCount =
                static_cast<int>(static_cast<signed char>(ref.m_field3d));
        ai->generateIndependentItem(ref, result.m_dropItems);
        ai->generateDropAvatarItem(static_cast<unsigned char>(ref.m_rateSlot),
                                   ref.m_field3e_byte, result.m_dropItems);
    }

    // ORIG 此处才绑定 ctx 指针局部（-0x14），并以其 +4 作 find_monster 实参。
    stGenerateRefCtx_t const& ctx =
        *reinterpret_cast<stGenerateRefCtx_t const*>(&ref.m_field34);
    CMonster* mon = static_cast<CMonster*>(
        G_CDataManager()->find_monster(ctx.m_rankBonusIndex));
    if (mon != 0)
    {
        if (mon->IsHellMonster() != 0)
            extraCount = 0;
        else
            extraCount =
                static_cast<int>(static_cast<signed char>(ref.m_field3d));

        // ORIG 无 script 具名局部（getMonsterScript 结果直接 +0x3ac 后调 empty）
        if (!static_cast<STMonsterScript*>(mon->getMonsterScript())
                 ->m_independentDrops.empty())     // +0x3ac
            mon->generateIndependentItem(ref, result.m_dropItems);

        mon->generateDropAvatarItem(static_cast<unsigned char>(ref.m_rateSlot),
                                    ref.m_field3e_byte, result.m_dropItems);

        // ORIG 短路物化无槽（mov $1/$0 岛 + 尾 test），具名 bool 会多槽+回读。
        if (!static_cast<STMonsterScript*>(mon->getMonsterScript())
                 ->m_catchItems.empty() &&
            ref.m_field45 != 0)                 // +0x70
            mon->generateCatchMonsterItem(result.m_dropItems);
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
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N85/N93）→ 试=新口径 n1_v 364/365 gap=-1 blocks=57：ORIG -0x48/-0x64.. 槽 vs 我方 -0x50/-0x6c..（全槽平移 8）+ x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp，N85）⇒ 维持。
#include "WongWorkDrop_B/_generateRandomItem.inc"
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
    // [M3S2WinDrop 2026-09-11] 本波落地 2 项（370→365，gap +6→+1；四数不变）：
    //   ① UseLuckPoint 三实参改**全内联**（去具名 grade/itemRarity 槽）——ORIG 由
    //      `call get_grade; mov %eax,%esi` / `call get_rarity; mov %eax,%ebx` 用
    //      callee-saved 保活 ⇒ -3 条；
    //   ② 去 probAcc 具名（乘法直接写在 cast 内）——ORIG 0x085369b3 由 x87 栈上值
    //      直接 `fnstcw/fldcw/fistpl`，无 float 槽往返 ⇒ -2 条。
    // 残余（判 N，编译器差异）：③ `t.v[2+i]` 我方多 1 条 `add $0x2,%eax`（ORIG
    //   `mov -0x5c(%ebp,%eax,4)` 直接以 eax=i 索引，4.4 不折叠常量基址）；
    //   ④ 后半段实参缓冲 ORIG 用 ebx/ecx/edx 而 4.4 用槽 + esi/edi（同条数）；
    //   ⑤ x87 比较 lowering（N85）；⑥ `jl` vs 4.4 `jg`（等价反向）。
    int probIdx = _getGenItemProbIdx(static_cast<int>(dungeonLv));
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
        // [M3S2WinDrop] 去 probAcc 具名：ORIG 0x085369b3 由 x87 栈上值直接 `fnstcw/
        // fldcw/fistpl`（无 float 槽往返）；具名 float 会让 4.4 多 `fstps S; flds S` 2 条。
        int probValue = static_cast<int>(
            static_cast<float>(t.v[2 + i]) * factor *
            m_dungeonBonus[i][difficulty] *
            m_monsterTypeBonus[i][static_cast<int>(role)] *
            m_partyBonus[i][partyPos]);              // fldcw truncate ≡ (int)
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
            reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
                ->sendItemDropRate(buf);
        }

        // ORIG 同型四连 + UseLuckPoint：Inven_Item ctor / 写 item idx /
        // create_item(1,&inv,1) / get_grade+get_rarity+UseLuckPoint / push_back。
        Inven_Item inv;                                // ORIG 内联 C2
        inv.m_addInfo = itemIdx;                       // ORIG 写对象首 +0x02
        G_CDataManager()->m_itemList->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
        // [M3S2WinDrop] ORIG 0x08536b?? 三实参均内联：`call get_grade; mov %eax,%esi`
        // / `call get_rarity; mov %eax,%ebx`（callee-saved 保活，无具名槽）。
        reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
            ->UseLuckPoint(static_cast<int>(inv.m_field1), item->get_rarity(),
                           item->get_grade(),
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
#include "WongWorkDrop_B/generateEquipGrade.inc"
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
#include "WongWorkDrop_B/generateUpgrade.inc"
{
    // [M3FreeNext15B 复验·回滚] ItemFarmingData.h 拷贝 ctor 加 `throw()` 的 **A/B 实测**
    // （父泳道 P340 真基线反演：字面量反转重编取 base .o，base/after 在该符号区间
    // **逐指令完全相同** —— insns 233=233、`_Unwind_Resume` **1=1**）⇒ 本 LP **并非**
    // 源自拷贝 ctor 的具/不可见，而是**本函数内具名局部 `entry` 的 dtor cleanup**
    // （函数体内其余调用可抛）；ORIG 无 LP 属 4.1 侧判定差，4.4 -O0 无合法源码解。
    // ⇒ throw() 按闸门（无 gap/ndiff 改善）已回滚，.h 恢复原状；旧 EH 普查判读更正。
    // @ORIG 0x08533676
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
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N93 + N-88 setne 族）→ 试=①守卫包裹形 `if(x==-1){}else{return x;}`（C1）读数不变；②bool 具名局部（C2）our 210→213 劣化回滚；③iterator 直接初始化（C3）our 210→204 且 p167 EXTRA `_Rb_tree_iteratorC1Ev` 消除，但 |gap| −2→+4、ndiff 81→80 ⇒ 回滚 ⇒ 维持（ORIG 0x08534fce+0x18 `cmpl;setne;test;je` 物化不可造；槽平移 0x14 独立存在）。
#include "WongWorkDrop_B/chooseItem.inc"
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
        // [M3S2WinDrop] ORIG 0x08535105 拷贝槽对（-0x24 → -0x40）：`mov (%eax),%eax;
        // mov %eax,-0x24; mov -0x24,%eax; mov %eax,-0x40`，第二个 ulong 局部取地址
        // 传给 randInt（引用参数）。
        unsigned long maxKeyRef = maxKey;
        // [M3S2WinDrop] ORIG 0x08535179 `lea -0x4c,%ecx; mov %ecx,0x8(%esp)`（直接传
        // roll 槽地址给 lower_bound）⇒ roll 类型 = map key_type（unsigned int），
        // 用 unsigned long 会多出一次截断临时（`mov S,%eax; mov %eax,S`）。
        unsigned int roll = m_rand->randInt(maxKeyRef);
        // ORIG 在 randInt 之后才 `movl $0xffffffff,-0x20`（result 槽）。
        result = -1;
        // [M3S2WinDrop] ORIG 0x0853518c 后置拷贝：lower_bound 返回先落临时槽(-0x5c)，
        // 再 `mov -0x5c,%eax; mov %eax,-0x48` ⇒ 先声明后赋值（非初始化式）。
        std::map<unsigned int, int>::iterator lb;
        lb = m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].lower_bound(roll);
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
#include "WongWorkDrop_B/_getGenItemProbIdx.inc"
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

#include "WongWorkDrop_B/_getGenItemProbIdx_1.inc"
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
