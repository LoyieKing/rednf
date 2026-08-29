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
    item_lock::CItemLock* expand = reinterpret_cast<item_lock::CItemLock*>(
        GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2));
    int r = expand->IsAbleItemUnlock(lock, outSec);
    return r;
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
    item_lock::CItemLock* expand = reinterpret_cast<item_lock::CItemLock*>(
        GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2));
    int r = expand->IsAbleItemUnlockCancel(lock);
    return r;
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
    unsigned long seed = time(0);
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
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("CItemMakingSkill.cpp", 0x33);
    CStreamGuard guard(stream, true);
    guard << (int)0x26e;
    guard << (int)pUser->GetUID();

    SIG_ITEM_MAKING_SKILL* sig =
        guard.GetInBuffer<SIG_ITEM_MAKING_SKILL>();
    // ORIG：memset(sig,0,0x1c)；characNo@+0；skills[11]@+4（ushort ×11）。
    memset(sig, 0, 0x1c);
    char* sigBuf = reinterpret_cast<char*>(sig);
    *reinterpret_cast<int*>(sigBuf) = pUser->get_charac_no(-1);
    for (int i = 0; i <= 0xa; ++i)
        *reinterpret_cast<unsigned short*>(sigBuf + 4 + i * 2) =
            m_skillLevels[i];
    return true;
}

void CItemMakingSkill::_reset()
{
    memset(m_skillLevels, 0, 0x16);   // ORIG 0x085453ba：memset(this+6,0,0x16)
}
