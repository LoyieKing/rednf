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

#pragma pack(push, 1)
// SIG_ITEM_MAKING_SKILL 协议视图（§4.3）：+0=characNo、+4=skills[11] ushort
struct ItemMakingSkillReqView
{
    int m_characNo;                  // +0x00
    unsigned short m_skills[0xb];    // +0x04（11 项）
};
#pragma pack(pop)

// ---------------------------------------------------------------------------
// 1. CUser::IsAbleItemLock(int,int) const —— ORIG T 0x08646a9c
//    报告 docs/class_func_reports/CUser/IsAbleItemLock.md 全文转写。
//    0x15=21 无物品/无 item 定义；0x13=19 业务不可锁；0x11=17 attach 型不可锁。
int CUser::IsAbleItemLock(int space, int slot) const
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
    if (it->GetItemType() == 0x16 && item->m_field0 != 1)
        return 0x13;
    if (it->GetItemType() == 0xb && it->GetAttachType() == 2)
        return 0x11;
    if (space == 0xc)
        return 0x13;
    unsigned char lock = item->m_amp.GetLock();
    if (lock != 0)
        return 0x13;
    // ORIG 0x08646bcf：GetCharacExpandDataR(this,2) 返回数据区指针，
    // 直接作为 item_lock::CItemLock* this 传入 IsAbleItemLock（ORIG 同款怪癖）。
    item_lock::CItemLock* expand = reinterpret_cast<item_lock::CItemLock*>(
        GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2));
    int r = expand->IsAbleItemLock(lock);
    return r;
}

// ---------------------------------------------------------------------------
// 2. CUser::IsAbleItemUnlock(int,int,int&) const —— ORIG T 0x08646bfe
//    报告 docs/class_func_reports/CUser/IsAbleItemUnlock.md 汇编为准
//    （反编译 C 的 param_2/param_3 有笔误，栈槽 0xc=space / 0x10=slot / 0x14=outSec）。
int CUser::IsAbleItemUnlock(int space, int slot, int& outSec) const
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
    if (result != 0)
        return result;
    return 0;
}

// ---------------------------------------------------------------------------
// 3. CUser::IsAbleItemUnlockCancel(int,int) const —— ORIG T 0x08646cae
//    报告 docs/class_func_reports/CUser/IsAbleItemUnlockCancel.md 全文转写。
int CUser::IsAbleItemUnlockCancel(int space, int slot) const
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
    if (result != 0)
        return result;
    return 0;
}

// ---------------------------------------------------------------------------
// 4. _postCheckForceChangeGrowType4AheadQuest(CUser*,Quest*) —— ORIG T 0x086686ed
//    objdump 0x086686ed..0x08668823 逐指令转写（无独立报告；调用方
//    _postCheckForceChangeGrowType 见 CUser.cpp:8029 区）。
//    quest->m_intDataList(+0x2c, vector<vector<int>>) 每组逐 questIdx：
//    find_quest 后递归本函数，并 getCurCharacQuestW()+4 处 CQuestClear
//    setClearedQuest(questIdx)。
void _postCheckForceChangeGrowType4AheadQuest(CUser* user, Quest* quest)
{
    std::vector<std::vector<int> >& groups = quest->m_intDataList;   // +0x2c
    if (groups.empty())
        return;
    for (unsigned int gi = 0; gi < groups.size(); ++gi)
    {
        std::vector<int>& group = groups[gi];
        for (std::vector<int>::iterator it = group.begin(); it != group.end(); ++it)
        {
            int questIdx = *it;
            Quest* q = G_CDataManager()->find_quest(questIdx);
            if (q == 0)
                continue;
            _postCheckForceChangeGrowType4AheadQuest(user, q);
            WongWork::CQuestClear* cleared =
                reinterpret_cast<WongWork::CQuestClear*>(
                    reinterpret_cast<char*>(user->getCurCharacQuestW()) + 4);
            cleared->setClearedQuest((unsigned int)questIdx);
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
int CraneMinigameManager::startUseMaterial(CUser* user, int slot)
{
    const CInventory* inven = user->getCurCharacInvenR();   // ORIG 0x080da27e
    Inven_Item item = inven->GetInvenSlot(1, slot);
    if (item.m_addInfo !=
        *(int*)((char*)this + 0x10))                        // this+0x10（类改继承 Script 后无命名成员）
        return 0x11;
    const int needCnt =
        (int)(short)*(const unsigned short*)(const void*)((const char*)this + 0x14);
    if (item.m_addInfo2 < needCnt)                          // ORIG jge 0x080ebe5a
        return 0x16;
    CInventory* invenW = user->getCurCharacInvenW();        // ORIG 0x080da28e
    if (!invenW->delete_item((INVEN_TYPE)1, slot, needCnt,
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

void CRedeemItem::_reset()
{
    m_redeemList.clear();   // ORIG 0x085f7b7d: std::list::clear(this+8)
}

bool CRedeemItem::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    _reset();   // ORIG 0x085f7846：虚调 vtable slot7
    int count;
    memcpy(&count, pData + 4, 4);
    for (int i = 0; i < count; ++i)
    {
        const char* entry = pData + i * 0x44;
        int check;
        memcpy(&check, entry + 0xa, 4);   // ORIG mov 0xa(%eax,%edx,1)
        if (check == 0)
        {
            alter();   // ORIG 0x085f7875: charac_expand::CData::alter()
        }
        else
        {
            Inven_Item_Expand_RedeemInfo info;
            memcpy(&info, entry + 8, sizeof(info));
            m_redeemList.push_back(info);
        }
    }
    return true;
}

bool CRedeemItem::_saveData(CUser* pUser)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("Redeem_Item.cpp", 0x108);
    CStreamGuard guard(stream, true);
    guard << (int)0x1f4;
    guard << (int)pUser->GetUID();

    SIG_REDEEMINFO_LIST_TO_DB* sig =
        guard.GetInBuffer<SIG_REDEEMINFO_LIST_TO_DB>();
    if (sig == 0)
        return false;
    memset(sig, 0, 0x59c);
    // SIG_REDEEMINFO_LIST_TO_DB 布局：characNo@+0 / count@+4 / entries[21]@8。
    char* sigBuf = reinterpret_cast<char*>(sig);
    *reinterpret_cast<int*>(sigBuf) = pUser->get_charac_no(-1);
    *reinterpret_cast<int*>(sigBuf + 4) = (int)m_redeemList.size();
    int i = 0;
    for (std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it =
             m_redeemList.begin();
         it != m_redeemList.end(); ++it, ++i)
    {
        memcpy(sigBuf + 8 + i * 0x44, &*it, 0x44);
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

CItemMakingSkill::CItemMakingSkill()
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
    unsigned long seed = time(0);
    // ORIG 0x08545134：lea -0x1c,%esi —— 4.4 对实参 &seed 表达式自然产 lea（P0）。
    m_rand = new CMTRand(seed);
}

CItemMakingSkill::~CItemMakingSkill()
{
    if (m_rand != 0)
        delete m_rand;
    m_rand = 0;
}

bool CItemMakingSkill::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    _reset();   // ORIG 0x08545218：虚调 vtable slot7
    for (int i = 0; i <= 0xa; ++i)
    {
        unsigned short v;
        memcpy(&v, pData + 4 + i * 2, 2);
        if (v > 0x3e7)
            m_skillLevels[i] = 0x3e8;
        else
            m_skillLevels[i] = v;
    }
    return true;
}

bool CItemMakingSkill::_saveData(CUser* pUser)
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
    char* sigBuf = (char*)guard.operator->()->GetInBuffer<SIG_ITEM_MAKING_SKILL>();
    memset(sigBuf, 0, 0x1c);
    reinterpret_cast<ItemMakingSkillReqView*>(sigBuf)->m_characNo =
        pUser->get_charac_no(-1);
    for (int i = 0; i <= 0xa; ++i)
        reinterpret_cast<ItemMakingSkillReqView*>(sigBuf)->m_skills[i] =
            m_skillLevels[i];
    // ORIG：MsgQueueMgr::put(DB_Q=2, guard) 提交 DB 队列（0x08545381）。
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

void CItemMakingSkill::_reset()
{
    memset(m_skillLevels, 0, 0x16);   // ORIG 0x085453ba：memset(this+6,0,0x16)
}
