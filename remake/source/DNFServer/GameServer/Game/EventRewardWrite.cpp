// EventRewardWrite.cpp —— eventReward 奖励发放链（OrigStStructs 独占）。
//
// 覆盖 ORIG 真实符号（全部 T，报告 docs/class_func_reports/eventReward/*.md 全文转写）：
//   createCharacInvenReward 0x080f0270 / createCharacEquipReward 0x080f033c
//   rewardWriteUserInven    0x080f0408 / rewardWriteUserEquip    0x080f06da
//   sendPostalReward        0x080f0976 / unsealRandomOption      0x080f0e40
//   applyItemOption         0x080f0d6e / StartEndPos             0x080f0d22（报告缺失，
//   objdump 直转）
//
// 注：ORIG 中 createCharac* / sendPostalReward 的调用侧以
//   G_CDataManager()->m_eventReward（CDataManager+0xb628）作为接收者，本 TU 照此书写；
//   this 形参在这些函数中不参与发放（见各报告 Ghidra C）。

#include <string.h>
// ============================================================================
// b28d-52 判词表（本 TU）：
//   已修：rewardKey::operator< 0x080f121a —— P222 正向包裹 + P196 三字节字段
//     signed char（CDataManager.h，已 R2 广播）⇒ 转 AE。
//   剩余 DIFF：
//   eventReward::unsealRandomOption 0x080f0e40 —— N82（帧 0x4c vs 0x3c）
//   eventReward::applyItemOption 0x080f0d6e —— N82（帧 0x14 vs 0x24）
//   eventReward::sendPostalReward 0x080f0976 —— N82（帧 0x1bc vs 0x1b0）
//   eventReward::rewardWriteUserEquip 0x080f06da —— N82（帧差 + 迭代器槽）
//   eventReward::rewardWriteUserInven 0x080f0408 —— N82（帧 0xa4 vs 0x94）
// ============================================================================

#include <map>
#include <string>
#include <vector>

#include "CDataManager.h"       // eventReward / rewardKey / itemData / G_CDataManager / find_item
#include "Inven_Item.h"         // Inven_Item
#include "CItem.h"              // CItem 虚/非虚访问器
#include "CInventory.h"         // g_slotBoundChecker / OS_API::snprintf
#include "CItemGenerator.h"     // random_option::CRandomOptionItemHandle::give_option / ENUM_RARITY
#include "GlobalData.h"         // g_randomOptionHandle
#include "LogManager.h"         // LogManager::logFormat
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "GameWorld.h"          // G_GameWorld / IsIntegratedPvPBaseChannel
#include "CEnvironment.h"       // G_CEnvironment
#include "CUser.h"              // CUser（继承 CUserCharacInfo：get_charac_level/getCurCharacNo）

namespace
{

// eventReward+0x18 的 CSlotBoundChecker 视图（P1 组件视图折叠为单 disp 寻址）。
struct EventRewardCheckerPos
{
    char m_pad[0x18];
    CSlotBoundChecker m_checker;
};

}  // unnamed namespace

// ORIG 以 this+0x18 内嵌 CSlotBoundChecker 调用 get_slot_bound；重建 CDataManager.h
// 以同布局 map 成员 m_slotBoundMap 保持 ABI（CSlotBoundChecker 静态实现无成员），
void eventReward::StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end,
                              int cap) const
{
    if (!reinterpret_cast<EventRewardCheckerPos const*>(this)->m_checker.get_slot_bound(
            type, cap, start, end))
    {
        start = 0;
        end = 0;
    }
}

// ============================================================================
// eventReward::applyItemOption —— ORIG T 0x080f0d6e
// ============================================================================
void eventReward::applyItemOption(Inven_Item& inven, const CItem* item,
                                  const itemData& data)
{
    if (item->is_stackable())
    {
        inven.m_addInfo2 = data.m_count;            // item+0x07 = data+0x08
        return;
    }
    reinterpret_cast<unsigned char&>(inven.m_field0) = data.m_seal;
    register unsigned int attachType = item->GetAttachType();  // ORIG 单次调用（寄存器化）
    if (attachType < 6 && ((1u << attachType) & 0x27))
    {
        inven.m_field0 = 0;
    }
    if (data.m_upgrade > 1)                         // +0x00 字节
    {
        inven.SetUpgrade(data.m_upgrade);
    }
    // ORIG 0x80f0d6e：虚调用 vptr+0x4c = vtable+0x54 = vt[21] IsRandomOption
    // （vptr 指向 vtable+8；勿与 vt[19] IsPossibleKiriProtection 混淆）
    // ORIG +0x91..+0xaf：短路物化岛（mov $1/jmp/mov $0 汇聚 + test）⇒ P226 语句表达式形。
    if (({ item->IsRandomOption() && data.m_roi == 0; }))  // +0x02 字节
    {
        this->unsealRandomOption(item, inven);
    }
}

// ============================================================================
// eventReward::unsealRandomOption —— ORIG T 0x080f0e40
// ============================================================================
int eventReward::unsealRandomOption(const CItem* item, Inven_Item& inven)
{
    if (item == 0)
        return 0;
    // ORIG 0x80f0e40：虚调用 vptr+0x4c = vtable+0x54 = vt[21] IsRandomOption
    if (!item->IsRandomOption())
        return 0;
    // ORIG 反汇编：第二次虚调用 vptr+0x50 = vtable 槽 20 = GetRandomOptionGrade
    //（CItem vtable @0x8c96da8：s19=IsRandomOption, s20=GetRandomOptionGrade）。
    // ORIG +0x3c..+0x42：&inven.m_random(+0x25) 在首个 getter 之前物化到 -0x1c 槽 ⇒ 具名指针局部。
    RandomOption* pOption = &inven.m_random.m_random;
    register int period = item->GetRandomOptionGrade();
    register int group = item->getItemGroupName();
    register int lvl = item->getUsableLevel();
    register int rarity = item->get_rarity();
    if (!GlobalData::g_randomOptionHandle->give_option(
            (unsigned long)inven.m_addInfo,
            (ENUM_RARITY)rarity, lvl, group, period, pOption))
    {
        LogManager::logFormat(
            1, "eventReward.cpp",
            __PRETTY_FUNCTION__, 0x240,
            "autoUnsealItem : %d", inven.m_addInfo);
    }
    return 1;
}

// ============================================================================
// eventReward::rewardWriteUserInven —— ORIG T 0x080f0408
// ============================================================================
bool eventReward::rewardWriteUserInven(Inven_Item* item, rewardKey key, int count)
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] p167 EXTRA `_ZNKSt17_Rb_tree_iterator…ptEv` +1 ⇒ 我方比 ORIG 多一次
    // `it->second`（4.4 每处 `it->` 各发一次 const ptEv）⇒ ORIG 为单次解引用缓存。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    // [L10-2] P28 增量侧：ORIG 调 `ppEi`（后置）⇒ `vit++`。
    for (std::vector<itemData>::iterator vit = vec.begin(); vit != vec.end(); vit++)
    {
                // ORIG +0x11a..+0x134：*vit 取 m_itemIdx（ebx 跨 G_CDataManager 调用存活）
        // 直接嵌套实参；citem 判空后 +0x157 再 *vit 一次写 inven.m_addInfo。
        CItem* citem = G_CDataManager()->find_item(vit->m_itemIdx);
        if (citem == 0)
            return 0;
                Inven_Item inven;
        inven.m_addInfo = vit->m_itemIdx;       // ORIG +0x162/+0x165（make_item 前）
        citem->make_item(inven);                    // vtable+0x8
        applyItemOption(inven, citem, *vit);
        int start = 0;
        int end = 0;
        StartEndPos(static_cast<Inven_Item::ITEM_TYPE>(inven.m_field1),
                    start, end, 0);
        for (int i = start; i <= end; ++i)
        {
            if (item[i].m_addInfo == 0)             // +0x02 槽判空
            {
                item[i] = inven;                    // 整体 0x3d 字节写入
                break;
            }
        }
    }
    return 1;
}

// ============================================================================
// eventReward::rewardWriteUserEquip —— ORIG T 0x080f06da
// ============================================================================
bool eventReward::rewardWriteUserEquip(Inven_Item* item, rewardKey key, int count)
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] 同 rewardWriteUserInven：ORIG 单次解引用（消 p167 EXTRA const ptEv）。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    for (std::vector<itemData>::iterator vit = vec.begin(); vit != vec.end(); vit++)
    {
                // ORIG +0x11a..+0x134：*vit 取 m_itemIdx（ebx 跨 G_CDataManager 调用存活）
        // 直接嵌套实参；citem 判空后 +0x157 再 *vit 一次写 inven.m_addInfo。
        CItem* citem = G_CDataManager()->find_item(vit->m_itemIdx);
        if (citem == 0)
            return 0;
                Inven_Item inven;
        inven.m_addInfo = vit->m_itemIdx;       // ORIG +0x162/+0x165（make_item 前）
        citem->make_item(inven);                    // vtable+0x8
        applyItemOption(inven, citem, *vit);
        if (citem->is_stackable())
            return 0;
        const int slot = citem->GetItemType();      // vtable+0xc
        item[slot - 10] = inven;                    // slot*0x3d - 0x262
    }
    return 1;
}

// ============================================================================
// eventReward::sendPostalReward —— ORIG T 0x080f0976
// ============================================================================
int eventReward::sendPostalReward(CUser& user, rewardKey key, int count,
                                  const char* title, const char* body)
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] 同 rewardWriteUserInven：ORIG 单次解引用（消 p167 EXTRA const ptEv）。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    // [L10-2] P28 增量侧：ORIG 调 `normal_iterator::ppEi`（后置）⇒ 源形 `vit++`；
    // `++vit` 走 ppEv 会缺 ppEi 并多发 ppEv（p167 MISSING/EXTRA 实证）。
    for (std::vector<itemData>::iterator vit = vec.begin(); vit != vec.end(); vit++)
    {
        const int itemIdx = vit->m_itemIdx;
        Inven_Item inven;
        CItem* citem = G_CDataManager()->find_item(itemIdx);
        int serverGroup = 0;
                // ORIG +0x159..+0x185：IsIntegratedPvPBaseChannel 先测，test %al 无取反；
        // 真分支（集成 PvP）取 user.GetServerGroup，假分支取 G_CEnvironment+0x378。
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
                        serverGroup = static_cast<int>(user.GetServerGroup());
        else
            serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        if (citem != 0)
        {
            inven.m_addInfo = itemIdx;              // item+0x02
            citem->make_item(inven);                // vtable+0x8
            applyItemOption(inven, citem, *vit);
            char buf[256] = { '\0' };
            const int lvl = user.get_charac_level();
            OS_API::snprintf(buf, 0xff,
                             g_scriptStringManager_.findString(4, body, 0), lvl);
            // ORIG +0x2ab：读的是 inven.m_addInfo（-0x87 = inven+0x02），非 itemIdx 形参副本；
            // 且短路带 bool 物化岛（mov $1/jmp/mov $0 + test）⇒ P226 语句表达式。
            if (({ inven.m_addInfo != 0 && citem->is_stackable(); }))
            {
                const int expDate = citem->getExpirationDate();
                const size_t len = strlen(buf);
                const unsigned int characNo = user.getCurCharacNo();
                const char* titleStr = g_scriptStringManager_.findString(4, title, 0);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    titleStr, inven, 0, characNo, buf, static_cast<int>(len),
                    static_cast<unsigned int>(expDate),
                    static_cast<ENUM_SERVER_GROUP>(serverGroup), 0, 1);
            }
        }
    }
    return 1;
}

// ============================================================================
// eventReward::createCharacInvenReward —— ORIG T 0x080f0270
// ============================================================================
bool eventReward::createCharacInvenReward(Inven_Item* item, rewardKey key, int count)
{
    bool ok = false;
    if (G_CDataManager()->m_eventReward.rewardWriteUserInven(item, key, count))
        ok = true;
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 1;                         // +0x14（inven）
    if (G_CDataManager()->m_eventReward.rewardWriteUserInven(item, key, count))
        ok = true;
    key.m_job = 0xb;                                // +0x10
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 1;                         // +0x14
    if (G_CDataManager()->m_eventReward.rewardWriteUserInven(item, key, count))
        ok = true;
    return ok;
}

// ============================================================================
// eventReward::createCharacEquipReward —— ORIG T 0x080f033c
// ============================================================================
bool eventReward::createCharacEquipReward(Inven_Item* item, rewardKey key, int count)
{
    bool ok = false;
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 2;                         // +0x14（equip）
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    key.m_job = 0xb;                                // +0x10
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 2;                         // +0x14
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    return ok;
}
