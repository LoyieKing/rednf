// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StubS3A_deps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// StubS3A_deps.cpp —— StubS3A 主批 NEEDS_DECL 依赖 producer（StubS3A.S3A_DEPS 批）。
// 全部定义按 ORIG 汇编/报告忠实转写（C++98 / 32 位）。
// 需宿主登记 CMake（source/cmake/game/CMakeLists.txt）。
#include <string.h>

#include "GlobalData.h"      // GlobalData::{s_stream_pool,s_msgq_mgr}
#include "MsgQueueMgr.h"     // MsgQueueMgr::DB_Q / put

#include "CUser.h"           // CUser::{GetCurCharacInventoryRef,GetCharacExpandDataR,
                             // getCurCharacQuestW,IsAbleItem* 声明}
#include "CDataManager.h"    // G_CDataManager()->{find_item,find_quest,m_emotionTags}
#include "CItem.h"           // CItem::{is_stackable,GetItemType,GetAttachType}
#include "Inven_Item.h"      // Inven_Item::{m_field0,m_addInfo,m_amp}
#include "CItemAmplifier.h"  // stAmplifyOption_t::GetLock
#include "CTitleBook.h"      // item_lock::CItemLock::{IsAbleItemLock,IsAbleItemUnlock,IsAbleItemUnlockCancel}
#include "CInventory.h"      // CInventory::{GetInvenSlot,delete_item}（startUseMaterial）
#include "CraneMinigameManager.h"  // CraneMinigameManager::startUseMaterial 声明
#include "Quest.h"           // Quest::m_intDataList（+0x2c）

// 【A 档 GI 顺序对齐】下列头原本在文件中途 include（供 CItemMakingSkill ctor 使用）；
// 该 ctor 前置到 TU 最前后需在此可见。含 include guard ⇒ 中途处重复 include 无副作用。
#include "CItemMakingSkill.h"
#include "PvPTypes.h"                 // CMTRand（CItemMakingSkill ctor）
#include <ctime>

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN16CItemMakingSkillC2Ev`（P327）。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// [b28d-62] AE：time(0) 内联实参（临时槽供 const& 绑定）复现 4.1 new 前预取
//   %esi；48=48。ORIG 0x08545108
#include "StubS3A_deps/CItemMakingSkill.inc"
{
    // ORIG 0x08545125：time(0) → operator new(0x9c8) → CMTRand(seed)
    // b28c-L5 代落判词（lane14 件，DELEGATED→本 TU 属主；根因由 lane14 探针定）：
    //   本件 48/48 指令数全等，唯一残差在 EH/预取段——ORIG 0x08545134 在
    //   `call time@plt` 之后**立刻** `lea -0x1c(%ebp),%esi` 预取 &seed（4.1 初始化
    //   前置，N88 族），4.4 则把该 lea 延到 `call CMTRand::C1ERKm` 前一条且落
    //   %edx（惰性求值 + callee-saved 选择差）。new-expression 的 landing-pad
    //   抑制已由 P210/BM11 覆盖（caller 前置于 callee ctor 定义）——本 TU 看不到
    //   CMTRand ctor 体（定义在 CUserCharacInfo 链外 TU），R3 跨 TU 迁定义不可行
    //   ⇒ 判 N88+N18a，语义零差，勿再开轮。
    // [b28d-62] 试临时实参形：ORIG 0x08545131..0x0854514b 为
    //   `call time; mov %eax,-0x1c(%ebp); lea -0x1c(%ebp),%esi; new; CMTRand(esi)`
    // ⇒ 原码把 time(0) 作**实参内联表达式**（临时槽 -0x1c 供 const& 绑定），
    //   4.1 在 new 之前预取地址入 %esi；具名 seed 形 4.4 后置 lea（历史判词）。
    m_rand = new CMTRand(time(0));
}

#pragma pack(push, 1)
// SIG_ITEM_MAKING_SKILL 协议视图（§4.3）：+0=characNo、+4=skills[11] ushort
struct ItemMakingSkillReqView
{
    // [M2S2 2026-09-11] P211/N76 反形异型化重测：ORIG 0x08545335 该 store 为
    //   RHS-first + `mov %eax,%edx` 中转；同 TU 的 RedeemListHeadView::m_characNo
    //   用同法（int→unsigned）实测翻正 ⇒ 此处同条件重试一次。
    unsigned int m_characNo;         // +0x00（RHS `pUser->get_charac_no(-1)` 为 int）
    // [b28d-63 本 TU] 宽度/符号：ORIG loadData 0x08545234 为 `movzwl; cmp $0x3e7,%ax; jle`
    //   ⇒ 载入零扩展 + 有符号 16 位比较（比较点用 (short) cast 表达，字段仍 unsigned
    //   以与 m_skillLevels 同型、令 else 直落 %ecx，见 loadData 判词）。
    unsigned short m_skills[0xb];    // +0x04（11 项）
};
#pragma pack(pop)

// ---------------------------------------------------------------------------
// 1. CUser::IsAbleItemLock(int,int) const —— ORIG T 0x08646a9c
//    报告 docs/class_func_reports/CUser/IsAbleItemLock.md 全文转写。
//    0x15=21 无物品/无 item 定义；0x13=19 业务不可锁；0x11=17 attach 型不可锁。
// [b28d-62] N82：p167 CLEAR。ORIG 0x08646a9c
#include "StubS3A_deps/IsAbleItemLock.inc"
{
    const Inven_Item* item =
        GetCurCharacInventoryRef((unsigned short)space, (unsigned short)slot);
    if (item == 0)
        return 0x15;
    CItem* it = G_CDataManager()->find_item(item->m_addInfo);
    if (it == 0)
        return 0x15;
    if (it->is_stackable())
        return 0x13;
    // [b28d-63 本 TU] P225 推广：ORIG 0x08646b2c 两条件各自物化
    //   （`cmp $0x16; sete; test; je L` + `movzbl; xor $1; test; je L`，L=then 之后）
    //   ⇒ 拆嵌套 if；`&&` 合写在 4.4 恒折成短路直跳 + bool 汇合（多 4 条）。
    if (it->GetItemType() == 0x16)
    {
        if (item->m_field0 != 1)
            return 0x13;
    }
    if (it->GetItemType() == 0xb && it->GetAttachType() == 2)
        return 0x11;
    if (space == 0xc)
        return 0x13;
    unsigned char lock = item->m_amp.GetLock();
    if (lock != 0)
        return 0x13;
    // ORIG 0x08646bb6：result 声明点预清 0（movl $0x0,-0xc）先于 GetCharacExpandDataR；
    // 结果免槽直传（ORIG 无 expand 局部），返回值落 result 槽后按 P225 X3 判返。
    // GetCharacExpandDataR(this,2) 返回数据区指针，直接作为
    // item_lock::CItemLock* this 传入 IsAbleItemLock（ORIG 同款怪癖）。
    int result = 0;
    result = ((item_lock::CItemLock*)GetCharacExpandDataR(
                  (ENUM_CHARAC_EXPAND_TYPE)2))
                 ->IsAbleItemLock(lock);
    while (result != 0) {
        return result;
    }
    return 0;
}
// [b28d-63 判词] DIFF→**NEAR 37=37**（p167 CLEAR）：拆嵌套 if + result 预清/判返后
//   逐段同形；唯一残差 = 两处 `call *%eax` vs ORIG `call *%edx`（N7 间接 call 目标
//   寄存器，4.4.7 恒 %eax；4.4.6 后端可翻，路由归 Main）。
// [b28d-63 虚槽核对·Main 全库排查] 本函数两处间接调用槽 = vptr+0xc：
//   _ZTV14CStackableItem@0x08c96ea0 +0x14 = 0x08514a84 = CStackableItem::GetItemType
//   （非 vptr+0x10 IsAvatarItem@0x08514a92 / vptr+0x14 IsCreatureItem@0x08514a9c）
//   ⇒ 槽号与语义一致，无取错。全 TU 另两处 = vptr+0x1c（_reset），
//   _ZTV16CItemMakingSkill@0x08c9c340 +0x24 = 0x085453ba = CItemMakingSkill::_reset ✓。

// ---------------------------------------------------------------------------
// 2. CUser::IsAbleItemUnlock(int,int,int&) const —— ORIG T 0x08646bfe
//    报告 docs/class_func_reports/CUser/IsAbleItemUnlock.md 汇编为准
//    （反编译 C 的 param_2/param_3 有笔误，栈槽 0xc=space / 0x10=slot / 0x14=outSec）。
#include "StubS3A_deps/IsAbleItemUnlock.inc"
{
    const Inven_Item* item =
        GetCurCharacInventoryRef((unsigned short)space, (unsigned short)slot);
    if (item == 0)
        return 0x15;
    if (space == 0xc)
        return 0x13;
    unsigned char lock = item->m_amp.GetLock();
    if (lock == 0)
        return 0x13;
    // ORIG 0x08646c5f：result 声明点预清 0（movl $0x0,-0xc），随后独立赋值；
    // lock 于声明点直落 callee-saved %ebx 跨 GetCharacExpandDataR 存活；
    // expand 结果免槽直用（技巧22/37）。
    // 【真语义 bug 修复·b28c-L5】原形把 CItemLock::IsAbleItemUnlock 的返回值
    //   **丢弃**（`if (call() != 0) return result;` 而 result 恒为预清的 0）；
    //   ORIG 0x08646c5f 是 `call / mov %eax,-0xc(%ebp) / cmpl $0,-0xc(%ebp) / …/
    //   mov -0xc(%ebp),%eax / jmp 尾` —— 命中失败码时返回**该码**（0/0x13），
    //   即调用结果被落进 result 槽并作为返回值透出。改为「声明预清 + 独立赋值 +
    //   按值判返」（P102 同一 bool 局部逐次赋值再判断 的 int 版；同时复现
    //   ORIG 的 `mov %eax,-0xc` 落槽与 `cmpl $0,-0xc` 槽比）。
    int result = 0;
    result = ((item_lock::CItemLock*)GetCharacExpandDataR(
                  (ENUM_CHARAC_EXPAND_TYPE)2))
                 ->IsAbleItemUnlock(lock, outSec);
    // [b28d-62] P225 X3 形：ORIG 0x08646c5f 为 `cmpl $0,-0xc; setne %al;
    //   test %al,%al; je <mov $0 岛>`（4.1 槽比 bool 物化）；4.4 对 if 恒直跳
    //   ⇒ 写 while+break 精确复现（语义等价：条件真时立即 return）。
    while (result != 0) {
        return result;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// 3. CUser::IsAbleItemUnlockCancel(int,int) const —— ORIG T 0x08646cae
//    报告 docs/class_func_reports/CUser/IsAbleItemUnlockCancel.md 全文转写。
#include "StubS3A_deps/IsAbleItemUnlockCancel.inc"
{
    const Inven_Item* item =
        GetCurCharacInventoryRef((unsigned short)space, (unsigned short)slot);
    if (item == 0)
        return 0x15;
    unsigned char lock = item->m_amp.GetLock();
    if (lock == 0)
        return 0x13;
    // ORIG 0x08646d02：result 声明点预清 0 + 独立赋值；lock 直落 %ebx 跨调用；
    // expand 结果免槽直用（同 IsAbleItemUnlock 形）。
    // 【真语义 bug 修复·b28c-L5】同上：ORIG 返回落槽后的调用结果（0/0x13），
    //   原形恒返 0 ⇒ 上层 Dispatcher 的「可解锁」判定被永久放行（可观察行为差异）。
    int result = 0;
    result = ((item_lock::CItemLock*)GetCharacExpandDataR(
                  (ENUM_CHARAC_EXPAND_TYPE)2))
                 ->IsAbleItemUnlockCancel(lock);
    // [b28d-62] P225 X3 形（同 IsAbleItemUnlock）：ORIG 0x08646d2f `cmpl
    //   $0,-0xc; setne %al; test %al,%al; je <mov $0 岛>` ⇒ while+break。
    while (result != 0) {
        return result;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// 4. _postCheckForceChangeGrowType4AheadQuest(CUser*,Quest*) —— ORIG T 0x086686ed
//    objdump 0x086686ed..0x08668823 逐指令转写（无独立报告；调用方
//    _postCheckForceChangeGrowType 见 CUser.cpp:8029 区）。
//    quest->m_intDataList(+0x2c, vector<vector<int>>) 每组逐 questIdx：
//    find_quest 后递归本函数，并 getCurCharacQuestW()+4 处 CQuestClear
//    setClearedQuest(questIdx)。
// [b28d-62] N93/P50：ORIG 有 normal_iterator 默认构造 + vector<vector<int>>::
//   operator[]（先声明后赋值形），我方缺；85/82。ORIG 0x086686ed
// [b28d-63 本 TU] p167 三缺口实证（ORIG calls=15 / our=12）：
//   ① MISSING `normal_iterator::C1Ev` ⇒ `iterator it;` 先声明、内层 `it = begin()`（P50 复活）；
//   ② MISSING `vector<vector<int>>::operator[]` ×1 ⇒ 删 groups/group 引用缓存（P183），
//      内层 cond/body 各自重读 `quest->m_intDataList[gi]`；
//   ③ 删 cleared 具名指针（ORIG 无该槽，questIdx 经 %ebx 跨 getCurCharacQuestW 保活 ⇒ P0 内联）。
//   ⇒ 结果 **AE 85/85**（p167 仅剩同址别名 C1Ev/C2Ev 与自调用重定位伪差）；gi/questIdx
//   槽序按 P212 全头部声明（questIdx 先声明占深槽 -0x14、gi -0x10）复现。
#include "StubS3A_deps/_postCheckForceChangeGrowType4AheadQuest.inc"
{
    if (quest->m_intDataList.empty())   // ORIG 0x086686f4：mov 0xc(%ebp);add $0x2c;call empty
        return;
    // ORIG 0x086870a：empty 检查后 `lea -0x1c; call normal_iterator::C1Ev`（P50 先声明）
    std::vector<int>::iterator it;
    // P212：ORIG 槽序 gi(-0x10) 浅于 questIdx(-0x14) ⇒ 全头部声明、questIdx 先声明（深）。
    int questIdx;
    for (unsigned int gi = 0; gi < quest->m_intDataList.size(); ++gi)
    {
        for (it = quest->m_intDataList[gi].begin();
             it != quest->m_intDataList[gi].end(); ++it)
        {
            questIdx = *it;
            Quest* q = G_CDataManager()->find_quest(questIdx);
            if (q == 0)
                continue;
            _postCheckForceChangeGrowType4AheadQuest(user, q);
            // ORIG 0x086878f：`mov -0x14,%ebx` 先物化 questIdx，再算 receiver（单语句内联）。
            reinterpret_cast<WongWork::CQuestClear*>(
                reinterpret_cast<char*>(user->getCurCharacQuestW()) + 4)
                ->setClearedQuest((unsigned int)questIdx);
        }
    }
}

// ---------------------------------------------------------------------------
// 5. trim(char*) —— 定义在 GameUtils.cpp（ORIG T 0x0829f0ef），此处不重复。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 6. CraneMinigameManager::startUseMaterial(CUser*,int) —— ORIG T 0x080ebe06
//    （无独立报告，d_aa_body.cpp objdump 0x080ebe06..0x080ebcbb 转写）。
//    this+0x10（m_field10）=材料 item idx；this+0x14 低 16 位符号扩展=数量。
//    0x11=材料不符；0x16=数量校验/删除失败；0=成功。
// [b28d-62] N82：p167 CLEAR。ORIG 0x080ebe06
// [b28d-63 本 TU] P0/§4.3：删 inven/invenW/needCnt 三个具名局部（ORIG 全内联，
//   帧 0x78→ORIG 0x64），裸偏移改具名字段 m_material(+0x10)/m_field14(+0x14)
//   （ORIG `mov 0x10(%eax),%eax` / `movzwl 0x14(%eax),%eax` 单条 disp）。
#include "StubS3A_deps/startUseMaterial.inc"
{
    // ORIG 0x080ebe0d：getCurCharacInvenR() 结果免槽直传 GetInvenSlot。
    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
    if (item.m_addInfo != m_material)                       // ORIG 0x080ebe3f
        return 0x11;
    // ORIG 0x080ebe4d：needCnt 免槽（movzwl+cwtl 直读），比较用寄存器。
    if (item.m_addInfo2 < (int)(short)m_field14)            // ORIG jge 0x080ebe5a
        return 0x16;
    // ORIG 0x080ebe63：needCnt 第二次重读（movzwl+movswl %ax,%ebx），receiver 免槽。
    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, slot, (int)(short)m_field14,
            (eItemDelReason)0x34, true))
        return 0x16;
    return 0;
}

// ---------------------------------------------------------------------------
// 7. Redeem_Item::CRedeemItem::_reset —— ORIG T 0x085f7b6e（0x16）
#include "Redeem_Item__CRedeemItem.h"   // Redeem_Item::CRedeemItem
#include "CItemMakingSkill.h"

//    list clear（this+8）。
// 8. Redeem_Item::CRedeemItem::loadData(CUser*, char*) —— ORIG T 0x085f7830
//    （0x8c）：虚调 _reset；count=*(int*)(pData+4)；i=0..count-1：
//    *(int*)(pData + i*0x44 + 0xa) == 0 → alter()；否则 push_back(
//    Inven_Item_Expand_RedeemInfo 副本 @ pData + i*0x44 + 8)。返回 1。
// 9. Redeem_Item::CRedeemItem::_saveData(CUser*) —— ORIG T 0x085f7998
//    （0x1d5）：StreamPool::Acquire("Redeem_Item.cpp",0x108) → guard<<0x1f4
//    → <<GetUID → sig=GetInBuffer<SIG_REDEEMINFO_LIST_TO_DB>()（0x59c）→
//    null 则返 0 → memset 0x59c → characNo=get_charac_no(-1) → count=list
//    size → 逐项 rep movsl 0x11 dword → sig+8+i*0x44 → put(s_msgq_mgr,2)
//    → 返 1。
//    SIG_REDEEMINFO_LIST_TO_DB：characNo@0 / count@4 / entries[21]@8
//    （21 * 0x44 = 0x594；8 + 0x594 = 0x59c）。
// ---------------------------------------------------------------------------

#include "PvPTypes.h"                 // CMTRand（CItemMakingSkill ctor）
#include <ctime>
#include "StreamPool.h"

namespace Redeem_Item
{

#pragma pack(push, 1)
// [b28d-63 本 TU] Redeem 行视图：ORIG loadData 以 0x44 步长扫描，
//   check 在 +0xa（`mov 0xa(%eax,%edx,1),%eax`）；头部 count 在 +4
//   （`mov 0x4(%eax),%eax`）。具名视图复现 scaled/disp 寻址（P291/P311）。
union RedeemEntryView
{
    struct Head
    {
        char m_pad00[0xa];   // +0x00..+0x09
        int m_check;         // +0x0a
        char m_pad0e[0x36];  // +0x0e..+0x43
    } m_head;
    struct Body
    {
        char m_pad00[8];     // +0x00..+0x07
        char m_info[0x3c];   // +0x08（push_back 源地址，ORIG `lea 0x8(%eax),%edx`）
    } m_body;
};
struct RedeemListHeadView
{
    // [M2S2 2026-09-11] P211/N76 反形异型化：ORIG 0x085f7a63 对 characNo store 为
    //   RHS-first + `mov %eax,%edx` 中转（`mov %eax,%edx; mov -0x20(%ebp),%eax;
    //   mov %edx,(%eax)`），我方同型（int 字段 ← int 返回）恒 LHS-first。字段改
    //   unsigned（RHS 仍 int = get_charac_no 返回值）⇒ 异型转换中转，翻回 ORIG 序。
    unsigned int m_characNo; // +0x00（RHS `pUser->get_charac_no(-1)` 为 int）
    unsigned int m_count;    // +0x04
};
#pragma pack(pop)
static_assert(sizeof(RedeemEntryView) == 0x44, "Redeem entry stride drift");

#include "StubS3A_deps/_reset.inc"
{
    m_redeemList.clear();   // ORIG 0x085f7b7d: std::list::clear(this+8)
}

// [b28d-62] 修：3 处 memcpy→直读/结构赋值（P167/N84），去 memcpy call 与
//   多余 ctor。ORIG 0x085f7830
// [b28d-63 本 TU] P0/P48/N84：ORIG 0x085f7848 把 pData 拷入具名局部 -0x10；
//   count 每轮重读（无局部）、check/entry 免槽、push_back 直接传 pData 内地址
//   （无 info 临时对象）；i 为 unsigned（cond `cmp;seta`）。原形多 4 局部 + 0x44
//   临时 + 3 callee-saved push（帧 0x7c vs ORIG 0x28）。
// [b28d-63 判词] 帧/指令数已与 ORIG 对齐（0x28；check 段 7=7、else 段 11=11），
//   verdict 仍 DIFF，残差全为后端形态：①ORIG 0x085f7857 提前载 buf 到 %edx 用
//   `mov 0xa(%eax,%edx,1)`，4.4 恒 `add -0x10(%ebp),%eax; mov 0xa(%eax)`；
//   ②ORIG 0x085f788c `lea 0x8(%eax),%edx`，4.4 恒 `add $0x8,%eax`（N102/N4 族，
//   union 视图/指针算术/数组下标三形均不翻）。gap≤0，非缺段。
#include "StubS3A_deps/loadData.inc"
{
    (void)pUser;
    _reset();   // ORIG 0x085f7846：虚调 vtable slot7
    const RedeemEntryView* buf =
        reinterpret_cast<const RedeemEntryView*>(pData);   // ORIG 0x085f7848
    for (unsigned int i = 0; i < ((const RedeemListHeadView*)buf)->m_count; ++i)
    {
        if (buf[i].m_head.m_check == 0)   // ORIG 0x085f7867 mov 0xa(%eax,%edx,1)
        {
            alter();   // ORIG 0x085f7875: charac_expand::CData::alter()
        }
        else
        {
            // ORIG 0x085f788c：lea 0x8(%eax),%edx 直传源地址（无临时对象）。
            m_redeemList.push_back(
                *reinterpret_cast<const Inven_Item_Expand_RedeemInfo*>(
                    buf[i].m_body.m_info));
        }
    }
    return true;
}

// [b28d-62] 修：CStreamGuard operator*/-> 链 + 非 const list iterator + 结构赋值
//   （P103/P132/P96/N84）。ORIG 0x085f7998
// [b28d-63 判词] 已修：stream 具名局部删（P259 内联实参）、日志串改 ORIG 原文
//   "RedeemItem.cpp"（原 "Redeem_Item.cpp"）、头部字段改 RedeemListHeadView 视图。
// [M2S2 2026-09-11 三项修正] ①旧「槽号整体 +4（N95/N126）」作废：根因是多余的第二个
//   具名局部 `char* sigBuf`（两个指针槽 -0x24/-0x20，ORIG 只一个）——删 sigBuf、
//   局部直接用视图指针类型后，**全部 5 槽与 ORIG 逐一命中**（guard -0x2c / begin -0x30 /
//   end -0x24 / sig -0x20 / i -0x1c），134→133 条。②characNo store 用 P211 字段异型化
//   （RedeemListHeadView::m_characNo int→unsigned）翻回 ORIG 的 RHS-first + %edx 中转，
//   与 count store 一并逐条同形。③唯一残差 = 循环体求值序（ORIG 先 `call operator*`
//   取源、再算目的地址并把 i 升格 %ebx；4.4 恒先算目的地址入 %ebx）—— P226 裸语句
//   表达式包 RHS 实测零影响（133 条不变，已回滚）⇒ N76/N18a 族后端求值序，gap -2。
#include "StubS3A_deps/_saveData.inc"
{
    // [b28d-63 本 TU] P259：Acquire 结果内联进 CStreamGuard 实参（ORIG 无 stream 槽）；
    //   文件名按 ORIG 0x085f79ae 为 "RedeemItem.cpp"（原 "Redeem_Item.cpp" 为日志串错）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("RedeemItem.cpp", 0x108), true);
    // [b28d-62] P103/P132 + P96：ORIG 走 operator* 2 次 + operator-> 1 次
    //   （p167 MISSING 实证）；list 迭代器用**非 const** iterator（const_iterator
    //   多 2 次转换构造 + 整套 const 版 operator*/*++/!= = EXTRA 实证）。
    **guard << (int)0x1f4;
    **guard << (int)pUser->GetUID();

    // [M2S2 2026-09-11] P0/P183/P329：原源码有两个具名局部（`sig` + `char* sigBuf`）
    //   占 -0x24/-0x20 两槽，ORIG 只有一个指针槽（-0x20）⇒ 每处使用多一轮
    //   `mov -0x24(%ebp),%eax; mov %eax,-0x20(%ebp)` 槽往返。现把 cast 内联到
    //   使用点、局部本身即视图指针类型（与 CItemMakingSkill::_saveData 同手法），
    //   复现 ORIG 单槽 + RHS-first store 形。
    RedeemListHeadView* sig = reinterpret_cast<RedeemListHeadView*>(
        guard->GetInBuffer<SIG_REDEEMINFO_LIST_TO_DB>());
    if (sig == 0)
        return false;
    memset(sig, 0, 0x59c);
    // SIG_REDEEMINFO_LIST_TO_DB 布局：characNo@+0 / count@+4 / entries[21]@8。
    // [b28d-63 本 TU] 视图字段（disp store）：ORIG 0x085f7a78 为
    //   `call size; mov -0x20(%ebp),%edx; mov %eax,0x4(%edx)`（无 lea）。
    sig->m_characNo = pUser->get_charac_no(-1);
    sig->m_count = (int)m_redeemList.size();
    int i = 0;
    for (std::list<Inven_Item_Expand_RedeemInfo>::iterator it =
             m_redeemList.begin();
         it != m_redeemList.end(); ++it, ++i)
    {
        // [b28d-62] N84：ORIG 用结构整体赋值（rep movsl），显式 memcpy 生 call。
        // [M2S2 2026-09-11 试形·负结论] P226 裸语句表达式包 RHS（`= ({ *it; })`）
        //   实测对循环体求值序**零影响**（仍先算目的地址入 %ebx 后 call operator*，
        //   133 条不变）⇒ 回滚。ORIG 0x085f7aa4 的「call 先 / dst 后 + `mov %edx,%ebx`
        //   两条 RHS 保活」属 4.1↔4.4 求值序（N76/N18a 族），源码不可控。
        *(Inven_Item_Expand_RedeemInfo*)(
            reinterpret_cast<char*>(sig) + 8 + i * 0x44) = *it;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

}  // namespace Redeem_Item

// ---------------------------------------------------------------------------
// 10. CItemMakingSkill 生命周期 + CData 纯虚覆盖
//     ctor 0x08545108：CData ctor → vptr → +0x1c = new CMTRand(time(0))。
//     dtor（D1 0x08545192）：置 vptr → delete m_rand → 置空 → 基类 dtor。
//     loadData 0x08545202：虚调 _reset；i=0..0xa：v=*(ushort*)(pData+4+i*2)；
//       v>0x3e7 → m_skillLevels[i]=0x3e8，否则=v。返回 1。
//     _saveData 0x08545276：StreamPool::Acquire("CItemMakingSkill.cpp",0x33)
//       → guard<<0x26e → <<GetUID → sig=GetInBuffer<SIG_ITEM_MAKING_SKILL>
//       （memset 0x1c）→ characNo=get_charac_no(-1) → skills[11] 复制
//       （this+6 ↔ sig+4，ushort ×11）→ put(s_msgq_mgr,2) → 返 1。
//     _reset 0x085453ba：memset(this+6, 0, 0x16)（m_skillLevels[11]）。
// ---------------------------------------------------------------------------

#include "StubS3A_deps/dtor_CItemMakingSkill.inc"
{
    if (m_rand != 0)
        delete m_rand;
    m_rand = 0;
}

// [b28d-62] 修：memcpy→movzwl 直读（P167），去 memcpy call。ORIG 0x08545202
// [b28d-63 本 TU] P0/P48：ORIG 0x0854521a 把 pData 拷入具名局部 -0x10（源码有 `char* buf`），
//   循环体对源**两次独立直读**（if 与 else 各一次，无 v 槽）；改为视图 scaled 索引
//   `0x4(%eax,%edx,2)` 复现 ORIG 寻址（原 add 链 + v 槽 = 我方多 2 条）。
// [b28d-63 判词] DIFF→**NEAR 37=37**（gap 1→0）：视图字段 + `(short)` 比较点 cast
//   复现 `movzwl; cmp $0x3e7,%ax; jle`，else 直落 %ecx；唯一残差 = _reset 虚调
//   `call *%eax` vs ORIG `call *%edx`（N7）。
#include "StubS3A_deps/loadData_1.inc"
{
    (void)pUser;
    _reset();   // ORIG 0x08545218：虚调 vtable slot7
    // ORIG 0x0854521a 槽 -0x10 存 pData 指针；具名为视图指针以复现 `i` 先于基址装载。
    const ItemMakingSkillReqView* buf =
        reinterpret_cast<const ItemMakingSkillReqView*>(pData);
    for (int i = 0; i <= 0xa; ++i)
    {
        // ORIG 0x08545229：两次独立直读（P48）。
        // (short) cast 复现 ORIG 有符号 16 位比较（0x08545234 `cmp $0x3e7,%ax; jle`）。
        if ((short)buf->m_skills[i] > 0x3e7)
            m_skillLevels[i] = 0x3e8;
        else
            m_skillLevels[i] = buf->m_skills[i];
    }
    return true;
}

// [b28d-62] N18a：历史判词保留（死 regmove 两段，三档类型旋钮已穷举）。
//   ORIG 0x08545276
#include "StubS3A_deps/_saveData_1.inc"
{
    // b28c-L5 代落判词（lane14 件，DELEGATED→本 TU 属主）：87→86，残差为
    //   `mov disp(%eax),%eax; mov %eax,%edx` 两段死 regmove —— 目标 %edx 是
    //   caller-saved 且值不跨 call 保活，按 lane14/lane28 终版口径**不适用**裸
    //   register（裸 register 只在目标为 callee-saved 且跨 call 时可能非劣化）；
    //   三档类型旋钮（callee 返回类型 / 字段同异型 / store 方向）已由 lane14 穷举
    //   ⇒ 判 N18a 封顶。本文件内我波次真修的是 IsAbleItemUnlock/Cancel 两处
    //   「返回值被预清局部吞掉」的语义硬伤（见其上函数头判词）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CItemMakingSkill.cpp", 0x33),
                       true);
    guard.operator*()->operator<<((int)0x26e);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<((int)pUser->GetUID());

    // ORIG：GetInBuffer 结果落唯一槽 -0x10（sigBuf）→ memset 0x1c →
    // get_charac_no(-1) 返回值经 %edx 直存 *(int*)sigBuf（值先地址后，无 callee-saved
    // 缓存）。skills[11]@+4（ushort ×11）循环内槽直读重物化。
    // [b28d-63 本 TU] 具名局部用视图指针类型：ORIG 0x0854533a 在 call 之后才载基址
    //   （`mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,(%eax)`），char* 形 4.4 恒
    //   LHS-first 并把基址提前缓存进 %ebx。
    ItemMakingSkillReqView* sigBuf =
        reinterpret_cast<ItemMakingSkillReqView*>(
            guard.operator->()->GetInBuffer<SIG_ITEM_MAKING_SKILL>());
    memset(sigBuf, 0, 0x1c);
    // [M2S2 2026-09-11 · N76 判词作废] 旧判词「三形均不翻 ⇒ N76」已推翻：正解是
    //   **字段侧异型化**（P211 反形异型化）—— 视图 `ItemMakingSkillReqView::m_characNo`
    //   int → unsigned int（RHS `get_charac_no` 仍 int ⇒ 异型转换中转），就地翻回
    //   ORIG 0x08545335 的 RHS-first：`mov %eax,%edx; mov -0x10(%ebp),%eax;
    //   mov %edx,(%eax)`（我方旧 LHS-first `mov -0x10(%ebp),%edx; mov %eax,(%edx)`）。
    //   实测 86→87 条 = ORIG 87，**verdict DIFF→AE**（.o 口径 ae 48→49 / diff 6→5）。
    //   旧判词失败原因：当时未把字段改 unsigned（仅在 char* 局部形下试过「RHS cast」）。
    //   零 ABI 风险：视图结构仅本 TU 内使用，宽/偏移/pack 不变。
    sigBuf->m_characNo = pUser->get_charac_no(-1);
    for (int i = 0; i <= 0xa; ++i)
        sigBuf->m_skills[i] = m_skillLevels[i];
    // ORIG：MsgQueueMgr::put(DB_Q=2, guard) 提交 DB 队列（0x08545381）。
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

#include "StubS3A_deps/_reset_1.inc"
{
    memset(m_skillLevels, 0, 0x16);   // ORIG 0x085453ba：memset(this+6,0,0x16)
}

// ===========================================================================
// [SweepCAutoPunish 2026-09-10] FreeType 收尾 API 占位定义（P210② 跨 TU 迁定义，
// Main 授权；原定义在 GlobalData.cpp，迁出后 GlobalData.cpp 只见声明 ⇒ 4.4 对
// CaptchaGen::~CaptchaGen 0x080de706 的 FT_Done_Face/FT_Done_FreeType 调用判
// 可能抛，复现 ORIG 的 EH landing：push esi/ebx + `_ZNSt6vectorIPvSaIS0_EED2Ev`
// cleanup + `_Unwind_Resume`。探针 /tmp/gdprobe/p2.cpp 实证：被调**同 TU 有定义**
// ⇒ 4.4 判 nothrow 不布 landing；**仅声明** ⇒ 布 landing。nm 全链接集扫描确认
// FT_Done_Face/FT_Done_FreeType 此前仅由 GlobalData.o 定义（无库定义）。
// 函数体逐字节照搬自 GlobalData.cpp。
// ===========================================================================
extern "C" int FT_Done_Face(void* face)
{
    (void)face;
    return 0;
}

// FreeType 库内部收尾函数（ORIG 0x087c3c00 FT_Done_Library / 0x087dc6b0
// FT_Done_Memory）：定义仍在 GlobalData.cpp，此处仅取声明。
extern "C" int FT_Done_Library(void* library);
extern "C" int FT_Done_Memory(void* memory);

// ORIG 0x087bca50（idx49 报告）：library != 0 时读取其首字段 memory（offset 0），
// 依次调用 FT_Done_Library(library) 与 FT_Done_Memory(memory)；恒返 0。
extern "C" int FT_Done_FreeType(void* library)
{
    if (library != 0)
    {
        void* memory = *(void**)library;   // library->memory @ offset 0
        FT_Done_Library(library);
        FT_Done_Memory(memory);
    }
    return 0;
}
