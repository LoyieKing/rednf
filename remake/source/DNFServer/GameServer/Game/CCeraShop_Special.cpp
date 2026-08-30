// df_game_r WongWork::CCeraShop 特殊商品三方法 + 依赖（U2 批次）。
// 依据 docs/class_func_reports/WongWork__CCeraShop/{IsSpecialItem,ProcessSpecialItem,
// SaveFeaturedIdx}.md 与 DB_SaveFeaturedGoodsIdx/makeRequest.md、
// STSpecailItem_Param/Result ctor（ORIG W 0x08234ec2/0x08234f36）逐函数转写。

#include "CGameManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CInventory.h"
#include "CStackableItem.h"
#include "CItemList.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "Stream.h"
#include "Inven_Item.h"
#include "CCeraShop.h"
#include "SpecialItemHandler.h"
#include "CHandlePremium.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CPremiumLetheManager.h"
#include "DB_UpdateInventoryCapacity.h"
#include "CEventMgr.h"          // CEventMgr::GetPremiumServiceBonus
#include "SPremiumInfo.h"       // SPremiumInfo 权威布局（g_SPremiumInfo）
#include "GameTypes.h"      // cMyTrace
#include "LogManager.h"     // LogManager::logFormat
#include "CEquipItem.h"     // getAvatarPeriod
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "GameWorld.h"      // G_GameWorld / send_all
#include "STStackableScript.h"  // STStackablePackageData（包数据 ORIG 实证元素类型）
#include "SkillSlot.h"          // SkillSlot 权威布局（getCurCharacSkillR 返回其对象）
// 注：WongWork::CGenUniqueNo 权威声明在 CInventory.h（已含）；勿重复 include CGenUniqueNo.h。
#include "premium_helper_function.h"
#include "DB_UpdateChuseokPackagePurchase.h"  // makeRequest（Chuseok 落库请求）
#include "CItemGenerator.h"  // random_option::CRandomOptionItemHandle::give_option

#include <cstring>
#include <map>
#include <vector>

#include "W8LT_DBTask.h"        // DB_SaveFeaturedGoodsIdx::makeRequest 权威声明



// STSpecailItem_Param/Result 完整定义已提升至 CCeraShop.h（ORIG W ctor
// 0x08234ec2/0x08234f36；布局 +0x00 user +0x04 itemIdx +0x08 value +0x0c items*
// +0x10 ceraParam* +0x14 count +0x18 time(long) +0x1c flag +0x1d/1e/1f char
// +0x20 extra(ctor 置 1)；Result 仅 int m_value）。本 TU 经 CCeraShop.h 获得。

// ============================================================================
// SpecialItemHandler —— ORIG 全局类（mangled _ZN18SpecialItemHandler…，非 WongWork 前缀）。
// 权威声明见 SpecialItemHandler.h；本 TU 为唯一实现点。
// ctor 0x0827bcec / execute 0x0827be0a / respond_* 家族地址见
// docs/class_func_reports/SpecialItemHandler/README.md。
// ============================================================================

// ---- SpecialItemHandler::SpecialItemHandler —— ORIG 0x0827bcec ----
// 对象布局：+0x00 CCeraShop* m_owner；+0x04.. 14 组 {respond fn, offset} 派发表。
// ctor 先清零 14 组，再按 type 填 dispatch 表（respond fn 地址，offset 全 0）。
SpecialItemHandler::SpecialItemHandler(WongWork::CCeraShop* owner)
{
    m_owner = owner;
    for (int i = 0; i < 0xe; ++i)
    {
        m_entries[i].fn = 0;
        m_entries[i].offset = 0;
    }
    m_entries[1].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_cargo);
    m_entries[2].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_premium);
    m_entries[5].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_package);
    m_entries[6].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_booster);
    m_entries[7].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_cargo_abs);
    m_entries[8].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_charac_slot_extend);
    m_entries[9].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_skill_confirm);
    m_entries[10].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_skill_tree_style);
    m_entries[11].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_inventory_upgrade);
    m_entries[12].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_booster_selection);
    m_entries[13].fn = reinterpret_cast<RespondFn>(&SpecialItemHandler::respond_special_item_cerapoint_giftcard);
}

// ---- SpecialItemHandler::execute —— ORIG 0x0827be0a ----
// 表驱动分发：type<0xe 时取 m_entries[type] 的 respond fn，以 (this+offset) 为接收者调用。
// ORIG 的 flag 低 1 位为“间接表”标记；本对象所有条目均为直接 respond 地址（低位置 0），
// 故等价于直接调用。type 越界返回 0。
bool SpecialItemHandler::execute(int type, const STSpecailItem_Param& param,
                                 STSpecailItem_Result& result)
{
    if (type > 0xd)
        return false;
    Entry& e = m_entries[type];
    SpecialItemHandler* self =
        reinterpret_cast<SpecialItemHandler*>(reinterpret_cast<char*>(this) + e.offset);
    e.fn(self, type, param, result);
    return true;
}

// ============================================================================
// respond_special_item_* 家族
// ============================================================================

// ---- respond_special_item_cargo —— ORIG 0x0827be8a ----
// 仓库扩容：getCurCharacCargoW 上调 UpgradeCargo(value)；成功发 cargo 升级包，
// 失败 result.m_value = 0x17。
void SpecialItemHandler::respond_special_item_cargo(int type,
                                                    const STSpecailItem_Param& param,
                                                    STSpecailItem_Result& result)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int value = param.m_value;
    CCargo* cargo = user->getCurCharacCargoW();
    if (cargo->UpgradeCargo(value) == 1)
        m_owner->_sendCargoUpgradePacket(user, type);
    else
        result.m_value = 0x17;
}

// ---- respond_special_item_cargo_abs —— ORIG 0x0827bee6 ----
// 绝对容量仓库升级：目标容量 = 当前 GetCapacity()+value；isUpgradable 通过则
// UpgradeCargo 后发包，否则 result.m_value = 0x12。
void SpecialItemHandler::respond_special_item_cargo_abs(int type,
                                                        const STSpecailItem_Param& param,
                                                        STSpecailItem_Result& result)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int value = param.m_value;
    const CCargo* cargoR = user->getCurCharacCargoR();
    int target = cargoR->GetCapacity() + value;
    if (cargoR->isUpgradable(target) == 1)
    {
        user->getCurCharacCargoW()->UpgradeCargo(target);
        m_owner->_sendCargoUpgradePacket(user, type);
    }
    else
    {
        result.m_value = 0x12;
    }
}

// ORIG 仅读取 param 的 m_user/m_value 到局部后返回（O0 保留死读取）。
void SpecialItemHandler::respond_special_item_gift_ticket(int,
                                                          const STSpecailItem_Param& param,
                                                          STSpecailItem_Result&)
{
    void* user = param.m_user;
    int value = param.m_value;
}

// ---- respond_special_item_skill_confirm —— ORIG 0x0827cdd0 ----
// 日志后调用 CPremiumLetheManager::ConfirmSkillReq(user)。
void SpecialItemHandler::respond_special_item_skill_confirm(int,
                                                            const STSpecailItem_Param& param,
                                                            STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    CPremiumLetheManager* mgr = G_CGameManager()->GetPremiumLetheManager();
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x1e1, 0);
    trace("ONE_DAY_LETHE : BUY_SKILL_CONFIRM_TICKET, char(%s), char_no(%d), style(%d)",
          user->getCurCharacName(), user->getCurCharacNo(),
          user->GetCurCharacSkillTreeIndex());
    if (mgr != 0)
        mgr->ConfirmSkillReq(user);
}

// ---- respond_special_item_skill_tree_style —— ORIG 0x0827ce7e ----
// 置技能树 index 0 → UpdateData → InitSkill(user, true, kind=1)；组包
// (header 0/2, byte 0, short 1, make_basic_info, finalize) 后 Send。
void SpecialItemHandler::respond_special_item_skill_tree_style(int,
                                                               const STSpecailItem_Param& param,
                                                               STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    CPremiumLetheManager* mgr = G_CGameManager()->GetPremiumLetheManager();
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        499, 0);
    trace("SKILL_TREE_STYLE : BUY_SKILL_TREE_STYLE, char(%s), char_no(%d)",
          user->getCurCharacName(), user->getCurCharacNo());
    user->SetCurCharacSkillTreeIndex(0);
    user->UpdateData();
    mgr->InitSkill(user, true, (ENUM_SKILL_TREE_KIND)1);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 0);
    packet.finalize(true);
    user->Send(packet);
}

// ---- respond_special_item_booster_selection —— ORIG 0x0827ccfc ----
// 转发 CCeraShop::_processBoosterSelect（参数映射见报告汇编）。
void SpecialItemHandler::respond_special_item_booster_selection(
    int, const STSpecailItem_Param& param, STSpecailItem_Result& result)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    m_owner->_processBoosterSelect(
        user, param.m_itemIdx, param.m_count, result.m_value, param.m_time,
        param.m_flag != 0, *static_cast<std::vector<Inven_Item>*>(param.m_items),
        *static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam),
        static_cast<char>(param.m_char1d), static_cast<char>(param.m_char1e));
}

// ---- respond_special_item_cerapoint_giftcard —— ORIG 0x0827de4e ----
// ORIG：经 GlobalData::s_pIPGHelper 虚表 +0xc 调用
//   (helper, user, value, 2, 0, 0)。
void SpecialItemHandler::respond_special_item_cerapoint_giftcard(
    int, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    WongWork::IPG::CIPGHelper* helper = GlobalData::s_pIPGHelper;
    CUser* user = static_cast<CUser*>(param.m_user);
    int value = param.m_value;
    // CIPGHelper 虚接口（vtable+0xc 槽）尚未建模成头，沿用裸槽派发；指针类型
    // 已按 GlobalData.h 权威声明落地。
    typedef void (*Fn)(void*, void*, int, int, int, int);
    Fn fn = *reinterpret_cast<Fn*>(*reinterpret_cast<char**>(helper) + 0xc);
    fn(helper, user, value, 2, 0, 0);
}

// ---- respond_special_item_charac_slot_extend —— ORIG 0x0827c0c2 ----
// 增角色槽限 → 组 SIG_EXTEND_CHARAC_SLOT 包（acc_id @0 / slotEffect @4 /
// slotLimit @5）入队列 2 → _sendCharacSlotExtendPacket。
void SpecialItemHandler::respond_special_item_charac_slot_extend(
    int type, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    user->incCharacSlotLimit(static_cast<unsigned char>(param.m_value));

    Stream* stream = GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0x8d);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<((int)0x28a);
    guard.operator*()->operator<<(-1);
    char* buf = reinterpret_cast<char*>(guard->GetInBuffer<SIG_EXTEND_CHARAC_SLOT>());
    *reinterpret_cast<int*>(buf + 0) = static_cast<int>(user->get_acc_id());
    buf[4] = static_cast<char>(user->getSlotEffectCount());
    buf[5] = static_cast<char>(user->getCharacSlotLimit());
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    m_owner->_sendCharacSlotExtendPacket(user, type);
}

// ---- respond_special_item_inventory_upgrade —— ORIG 0x0827bf90 ----
// 背包容量 < 0x10 时：ExpandInventory 日志 → 容量 += value → DB_UpdateInventoryCapacity
// 持久化 → _sendInventoryUpgradePacket；否则 result.m_value = 0x12。
void SpecialItemHandler::respond_special_item_inventory_upgrade(
    int type, const STSpecailItem_Param& param, STSpecailItem_Result& result)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    // ORIG 调 getCurCharacInvenR（const 版，无 enableSaveInven 副作用；W 版会置脏标记）
    CInventory* inv = const_cast<CInventory*>(user->getCurCharacInvenR());
    int cap = inv->get_inventory_capacity();
    if (cap < 0x10)
    {
        int newCap = cap + param.m_value;
        user->m_historyLog.ExpandInventory(cap, newCap, 0);
        inv->set_inventory_capacity(newCap);
        DB_UpdateInventoryCapacity::makeRequest(user->GetServerGroup(), user->GetUID(),
                                                (unsigned int)user->getCurCharacNo(), newCap);
        m_owner->_sendInventoryUpgradePacket(user, type);
    }
    else
    {
        result.m_value = 0x12;
    }
}

// ---- respond_special_item_booster —— ORIG 0x0827ca4a ----
// setOriginalItem → _onBuySpecialItem(user, 6, itemIdx) → 遍历 find_item(itemIdx)
// 的 booster 子物品表 getBoosterItem，逐项 _processAdditionalItem →
// GetRestrictCode/IsRestrictedGoods/SetRestrictedGoods/SaveFeaturedIdx → SaveInventory。
// 子物品项 vtable+0x60 槽经 CItem 虚表序核对 = set_parent_booster_item_id(ulong)，
// 已按真实虚方法直调（CItem.h 声明，槽位 +0x60 吻合）。
void SpecialItemHandler::respond_special_item_booster(int,
                                                      const STSpecailItem_Param& param,
                                                      STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    std::vector<Inven_Item>* items = static_cast<std::vector<Inven_Item>*>(param.m_items);
    WongWork::CCeraShop::stCeraShopItemParam_t* cera =
        static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam);

    m_owner->setOriginalItem(itemIdx);
    m_owner->_onBuySpecialItem(user, ENUM_CERA_SPECIALITEM_BOOSTER,
                               static_cast<unsigned long>(itemIdx));

    CStackableItem* base = static_cast<CStackableItem*>(G_CDataManager()->find_item(itemIdx));
    std::vector<stStackableBoosterItemInfo_t> boosterList;
    if (base != 0)
        base->getBoosterItem(boosterList);

    for (std::vector<stStackableBoosterItemInfo_t>::iterator it = boosterList.begin();
         it != boosterList.end(); ++it)
    {
        CItem* subItem = G_CDataManager()->find_item((int)it->m_itemIdx);
        // ORIG vtable+0x60 = CStackableItem::set_parent_booster_item_id(unsigned long)
        if (subItem != 0)
            subItem->set_parent_booster_item_id((unsigned long)itemIdx);
        // _processAdditionalItem 实参：itemIdx, addInfo(+0x8), avatarLevel(+0xe), avatarType(+0xd)
        m_owner->_processAdditionalItem(user, (unsigned long)it->m_itemIdx, it->m_addInfo,
                                        it->m_avatarLevel, it->m_avatarType, *items, *cera, 0);
        if (subItem != 0)
            subItem->set_parent_booster_item_id(0);
    }

    int restrictCode = G_CDataManager()->getItemList()->GetRestrictCode(itemIdx);
    if (restrictCode != 0)
    {
        if (!user->IsRestrictedGoods(restrictCode))
        {
            user->SetRestrictedGoods(restrictCode);
            m_owner->SaveFeaturedIdx(user, restrictCode, (int)param.m_time, 0);
        }
    }
    user->SaveInventory();
}

// ---- respond_special_item_package —— ORIG 0x0827c204 ----
// 完整还原。此前 BLOCKED 判定的"匿名 static helper 0x827e06c..0x827e320"实为 STL 模板实例
// （std::vector<tagPackageItemSelection> / std::vector<STStackablePackageData> / iterator 操作），
// 由编译器依本函数 STL 用法自动生成，无需手写。依赖的 CCeraShop 方法
// (_onBuySpecialItem/_processAdditionalItem) 声明于 CCeraShop.h，实现由 CCeraShop 还原批次提供。
// 包字段/随机箱统计调用链：CStackableItem::m_pkg21c(+0x21c) 包列表、m_pkg234(+0x234)
// 逐选择包表，ceraParam 的 vector<tagPackageItemSelection>(+0xc)/vector<pair<int,int>>(+0x0)，
// 逐包 _processAdditionalItem（子物品 vtable+0x60 槽按 CItem::set_parent_booster_item_id
// 真实虚方法直调）+ CUser::SendRandomBoxStatistic(0x380/0x28982d/0x289749)。
void SpecialItemHandler::respond_special_item_package(
    int, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    std::vector<Inven_Item>* items = static_cast<std::vector<Inven_Item>*>(param.m_items);
    WongWork::CCeraShop::stCeraShopItemParam_t* cera =
        static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam);
    long time = param.m_time;

    if (itemIdx == 0x1b)
    {
        // 疯狂升级事件包：header 0xb4 + body "0612\0"+accId+characNo 入队列 4。
        Stream* stream = GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0xc3);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<((int)0xb4);
        guard.operator*()->operator<<(-1);
        char* buf = reinterpret_cast<char*>(guard->GetInBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT>());
        memcpy(buf, "0612\0", 5);                       // DAT_08c07e5a
        *reinterpret_cast<int*>(buf + 5) = static_cast<int>(user->get_acc_id());
        *reinterpret_cast<int*>(buf + 9) = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(4), guard);
    }

    m_owner->_onBuySpecialItem(user, (ENUM_CERA_SPECIALITEM)5,
                               static_cast<unsigned long>(itemIdx));

    // ORIG 实证元素类型（respond_special_item_package 反编译）：
    //   +0x21c = std::vector<STStackablePackageData>（包基础物表）
    //   +0x234 = std::vector<std::vector<STStackablePackageData>>（每选择包组表）
    // 头文件 m_pkg21c/m_pkg234 当前为 vector<char> 占位（元素类型修正涉共享头
    // CStackableItem.h 的多 TU 验证，另行批次）；此处以具名字段锚定偏移、按
    // ORIG 类型取视图。
    CStackableItem* stackItem =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(itemIdx));
    std::vector<STStackablePackageData> pkgList(
        *reinterpret_cast<std::vector<STStackablePackageData>*>(&stackItem->m_pkg21c));
    std::vector<std::vector<STStackablePackageData> >* pkgTable =
        reinterpret_cast<std::vector<std::vector<STStackablePackageData> >*>(
            &stackItem->m_pkg234);
    std::vector<tagPackageItemSelection>& selections = cera->m_sel;   // +0xc
    std::vector<std::pair<int, int> >& pairs = cera->m_pairs;         // +0x0
    if (!selections.empty())
    {
        if (selections.size() > 1)
        {
            // 多选择：逐项记录 error 日志（kdd372418Log 前缀，ORIG 原样）。
            for (std::vector<tagPackageItemSelection>::iterator it = selections.begin();
                 it != selections.end(); ++it)
            {
                char* accStr = NumberToString(user->get_acc_id(), 0);
                LogManager::logFormat(
                    1, "SpecialItemHandler.cpp",
                    __PRETTY_FUNCTION__,
                    0xdb, "kdd372418Log::Package Selection item error (%s,%d,%d,%d,%d)",
                    accStr, user->get_charac_no(-1), it->m_itemIdx,
                    (int)it->m_outer, (int)it->m_inner);
            }
        }
        // 主选择循环：找 itemIdx 匹配项，校验 pkgTable 索引，push 到 pkgList 并 erase。
        for (std::vector<tagPackageItemSelection>::iterator it = selections.begin();
             it != selections.end(); )
        {
            if (it->m_itemIdx == itemIdx)
            {
                bool invalid = true;
                if (it->m_outer < pkgTable->size())
                {
                    std::vector<STStackablePackageData>& inner = (*pkgTable)[it->m_outer];
                    if (inner.size() > it->m_inner)
                    {
                        pkgList.push_back(inner[it->m_inner]);
                        it = selections.erase(it);
                        invalid = false;
                    }
                }
                if (invalid)
                {
                    char* accStr = NumberToString(user->get_acc_id(), 0);
                    LogManager::logFormat(
                        1, "SpecialItemHandler.cpp",
                        __PRETTY_FUNCTION__,
                        0xe7, "Package Selection error (%s,%d,%d,%d,%d)",
                        accStr, user->get_charac_no(-1), it->m_itemIdx,
                        (int)it->m_outer, (int)it->m_inner);
                    break;
                }
            }
            else
            {
                ++it;
            }
        }
    }

    // 逐包处理：配对 itemIdx→count 消费、set_parent_booster_item_id（vtable+0x60 槽）、
    // _processAdditionalItem、SendRandomBoxStatistic。
    for (std::vector<STStackablePackageData>::iterator it = pkgList.begin();
         it != pkgList.end(); ++it)
    {
        unsigned char local25 = 0;
        for (std::vector<std::pair<int, int> >::iterator pit = pairs.begin();
             pit != pairs.end(); )
        {
            if (pit->first == it->m_field0)
            {
                local25 = static_cast<unsigned char>(pit->second);
                pairs.erase(pit);
                break;
            }
            else
            {
                ++pit;
            }
        }

        CItem* sub = G_CDataManager()->find_item(it->m_field0);
        // ORIG (**(code**)(*sub + 0x60))(sub, itemIdx)：CItem vtable 序核对槽 +0x60
        // = set_parent_booster_item_id(unsigned long)，按真实虚方法直调。
        if (sub != 0)
            sub->set_parent_booster_item_id(static_cast<unsigned long>(itemIdx));

        m_owner->_processAdditionalItem(
            user, static_cast<unsigned long>(it->m_field0),
            static_cast<unsigned short>(it->m_field4), local25, 0xff, *items, *cera, 0);

        if (sub != 0)
            sub->set_parent_booster_item_id(0);

        // 随机箱统计调用链（ORIG 0x0827c8fc..0x0827c976）
        if (it->m_field0 == 0x289749)
            user->SendRandomBoxStatistic(2, (ENUM_RANDOMBOX_ACTION_KIND)0);
        else if (it->m_field0 == 0x28982d)
            user->SendRandomBoxStatistic(1, (ENUM_RANDOMBOX_ACTION_KIND)0);
        else if (it->m_field0 == 0x380)
            user->SendRandomBoxStatistic(0, (ENUM_RANDOMBOX_ACTION_KIND)0);
    }

    // 收尾：GetRestrictCode → SetRestrictedGoods/SaveFeaturedIdx → SaveInventory。
    int restrictCode = G_CDataManager()->getItemList()->GetRestrictCode(itemIdx);
    if (restrictCode != 0)
    {
        if (!user->IsRestrictedGoods(restrictCode))
        {
            user->SetRestrictedGoods(restrictCode);
            m_owner->SaveFeaturedIdx(user, restrictCode, static_cast<int>(time), 0);
        }
    }
    user->SaveInventory();
}

// ---- respond_special_item_premium —— ORIG 0x0827d3dc ----
// g_map_premium_item_data[itemIdx] 命中后按 premium type (STPremiumItemData.m_value0)
// jump-table 分发：term/flag/item 取自 m_value4(低2字节)/m_value8。各 case 处理
// 限制码/日志/技能备份/表情/疲劳/金币等；收尾统一走
// cal_end_time → send_db → handleSetUserPremium → handlePremiumEffect →
// is_notify/_sendPremiumPacket → write_log。未命中记 PREMIUM_ERROR 日志返回。
// 注：报告汇编中 "ItemVendingMachine::BuyAuctionItem" 标签为 report_resolve 误标，
// 实际地址 0x0817a10c 为 CItemList::GetRestrictCode（nm 核实），本函数不使用 BuyAuctionItem。
void SpecialItemHandler::respond_special_item_premium(int type,
                                                      const STSpecailItem_Param& param,
                                                      STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    int count = param.m_count;
    int flag = param.m_flag;                    // local_39（byte@+0x1c）
    long timeVal = param.m_time;                // local_a4
    long timeA = timeVal;                       // local_34
    long timeB = timeVal;                       // local_38

    std::map<unsigned long, STPremiumItemData>::iterator it =
        g_map_premium_item_data.find((unsigned long)itemIdx);
    if (it == g_map_premium_item_data.end())
    {
        LogManager::logFormat(
            1, "SpecialItemHandler.cpp",
            __PRETTY_FUNCTION__,
            0x2ff, "PREMIUM_ERROR : NO PREMIUM ITEM!! : ITEM_ID (%u)",
            (unsigned int)itemIdx);
        return;
    }

    const STPremiumItemData& d = it->second;
    int ptype = d.m_value0;                     // 商品类型（jump 键）
    int term = (int)d.m_c4;                     // 期限（signed char）
    unsigned int flag2 = (unsigned int)(unsigned char)d.m_c5;  // 次字节
    int item = d.m_value8;                      // 限制物品 idx
    int resultVal = premium_helper_function::cal_premium_term(
        term, param.m_char1f != 0, param.m_extra);

    int restrictItem = item;
    int restrictIdx = itemIdx;

    switch (ptype)
    {
    case 1:
    case 3:
        resultVal = WongWork::CEventMgr::GetPremiumServiceBonus() + resultVal + 1;
        break;
    case 8:
        flag = 1;
        break;
    case 0xd:
    {
        bool b = false;
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xd) != 1 &&
            user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xe) == 1)
            b = true;
        if (b)
        {
            WongWork::SUserPremiumInfo* info = user->GetPremiumInfo()->GetPremiumInfo(0xe);
            if (info->m_type == 0xe)
            {
                timeVal = info->m_end;
                timeA = timeVal;
            }
        }
        break;
    }
    case 0xe:
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xd) != 0)
        {
            count = G_CDataManager()->getItemList()->GetRestrictCode(itemIdx);
            ptype = 0xd;
        }
        break;
    case 0xf:
        if (itemIdx == 200)
            resultVal = 0x1e;
        else if (itemIdx == 0xcb)
            resultVal = 3;
        else if (itemIdx == 0xcc)
            resultVal = 7;
        else if (itemIdx == 0xcd)
            resultVal = 0xe;
        if (user->getCurCharacInvenR()->GetCoin() < 3)
        {
            int coin = user->getCurCharacInvenR()->GetCoin();
            int payCoin = user->getCurCharacInvenR()->GetPayCoin();
            int eventCoin = user->getCurCharacInvenR()->GetEventCoin();
            int sum = coin + payCoin + eventCoin;
            int amt = 3;
            if (100000 < sum + 3)
                amt = 100000 - sum;
            if (0 < amt)
            {
                user->getCurCharacInvenW()->UpdateDailyCoin(amt);
                int c2 = user->getCurCharacInvenR()->GetCoin();
                user->m_historyLog.CoinAdd(c2, amt, (eCoinAddReason)4);
            }
        }
        break;
    case 0x16:
    case 0x1b:
    case 0x1d:
        count = G_CDataManager()->getItemList()->GetRestrictCode(restrictItem);
        flag = 1;
        break;
    case 0x21:
    {
        if (flag2 == 0 && count != 0 &&
            user->IsRestrictedGoods(count) != 1)
        {
            user->SetRestrictedGoods(count);
            int serverCode = 0;
            if (g_SPremiumInfo[ptype].m_useTargetServer == 1)
                serverCode = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x198+0x1e0
            m_owner->SaveFeaturedIdx(user, count, (int)timeB, serverCode);
        }
        count = G_CDataManager()->getItemList()->GetRestrictCode(restrictItem);
        flag = 1;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3a5, 0);
        trace("ONE_DAY_LETHE : BUY_RESTRICTED_PREMIUM, char(%s), char_no(%d)",
              user->getCurCharacName(), user->getCurCharacNo());
        if (G_CGameManager()->GetPremiumLetheManager() != 0)
        {
            if (user->IsEndLetheAtLogin() != 1)
            {
                G_CGameManager()->GetPremiumLetheManager()->BackupSkill(user, SKILL_TREE_1);
                G_CGameManager()->GetPremiumLetheManager()->BackupSkill(user, SKILL_TREE_2);
            }
            user->m_historyLog.SkillSPInfoSave(user, (eSkillSPInfoSaveReason)0);
            int job = user->get_charac_job();
            // getCurCharacSkillR() 返回当前角色 SkillSlot 对象（SkillSlot.h 权威布局：
            // m_skillSlot[204]@+0x46 / m_skillSlot2[204]@+0x1de），ORIG 裸偏移即两棵
            // 技能树已学技能块，经访问器按树序取用。
            const SkillSlot* skills =
                reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR());
            user->m_historyLog.SkillSave(skills->get_skillslot_buf(SKILL_TREE_1),
                                         (unsigned int)job, SKILL_TREE_1,
                                         (eSkillSaveReason)0);
            user->m_historyLog.SkillSave(skills->get_skillslot_buf(SKILL_TREE_2),
                                         (unsigned int)job, SKILL_TREE_2,
                                         (eSkillSaveReason)0);
        }
        break;
    }
    case 0x4f:
    {
        count = G_CDataManager()->getItemList()->GetRestrictCode(restrictItem);
        flag = 1;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3cb, 0);
        trace("GROWTH_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
              user->getCurCharacName(), user->getCurCharacNo());
        break;
    }
    case 0x53:
    {
        count = G_CDataManager()->getItemList()->GetRestrictCode(restrictItem);
        flag = 1;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3d3, 0);
        trace("EMOTICON_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
              user->getCurCharacName(), user->getCurCharacNo());
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x57) != 0)
            m_owner->_ClosePremium(user, 0x57);
        break;
    }
    case 0x57:
    {
        count = G_CDataManager()->getItemList()->GetRestrictCode(restrictItem);
        flag = 1;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3db, 0);
        trace("EMOTICON_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
              user->getCurCharacName(), user->getCurCharacNo());
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x53) != 0)
            m_owner->_ClosePremium(user, 0x53);
        break;
    }
    case 0x5c:
    {
        count = G_CDataManager()->getItemList()->GetRestrictCode(restrictItem);
        flag = 1;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3e5, 0);
        trace("CUBE_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
              user->getCurCharacName(), user->getCurCharacNo());
        break;
    }
    default:
        if (count == 0)
            count = G_CDataManager()->getItemList()->GetRestrictCode(itemIdx);
        break;
    }

    // ---- 公共收尾（switch 之后）----
    if (count != 0 && user->IsRestrictedGoods(count) != 1)
    {
        user->SetRestrictedGoods(count);
        int ts = premium_helper_function::get_target_server(ptype);
        m_owner->SaveFeaturedIdx(user, count, (int)timeB, ts);
    }

    bool needGain = false;
    if (g_SPremiumInfo[ptype].m_needDailyCheck == 0 ||
        user->CheckFatigue() == 1)
        needGain = false;
    else
        needGain = true;
    if (needGain)
        user->gainCurCharacFatigue(1);

    long end = 0;
    premium_helper_function::cal_end_time(user, timeVal, end, timeA, ptype,
                                          flag != 0, resultVal);
    premium_helper_function::send_db(user, ptype, timeVal, end, count);
    WongWork::CHandlePremium::handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)ptype,
                                                   timeVal, end, count, true);
    WongWork::CHandlePremium::handlePremiumEffect(user, (ENUM_PREMIUM_TYPE)ptype);
    if (premium_helper_function::is_notify(user, ptype) != 1)
        m_owner->_sendPremiumPacket(user, type, ptype, (int)(end - timeA));
    premium_helper_function::write_log(user, ptype);
}

// ============================================================================
// premium_helper_function（免费命名空间）—— ORIG 0x827cff0
// ============================================================================
// if(flag) term *= (byte)CDataManager+0x877e；return term * extra。
int premium_helper_function::cal_premium_term(int term, bool flag, int extra)
{
    if (flag)
        return (int)G_CDataManager()->m_boosterGage.m_cMultiboxCount * term * extra;
    return term * extra;
}

// ---- cal_end_time —— ORIG 0x0827d024 ----
// 计算 premium 开始/结束时间（param_6 时按既有 premium 结束续期）。
void premium_helper_function::cal_end_time(CUser* user, long& start, long& end,
                                           long base, int type, bool bActive,
                                           int term)
{
    if (bActive)
    {
        WongWork::CUserPremium* premium = user->GetPremiumInfo();
        WongWork::SUserPremiumInfo* info = premium->GetPremiumInfo(type);
        if (info->m_type == type)
        {
            end = (long)info->m_end + (long)term * 0x15180;
            start = (long)info->m_start;
        }
        else
        {
            end = (long)term * 0x15180 + base;
        }
    }
    else
    {
        end = (long)term * 0x15180 + base;
    }
    if (g_SPremiumInfo[type].m_timeLimit != 0 &&
        start < g_SPremiumInfo[type].m_timeLimit)
    {
        end = (long)g_SPremiumInfo[type].m_timeLimit;
    }
}

// ---- get_target_server —— ORIG 0x0827d0f5 ----
// g_SPremiumInfo[type].m_useTargetServer(+0x5c)==1 时返回
// G_CEnvironment()->m_serverEnvir.m_serverGroup（CEnvironment+0x378），否则 0。
int premium_helper_function::get_target_server(int type)
{
    if (g_SPremiumInfo[type].m_useTargetServer == 1)
        return G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x198+0x1e0
    return 0;
}

// ---- send_db —— ORIG 0x0827d127 ----
// 投递 0x54 头 premium DB 流（uid/accId/type/start/end/flag/serverCode）至 DB_Q。
void premium_helper_function::send_db(CUser* user, int type, long start, long end,
                                      int flag)
{
    // ORIG 0x0827d0f5：acc_id/target_server 按无符号(lsEj)、start/end 按 long(lsEl) 写。
    Stream* stream = GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0x295);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<((int)0x54);
    guard.operator*()->operator<<((int)user->GetUID());
    guard.operator*()->operator<<((unsigned int)user->get_acc_id());
    guard.operator*()->operator<<((int)type);
    guard.operator*()->operator<<(start);
    guard.operator*()->operator<<(end);
    guard.operator*()->operator<<((int)flag);
    guard.operator*()->operator<<((unsigned int)get_target_server(type));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- is_notify —— ORIG 0x0827d29d ----
// premium 项存在且 m_state==2（等待开始）时返回 1。
int premium_helper_function::is_notify(CUser* user, int type)
{
    WongWork::CUserPremium* premium = user->GetPremiumInfo();
    WongWork::SUserPremiumInfo* info = premium->GetPremiumInfo(type);
    if (info->m_type == type)
        return (info->m_state == 2) ? 1 : 0;
    return 0;
}

// ---- write_log —— ORIG 0x0827d2df ----
// type==0x53/0x57：重算+发表情；type==8：组 2 包并广播 basic_info。
void premium_helper_function::write_log(CUser* user, int type)
{
    if (type == 0x53 || type == 0x57)
    {
        user->ReCalcChattingEmoticon();
        user->SendChattingEmoticon();
    }
    else if (type == 8)
    {
        PacketGuard packet;
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info((char*)&packet, '\0');
        packet.finalize(true);
        G_GameWorld()->send_all(packet, user);
    }
}

namespace WongWork
{

// ---- _CheckBuyingCondition 分支内部辅助（file-static，不属 CCeraShop 成员）----

// case 0x03 / 0x06 事件窗口：创角时间落在 [start,end) 内返回 1。
static int _cbcEventTimeWindow(CUser* user, long startTime, long endTime)
{
    long createTime = user->getCurCharacCreateTime();
    return (createTime >= startTime && createTime < endTime) ? 1 : 0;
}

// case 0x04 仓库容量精确匹配：GetCapacity() 与固定容量表比较。
static int _cbcCargoCapacity(CUser* user, unsigned long itemIdx)
{
    int cap = user->getCurCharacCargoR()->GetCapacity();
    if (itemIdx == 0x3c)
        return cap == 0x48 ? 1 : 0;
    if (itemIdx < 0x3d)
    {
        if (itemIdx == 0x39)
            return cap == 0x18 ? 1 : 0;
        if (itemIdx < 0x3a)
        {
            if (itemIdx == 0x32)
                return cap == 8 ? 1 : 0;
        }
        else if (itemIdx == 0x3a)
            return cap == 0x28 ? 1 : 0;
        else if (itemIdx == 0x3b)
            return cap == 0x38 ? 1 : 0;
    }
    else if (itemIdx == 0x3e)
        return cap == 0x68 ? 1 : 0;
    else if (itemIdx < 0x3e)
        return cap == 0x58 ? 1 : 0;
    else if (itemIdx == 0x28978b)
        return cap == 0x78 ? 1 : 0;
    else if (itemIdx == 0x28978c)
        return cap == 0x88 ? 1 : 0;
    return 0;
}

// case 0x05 背包容量+造物：check_item_exist 于背包/仓库均 -1 时 create_item 后
// tryInsertItemIntoInventory 成功返回 1。
// ORIG 顺序：Inven_Item::Inven_Item → m_addInfo=itemIdx → create_item(type=0,item,1)。
static int _cbcInvenInsertItem(CUser* user, unsigned long itemIdx)
{
    const CInventory* inv = user->getCurCharacInvenR();
    if (inv->check_item_exist((int)itemIdx) != -1)
        return 0;
    const CCargo* cargo = user->getCurCharacCargoR();
    if (cargo->check_item_exist((int)itemIdx) != -1)
        return 0;

    Inven_Item item;
    item.m_addInfo = (int)itemIdx;  // ORIG uStack_5b = param_3（写入 m_addInfo@+0x02）
    G_CDataManager()->getItemList()->create_item(
        (ENUM_ITEM_CREATE_TYPE)0, item, 1);
    if (user->getCurCharacInvenW()->tryInsertItemIntoInventory(item) >= 0)
        return 1;
    return 0;
}

// case 0x06 事件商品集：时间窗内要求背包/仓库均无 0x65d6..0x65da 任一。
static int _cbcEventItemSet(CUser* user, unsigned long itemIdx)
{
    if (!_cbcEventTimeWindow(user, 0x46a7b9d0, 0x46d5de50))
        return 0;
    const CInventory* inv = user->getCurCharacInvenR();
    const CCargo* cargo = user->getCurCharacCargoR();
    for (int id = 0x65d6; id <= 0x65da; ++id)
    {
        if (inv->check_item_exist(id) != -1 || cargo->check_item_exist(id) != -1)
            return 0;
    }
    return 1;
}

// case 0x0c 角色槽/服务器环境：m_serverType==2 或 m_serverGroup∈{5,0xb,0xc} 阻止。
static int _cbcEnvAndSlotLimit(CUser* user)
{
    CEnvironment* env = G_CEnvironment();
    if (env->m_serverEnvir.m_serverType == 2)
        return 0;
    int grp = env->m_serverEnvir.m_serverGroup;
    if (grp == 0xb || grp == 0xc || grp == 5)
        return 0;
    if (user->getCharacSlotLimit() < 0x15)
        return 1;
    return 0;
}

// case 0x0d 背包容量精确：0x2897c8→cap==0，0x2897c9→cap==8。
static int _cbcInventoryCapacity(CUser* user, unsigned long itemIdx)
{
    int cap = user->getCurCharacInvenR()->get_inventory_capacity();
    if (itemIdx == 0x2897c8)
        return cap == 0 ? 1 : 0;
    if (itemIdx == 0x2897c9)
        return cap == 8 ? 1 : 0;
    return 0;
}

// case 0x10 转职/可堆叠限定：job 9/10 阻止；否则 verifyStackableItemActionPlace(TYPE_CLASS_CHANGE=0x2e)。
static int _cbcCharacJobStackable(CUser* user, unsigned long itemIdx)
{
    if (user->get_charac_job() == 9)
        return 0;
    if (user->get_charac_job() == 10)
        return 0;
    return user->verifyStackableItemActionPlace(StackableAction::TYPE_CLASS_CHANGE,
                                                (unsigned int)itemIdx);
}

// ============================================================================
// checkBuyingCondition —— ORIG 0x08323f2a (T)
// 仅转发到 _CheckBuyingCondition(CUser*, uint, ulong, uint, ushort, uchar, ushort)。
// ============================================================================
int CCeraShop::checkBuyingCondition(CUser* user, unsigned int a, unsigned long itemIdx,
                                    unsigned int b, unsigned short c, unsigned char d,
                                    unsigned short e)
{
    return _CheckBuyingCondition(user, a, itemIdx, b, c, d, e);
}

// ============================================================================
// _CheckConditionPutonAvatar —— ORIG 0x08324966 (T)
// 遍历角色装备槽（INVEN 0，slot 0..9）与头像槽（INVEN 2，slot 0..0x68），
// 任一槽位 Inven_Item 的 m_addInfo(+0x02) 非 0 即返回 1（已有装备/头像），否则 0。
// ============================================================================
int CCeraShop::_CheckConditionPutonAvatar(CUser* user)
{
    const CInventory* inv = user->getCurCharacInvenR();
    for (int i = 0; i <= 9; ++i)
    {
        const Inven_Item* it = inv->GetInvenRef(0, i);
        if (it != 0 && it->m_addInfo != 0)
            return 1;
    }
    for (int i = 0; i <= 0x68; ++i)
    {
        const Inven_Item* it = inv->GetInvenRef(2, i);
        if (it != 0 && it->m_addInfo != 0)
            return 1;
    }
    return 0;
}

// ============================================================================
// _CheckBuyingCondition —— ORIG 0x08323f86 (T)
// 商城购买前置条件分发（uint/ulong 重载，被 checkBuyingCondition 与
// _CheckBuyingCondition(CCeraShopGoods*) 共用）。按 itemIdx 归一化 category
// 后分派 0x01/_CheckConditionPutonAvatar、0x03（创角时间窗）、0x04（仓库容量）、
// 0x05（背包容量+造物）、0x06（事件商品存在性）、0x08（等级）、0x0b（技能树）、
// 0x0c（角色槽/服务器环境）、0x0d（背包容量精确）、0x10（转职/可堆叠限定）。
// ============================================================================
int CCeraShop::_CheckBuyingCondition(CUser* user, unsigned int a, unsigned long itemIdx,
                                     unsigned int b, unsigned short c, unsigned char d,
                                     unsigned short e)
{
    // local_20：归一化 category。ORIG 先按 itemIdx 特例置 1/3，再按区间映射。
    int category = 0;

    if (a == 0x1adbd)
        category = 1;
    else if (a == 0x1c13c)
        category = 3;

    if (b != 1)
    {
        CItem* citem = G_CDataManager()->find_item((int)itemIdx);
        if (citem == 0)
            return 0;
        if (citem->IsAvatarItem())  // vtable +0x10（IsAvatarItem）
        {
            bool inner;
            if (e == 0)
            {
                inner = true;  // LAB_0832403b
            }
            else
            {
                std::vector<stAvatarTypeSelect_t>* sel =
                    (std::vector<stAvatarTypeSelect_t>*)(
                        ((CEquipItem*)citem)->getAvatarTypeSelect());
                if (sel->size() <= e)
                    inner = true;
                else
                    inner = false;
            }
            if (inner)
                return 0;
            if (e != c)
                return 0;
            std::vector<stAvatarTypeSelect_t>* sel =
                (std::vector<stAvatarTypeSelect_t>*)(
                    ((CEquipItem*)citem)->getAvatarTypeSelect());
            stAvatarTypeSelect_t& selItem = (*sel)[e];  // at(e)
            int slotCount = selItem.m_field10;
            if (c != 4 && slotCount != 0)
                return 0;
        }
    }

    // 归一化 category（itemIdx 区间/特例映射，见报告汇编 0x0239..0x0241）。
    if (itemIdx < 0x34a)
    {
        if (itemIdx > 0x346)
            category = 0xb;
        else if (itemIdx < 0x44)
        {
            if (itemIdx < 0x3f)
            {
                if (itemIdx == 0x33)
                    category = 5;
                else if (itemIdx < 0x34)
                {
                    if (itemIdx == 0x32)
                        category = 4;
                }
                else if (itemIdx > 0x38)
                    category = 4;
            }
            else
                category = 6;
        }
        else if (itemIdx < 0xc1)
        {
            if (itemIdx < 0xbe)
            {
                if (itemIdx - 0x45 < 5)
                    category = 6;
            }
            else
                category = 0xb;
        }
        else if (itemIdx == 0x2e4 || itemIdx == 0x335)
            category = 0xb;
    }
    else
    {
        if (itemIdx > 0x28978c)
        {
            if (itemIdx < 0x2897ca)
            {
                if (itemIdx < 0x2897c8)
                {
                    if (itemIdx == 0x28978f)
                        category = 0xc;
                }
                else
                    category = 0xd;
            }
            else if (itemIdx > 0x28995c &&
                     (itemIdx < 0x28995f || itemIdx == 0x2899ab))
                category = 0x10;
        }
        else
        {
            if (itemIdx > 0x28978a)
                category = 4;
            else if (itemIdx == 0x1ee2 || itemIdx == 0x3a0)
                category = 0xb;
            else if (itemIdx > 0x1ee3)
            {
                if (itemIdx == 0x2896cb)
                    category = 0xb;
                else if (itemIdx == 0x28978a)
                    category = 5;
            }
        }
    }

    if (category == 0)
        return 1;

    switch (category)
    {
    case 1:
        return _CheckConditionPutonAvatar(user);

    case 3:
        return _cbcEventTimeWindow(user, 0x4589a450, 0x45ae8e50);

    case 4:
        return _cbcCargoCapacity(user, itemIdx);

    case 5:
        return _cbcInvenInsertItem(user, itemIdx);

    case 6:
        return _cbcEventItemSet(user, itemIdx);

    case 8:
        return user->get_charac_level() > 0xe ? 1 : 0;

    case 0xb:
        return user->GetCurCharacSkillTreeIndex() == -1 ? 1 : 0;

    case 0xc:
        return _cbcEnvAndSlotLimit(user);

    case 0xd:
        return _cbcInventoryCapacity(user, itemIdx);

    case 0x10:
        return _cbcCharacJobStackable(user, itemIdx);
    }

    return 0;
}

// ============================================================================
// IsSpecialItem —— ORIG 0x08324a1e (T)
// 固定商品表：itemId → (type, value, flag)；表外回退 CDataManager 物品类型判定。
// ============================================================================
bool CCeraShop::IsSpecialItem(int itemIdx, int& outType, int& outValue, bool& flag)

{
    switch (itemIdx)
    {
    case 5:
        outType = 1;
        outValue = 0x18;
        return true;

    case 6:
        outType = 1;
        outValue = 0x98;
        return true;

    case 9:
        outType = 2;
        outValue = 1;
        return true;

    case 11:
        outType = 2;
        outValue = 3;
        return true;

    case 16:
        outType = 4;
        outValue = 8;
        return true;

    case 18:
        outType = 2;
        outValue = 9;
        flag = true;
        return true;

    case 19:
        outType = 2;
        outValue = 10;
        flag = true;
        return true;

    case 20:
        outType = 2;
        outValue = 0xb;
        flag = true;
        return true;

    case 22:
        outType = 2;
        outValue = 0xd;
        flag = false;
        return true;

    case 23:
        outType = 2;
        outValue = 0xe;
        flag = false;
        return true;

    case 29:
        outType = 2;
        outValue = 0x10;
        flag = false;
        return true;

    case 30:
        outType = 2;
        outValue = 0x11;
        flag = true;
        return true;

    case 31:
        outType = 2;
        outValue = 0x12;
        flag = true;
        return true;

    case 32:
        outType = 2;
        outValue = 0x13;
        flag = true;
        return true;

    case 33:
        outType = 2;
        outValue = 0x14;
        flag = true;
        return true;

    case 34:
        outType = 2;
        outValue = 0x15;
        flag = true;
        return true;

    case 35:
        outType = 2;
        outValue = 0x16;
        flag = true;
        return true;

    case 43:
        outType = 2;
        outValue = 0x17;
        flag = true;
        return true;

    case 44:
        outType = 2;
        outValue = 0x18;
        flag = true;
        return true;

    case 45:
        outType = 2;
        outValue = 0x19;
        flag = true;
        return true;

    case 47:
        outType = 2;
        outValue = 0x1b;
        flag = true;
        return true;

    case 50:
    case 2660235:
    case 2660236:
        outType = 7;
        outValue = 0x10;
        return true;

    case 68:
        outType = 1;
        outValue = 0x48;
        return true;

    case 110:
        outType = 1;
        outValue = 0x28;
        return true;

    case 137:
        outType = 2;
        outValue = 0x1d;
        flag = true;
        return true;

    case 193:
        outType = 2;
        outValue = 0x1e;
        flag = true;
        return true;

    case 194:
        outType = 2;
        outValue = 0x1f;
        flag = true;
        return true;

    case 196:
        outType = 2;
        outValue = 0x20;
        flag = true;
        return true;

    case 197:
        outType = 2;
        outValue = 0x21;
        flag = true;
        return true;

    case 198:
        outType = 2;
        outValue = 0x21;
        flag = true;
        return true;

    case 200:
        outType = 2;
        outValue = 0x28;
        flag = true;
        return true;

    case 720:
        outType = 2;
        outValue = 0x23;
        flag = true;
        return true;

    case 741:
        outType = 2;
        outValue = 0x24;
        flag = true;
        return true;

    case 742:
        outType = 2;
        outValue = 0x25;
        flag = true;
        return true;

    case 743:
        outType = 2;
        outValue = 0x26;
        flag = true;
        return true;

    case 744:
        outType = 2;
        outValue = 0x27;
        flag = true;
        return true;

    case 820:
        outType = 2;
        outValue = 0x29;
        flag = true;
        return true;

    case 821:
        outType = 10;
        outValue = 0;
        return true;

    case 920:
        outType = 2;
        outValue = 0x2c;
        flag = true;
        return true;

    case 2660012:
        outType = 2;
        outValue = 0x2d;
        flag = true;
        return true;

    case 2660013:
        outType = 2;
        outValue = 0x2e;
        flag = true;
        return true;

    case 2660050:
        outType = 2;
        outValue = 0x2f;
        flag = true;
        return true;

    case 2660051:
        outType = 2;
        outValue = 0x30;
        flag = true;
        return true;

    case 2660239:
        outType = 8;
        outValue = 1;
        return true;

    case 2660290:
        outType = 2;
        outValue = 0x21;
        flag = true;
        return true;

    case 2660358:
        outType = 2;
        outValue = 0x21;
        flag = true;
        return true;

    case 2660401:
        outType = 2;
        outValue = 0x23;
        flag = true;
        return true;

    case 2660402:
        outType = 2;
        outValue = 0x26;
        flag = true;
        return true;

    case 2660408:
        outType = 2;
        outValue = 0x4f;
        flag = true;
        return true;

    case 2660409:
        outType = 2;
        outValue = 0x4c;
        flag = true;
        return true;

    case 2660410:
        outType = 2;
        outValue = 0x4d;
        flag = true;
        return true;

    case 2660411:
        outType = 2;
        outValue = 0x4e;
        flag = true;
        return true;

    case 2660543:
        outType = 2;
        outValue = 0x50;
        flag = true;
        return true;

    case 2660544:
        outType = 2;
        outValue = 0x53;
        flag = true;
        return true;

    case 2660546:
        outType = 2;
        outValue = 0x57;
        flag = true;
        return true;

    case 2660703:
        outType = 2;
        outValue = 0x4f;
        flag = true;
        return true;

    case 2670009:
        outType = 1;
        outValue = 0x28;
        return true;

    case 2670266:
        outType = 2;
        outValue = 0x46;
        flag = true;
        return true;

    case 2675025:
        outType = 2;
        outValue = 0x11;
        flag = true;
        return true;

    case 2675026:
        outType = 2;
        outValue = 0x17;
        flag = true;
        return true;

    case 2675028:
        outType = 2;
        outValue = 0x1f;
        flag = true;
        return true;

    case 2675057:
        outType = 4;
        outValue = 0x2a;
        return true;

    case 2675058:
        outType = 2;
        outValue = 0x2b;
        flag = true;
        return true;

    case 10000387:
        outType = 2;
        outValue = 0x5c;
        flag = true;
        return true;

    case 10000388:
        outType = 2;
        outValue = 0x59;
        flag = true;
        return true;

    case 10000389:
        outType = 2;
        outValue = 0x5a;
        flag = true;
        return true;

    case 10000390:
        outType = 2;
        outValue = 0x5b;
        flag = true;
        return true;

    case 10000391:
        outType = 2;
        outValue = 0x5d;
        flag = true;
        return true;

    case 690000221:
        outType = 0xd;
        outValue = 10;
        flag = true;
        return true;

    case 690000222:
        outType = 0xd;
        outValue = 0x32;
        flag = true;
        return true;

    case 690000223:
        outType = 0xd;
        outValue = 100;
        flag = true;
        return true;

    case 690000224:
        outType = 0xd;
        outValue = 500;
        flag = true;
        return true;

    case 690000225:
        outType = 0xd;
        outValue = 1000;
        flag = true;
        return true;

    default:
        break;
    }

    // 默认路径（ORIG +0xd0d）：可堆叠物品按 GetItemType 分类。
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item != 0 && item->is_stackable())
    {
        int type = item->GetItemType();
        if (type == 0x26)
            return false;
        if (type == 0xe)
        {
            outType = 5;
            flag = true;
            return true;
        }
        if (type == 0xf || type == 0x10)
        {
            outType = 6;
            flag = true;
            return true;
        }
        if (type == 0x22)
        {
            outType = 0xc;
            return true;
        }
    }
    return false;
}

// ============================================================================
// ProcessSpecialItem —— ORIG 0x0832587e (T)
// ============================================================================
bool CCeraShop::ProcessSpecialItem(CUser* user, int itemIdx, int count,
                                   int& restrictCode, long curSec, bool flag,
                                   std::vector<Inven_Item>& items,
                                   stCeraShopItemParam_t& param,
                                   char c1, char c2, bool flag2, int extra)
{
    if (user->getCurCharacR() == 0)
        return false;

    // 特殊商品分类：先查 premium 数据映射（g_map_premium_item_data 未建模，
    // ORIG 命中时固定 type=2），未命中则走 IsSpecialItem。
    int type = 0;
    int value = 0;
    bool f = false;

    if (!IsSpecialItem(itemIdx, type, value, f))
        return false;

    if (count == 0 && (itemIdx == 0x10 || itemIdx == 0x28d171))
        count = 50000;

    // GetPremiumLetheManager 结果在 ORIG 中为死存储（+0x68 局部），省略。

    STSpecailItem_Param p(items, param);
    p.m_user = user;
    p.m_itemIdx = itemIdx;
    p.m_value = value;
    p.m_count = count;
    p.m_time = curSec;
    p.m_flag = flag ? 1 : 0;
    p.m_char1d = (unsigned char)c1;
    p.m_char1e = (unsigned char)c2;
    p.m_char1f = flag2 ? 1 : 0;
    p.m_extra = extra;

    // m_handler：CCeraShop +0x00 的 SpecialItemHandler*（权威具名成员，ctor new 填充）。
    STSpecailItem_Result result;
    if (!m_handler->execute(type, p, result))
        return false;

    restrictCode = result.m_value;
    return true;
}

// ============================================================================
// _CheckBuyingCondition —— ORIG 0x08324810 (T)
//   CCeraShopGoods 重载。groupIdx==1 且 param6==0：按金币家庭（GetCoin+GetPayCoin+
//   GetEventCoin）+stackCount 判定金币上限（<0x186a1 通过，否则 outCode=0x4f）。
//   其余 groupIdx 委托 uint 重载 _CheckBuyingCondition(CUser*, uint, ulong, uint,
//   ushort, uchar, ushort)；子返回非 1 则 outCode=8。
// ============================================================================
bool CCeraShop::_CheckBuyingCondition(CUser* user, const CCeraShopGoods* goods,
                                      unsigned int param3, unsigned char param4,
                                      unsigned short param5, char param6, int& outCode)
{
    int groupIdx = goods->GetGroupIdx();
    if (groupIdx == 1 && param6 == 0)
    {
        const CInventory* inv = user->getCurCharacInvenR();
        unsigned int coin = inv->GetCoin();
        unsigned int payCoin = inv->GetPayCoin();
        unsigned int eventCoin = inv->GetEventCoin();
        int stackCount = goods->GetStackCount();
        if ((int)(coin + payCoin + eventCoin + (unsigned int)stackCount) < 0x186a1)
        {
            outCode = 0;
            return true;
        }
        outCode = 0x4f;
        return false;
    }
    unsigned short expiryDate = (unsigned short)goods->GetExpiryDate();
    unsigned int stackCount = (unsigned int)goods->GetStackCount();
    unsigned long itemIdx = (unsigned long)goods->GetItemIdx();
    bool ok = _CheckBuyingCondition(user, param3, itemIdx, (unsigned int)stackCount,
                                    (unsigned short)expiryDate, param4, param5);
    if (!ok)
        outCode = 8;
    return ok;
}

// ============================================================================
// _CheckValidCount —— ORIG 0x08323af4 (T)
//   双重校验：1) 条目 count 累计（IsSpecialItem 分组），任一分组 >1 则失败；
//   2) 对每个 stack-buyable 商品，按 restrictCode 校验限制购买仍为可重复。
//   返回 true=通过；其间任何限制冲突返回 false。
// ============================================================================
bool CCeraShop::_CheckValidCount(const CCeraShopGoods** goodsList)
{
    int typeCount[15];
    std::memset(typeCount, 0, sizeof(typeCount));

    int n = 0;
    for (; n < 0x23 && goodsList[n] != 0; ++n)
    {
        for (int j = n + 1; j < 0x23 && goodsList[j] != 0; ++j)
        {
            if (goodsList[n]->GetItemIdx() == goodsList[j]->GetItemIdx())
            {
                int groupIdx = goodsList[n]->GetGroupIdx();
                if (groupIdx == 3)
                {
                    CItem* item = G_CDataManager()->find_item(goodsList[n]->GetItemIdx());
                    if (item == 0 || !item->is_stackable())
                        return false;
                }
                else if (groupIdx == 7 || groupIdx == 0)
                {
                    return false;
                }
            }
        }

        bool specialFlag = false;
        int type = 0, value = 0;
        if (IsSpecialItem(goodsList[n]->GetItemIdx(), type, value, specialFlag))
        {
            ++typeCount[type];
            if (specialFlag)
            {
                // 上限 1：typeCount[type] 取 min(typeCount[type]-1, 1) 后写回
                int cur = typeCount[type] - 1;
                typeCount[type] = (cur < 1 ? cur : 1);
            }
        }
    }

    for (int i = 1; i < 0xf; ++i)
    {
        if (typeCount[i] > 1)
            return false;
    }

    // 可叠购买商品按 restrictCode 校验（CItemList::GetRestrictCode）。
    typedef std::pair<const CCeraShopGoods*, unsigned int> GoodsCount;
    std::vector<GoodsCount> counts;
    for (int i = 0; i < 0x23; ++i)
    {
        if (goodsList[i] == 0)
            continue;
        if (goodsList[i]->isStackBuyable())
        {
            int restrictCode = G_CDataManager()->getItemList()->GetRestrictCode(
                goodsList[i]->GetItemIdx());
            if (restrictCode != 0)
            {
                // 按商品指针找到已有计数并自增；否则插入新项 count=1。
                bool found = false;
                for (std::vector<GoodsCount>::iterator it = counts.begin();
                     it != counts.end(); ++it)
                {
                    if (it->first == goodsList[i])
                    {
                        ++(it->second);
                        found = true;
                        break;
                    }
                }
                if (!found)
                    counts.push_back(GoodsCount(goodsList[i], 1));
            }
        }
    }

    for (std::vector<GoodsCount>::iterator it = counts.begin();
         it != counts.end(); ++it)
    {
        if (it->second > 1)
            return false;
    }

    return true;
}

// ============================================================================
// ProcessError —— ORIG 0x08323978 (T)
// 语义：构造 0x43 错误包（header 1/0x43），按调用方错误码序列化，Send 后记
// cMyTrace "NEXON BILLING : Buy Error(%d,%u)"。
// C 反编译：put_header(1,0x43); put_byte(0); put_byte(0); put_short(param_2);
//           put_byte(param_7); put_int(param_3); put_int(param_4);
//           put_int(param_5); put_int(param_6); put_int(-1); finalize(true);
//           user->Send(packet); cMyTrace(...)。
// ============================================================================
void CCeraShop::ProcessError(CUser* user, unsigned short a, int b, unsigned int c,
                             int d, int e, char f)
{
    PacketGuard packet;
    packet.put_header(1, 0x43);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_short(a);
    packet.put_byte(f);
    packet.put_int(b);
    packet.put_int(c);
    packet.put_int(d);
    packet.put_int(e);
    packet.put_int(-1);
    packet.finalize(true);
    user->Send(packet);

    cMyTrace trace(__PRETTY_FUNCTION__, 0xc2a, 0);
    trace("NEXON BILLING : Buy Error(%d,%u)", (int)a, c);
}

// ============================================================================
// SaveFeaturedIdx —— ORIG 0x08325a30 (T)
// ============================================================================
void CCeraShop::SaveFeaturedIdx(CUser* user, int restrictCode, int curSec, int d)
{
    if (restrictCode == 0)
        return;
    int characNo = user->get_charac_no(-1);
    unsigned int accId = user->get_acc_id();
    DB_SaveFeaturedGoodsIdx::makeRequest(accId, characNo, restrictCode, curSec, d);
}

// ============================================================================
// setOriginalItem —— ORIG 0x08327ee6 (T)：记录原始商品 idx 到 m_originalItemIdx。
// ============================================================================
void CCeraShop::setOriginalItem(int itemIdx)
{
    m_originalItemIdx = itemIdx;
}

// ============================================================================
// getOriginalItem —— ORIG 0x08327ef4 (T)：读取记录的原始商品 idx（<=0 返回 0）。
// ============================================================================
int CCeraShop::getOriginalItem()
{
    return m_originalItemIdx > 0 ? m_originalItemIdx : 0;
}

// ============================================================================
// Is2010ChuseokPacket —— ORIG 0x083280dc (T)
// 2010 秋夕（Chuseok）包商品判定：itemIdx ∈ (0x2896d3, 0x2896ef]。
// ============================================================================
bool CCeraShop::Is2010ChuseokPacket(unsigned int itemIdx)
{
    return itemIdx > 0x2896d3 && itemIdx <= 0x2896ef;
}

// ============================================================================
// _processCoin —— ORIG 0x08325a88 (T)
// 代币购买入账：PayCoin += value → historyLog.PayCoinAdd(总额, 增量, reason 3)
// → SaveMoneyCoin → SendCoin。第 3 参 flag 在 ORIG 中未参与逻辑（签名保留）。
// ============================================================================
void CCeraShop::_processCoin(CUser* user, unsigned short value, bool)
{
    user->getCurCharacInvenW()->SetPayCoin(
        user->getCurCharacInvenR()->GetPayCoin() + value);
    user->m_historyLog.PayCoinAdd(
        (int)user->getCurCharacInvenR()->GetPayCoin(), (int)value,
        (eCoinAddReason)3);
    user->SaveMoneyCoin();
    user->SendCoin();
}

// ============================================================================
// autoUnsealItem —— ORIG 0x08327f10 (T)
// 自动解封：商品为随机选项物品（IsRandomOption）且原商品
// （getOriginalItem→find_item）IsRandomOptionUnseal 时，经
// GlobalData::g_randomOptionHandle->give_option 写入 invItem 的随机选项区
// （m_random.m_random，ORIG &invItem+0x25）。失败记 logFormat。
// ============================================================================
void CCeraShop::autoUnsealItem(const CItem* item, Inven_Item& invItem)
{
    if (item != 0 && item->IsRandomOption())
    {
        int original = getOriginalItem();
        if (original != 0)
        {
            const CItem* origItem = G_CDataManager()->find_item(original);
            if (origItem != 0 && origItem->IsRandomOptionUnseal())
            {
                // ORIG 实参序 vtable 槽核对 —— get_rarity/getUsableLevel/
                // getItemGroupName/GetRandomOptionGrade(vtable+0x50)；
                // options = invItem.m_random.m_random（ORIG &invItem+0x25）。
                if (!GlobalData::g_randomOptionHandle->give_option(
                        (unsigned long)invItem.m_addInfo,
                        (ENUM_RARITY)item->get_rarity(),
                        item->getUsableLevel(), item->getItemGroupName(),
                        item->GetRandomOptionGrade(), &invItem.m_random.m_random))
                {
                    LogManager::logFormat(
                        1, "CeraShop.cpp",
                        __PRETTY_FUNCTION__,
                        0x2131, "autoUnsealItem : %d", invItem.m_addInfo);
                }
            }
        }
    }
}

// ============================================================================
// _processItem —— ORIG 0x08325b22 (T)
// 一般可叠/装备商品主路径：ProcessSpecialItem 扣费分发（c1=c2=0、flag2=false、
// extra=item.m_addInfo2）→ AvatarCoin::BuyProcess → insertItemIntoInventory(0xf)
// → SendUpdateItemList（artifact 走 ITEMSPACE 7）→ save 时 SaveInventory；
// 入包失败走 ReqDBSendNewMailCashShop 邮件兜底并返回 0。
// ============================================================================
int CCeraShop::_processItem(CUser* user, Inven_Item item, int count, int& restrictCode,
                            long curSec, bool flag, bool save,
                            std::vector<Inven_Item>& items,
                            stCeraShopItemParam_t& param)
{
    int slot = -1;
    if (!ProcessSpecialItem(user, item.m_addInfo, count, restrictCode, curSec,
                            flag, items, param, 0, 0, false, item.m_addInfo2))
    {
        if (AvatarCoin::BuyProcess(user, item))
            return 0;

        if ((slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                 item, (eItemAddReason)0xf, true, false)) != -1)
        {
            if (item.m_field1 == Inven_Item::ITEM_TYPE_ARTIFACT)
                user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7,
                                         slot);
            else
                user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                         slot);
            if (save)
                user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104",
                                                  (bool*)0);
            WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                user, item, 0, (unsigned int)user->getCurCharacNo(), true, msg,
                (int)std::strlen(msg));
            slot = 0;
        }
        return slot;
    }
    return 0;
}

// ============================================================================
// _processCreature —— ORIG 0x08325d66 (T)
// 造物（creature）商品：CCreatureMgr::CheckItem 预检（失败置 outFlag=1）→
// type==5（蛋）时 SetEggItem 孵化转换（失败记 History ERROR 并置 outFlag=1）→
// insertItemIntoInventory(0x19)：成功则 InsertCreatureItem + 刷新列表 +
// SaveInventory，outCode=GetInvenSlot(3,slot).m_addInfo；失败按类型走
// Creature/CashShop 邮件兜底，outCode=item.m_addInfo、返回 0。
// ============================================================================
int CCeraShop::_processCreature(CUser* user, Inven_Item item, int& outCode,
                                int& outFlag, bool, int usablePeriod,
                                int expirationDate)
{
    int slot = -1;
    bool checkOk = true;
    if (!user->getCurCharacInvenW()->GetCreatureMgrW()->CheckItem(user, &item,
                                                                  checkOk))
        outFlag = 1;
    if (checkOk)
    {
        if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        {
            if (!user_creature::CCreatureItemConverter::SetEggItem(&item))
            {
                LogManager::logFormat(
                    1, "CeraShop.cpp",
                    __PRETTY_FUNCTION__,
                    0x171f, "History ERROR, CCreatureItemConverter::SetEggItem");
                outFlag = 1;
            }
        }

        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)0x19, true, false);
        if (slot < 0)
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104",
                                                  (bool*)0);
            if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
                WongWork::CMailBoxHelper::ReqDBSendNewCreatureMail(
                    user, user->getCurCharacNo(), item, msg,
                    (int)std::strlen(msg));
            else
                WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                    user, item, 0, (unsigned int)user->getCurCharacNo(), true,
                    msg, (int)std::strlen(msg));
            outCode = item.m_addInfo;
            return 0;
        }
        else
        {
            user->getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
                &item, slot, 0x19, usablePeriod, expirationDate);
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7,
                                     slot);
            user->SaveInventory();
            Inven_Item placed = user->getCurCharacInvenR()->GetInvenSlot(3, slot);
            outCode = placed.m_addInfo;  // ORIG 读 sret 缓冲 +2 处 dword
        }
    }
    return slot;
}

// ============================================================================
// _sendCargoUpgradePacket —— ORIG 0x08327726 (T)
// 组 0x42 仓库扩容包：header(0,0x42) / short(type) / short(GetCapacity)。
// ============================================================================
void CCeraShop::_sendCargoUpgradePacket(CUser* user, int type)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(type);
    int cap = user->getCurCharacCargoR()->GetCapacity();
    packet.put_short(cap);
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// _sendCharacSlotExtendPacket —— ORIG 0x08327674 (T)
// 组 0x42 角色槽扩展包：header(0,0x42) / short(type) / byte(getCharacSlotLimit()).
// ============================================================================
void CCeraShop::_sendCharacSlotExtendPacket(CUser* user, int type)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(type);
    unsigned char slotLimit = user->getCharacSlotLimit();
    packet.put_byte(slotLimit & 0xff);
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// _sendInventoryUpgradePacket —— ORIG 0x083275be (T)
// 组 0x42 背包扩容包：header(0,0x42) / short(type) / short(get_inventory_capacity()).
// ============================================================================
void CCeraShop::_sendInventoryUpgradePacket(CUser* user, int type)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(type);
    int cap = user->getCurCharacInvenR()->get_inventory_capacity();
    packet.put_short(cap);
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// _sendPremiumPacket —— ORIG 0x083277dc (T)
// 组 0x42 高级会员购买通知包：header(0,0x42)/short(param2)/byte(param3)/int(param4)。
// ============================================================================
void CCeraShop::_sendPremiumPacket(CUser* user, int a, int type, int remainSec)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(a);
    packet.put_byte((unsigned char)type);
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// _ClosePremium —— ORIG 0x08327bb6 (T)
// 关闭 type 指定 premium：取 GetPremiumInfo(type)，非空则逐项投递 LOGDB_Q
// （0x54 头 + uid + accId + type + curSec + curSec+1 + flag + serverCode），
// 并调 CHandlePremium::handleSetUserPremium / handlePremiumEffect /
// handleNotifyPremiumInfo。
// ============================================================================
void CCeraShop::_ClosePremium(CUser* user, int type)
{
    WongWork::CUserPremium* premium = user->GetPremiumInfo();
    std::vector<WongWork::SUserPremiumInfo> list;
    WongWork::SUserPremiumInfo* info = premium->GetPremiumInfo(type);
    if (info->m_type != 0)
        list.push_back(*info);

    int curSec = GlobalData::s_systemTime_.getCurSec();
    int nextSec = curSec + 1;
    unsigned int serverCode = 0;
    for (size_t i = 0; i < list.size(); ++i)
    {
        const WongWork::SUserPremiumInfo& e = list[i];
        serverCode = 0;
        // m_useTargetServer(+0x5c)==1 → serverCode = m_serverEnvir.m_serverGroup
        // （CEnvironment+0x378）
        if (g_SPremiumInfo[e.m_type].m_useTargetServer == 1)
        {
            serverCode = (unsigned int)G_CEnvironment()->m_serverEnvir.m_serverGroup;
        }
        Stream* stream = GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x20e7);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<((int)0x54);   // 0x54 头
        guard.operator*()->operator<<((int)user->GetUID());
        guard.operator*()->operator<<((int)user->get_acc_id());
        guard.operator*()->operator<<((int)e.m_type);
        guard.operator*()->operator<<((int)curSec);
        guard.operator*()->operator<<((int)nextSec);
        guard.operator*()->operator<<((int)e.m_flag);
        guard.operator*()->operator<<((int)serverCode);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        WongWork::CHandlePremium::handleSetUserPremium(
            user, (ENUM_PREMIUM_TYPE)e.m_type, curSec, nextSec, e.m_flag, false);
        WongWork::CHandlePremium::handlePremiumEffect(user, (ENUM_PREMIUM_TYPE)e.m_type);
        WongWork::CHandlePremium::handleNotifyPremiumInfo(user, e.m_type, 0);
    }
}

// ============================================================================
// _onBuySpecialItem —— ORIG 0x08327454 (T)
// itemIdx ∈ 特殊集（0x3f..0x49 除 0x44，掩码 0x7df）时，投递
// SIG_SAVE_CRAZY_LEVELUP_EVENT 流（"0707" 头 + accId + characNo(-1)）至 LOGDB_Q；
// 再按 Is2010ChuseokPacket(itemIdx) 分支调 DB_UpdateChuseokPackagePurchase。
// ============================================================================
void CCeraShop::_onBuySpecialItem(CUser* user, ENUM_CERA_SPECIALITEM,
                                  unsigned long itemIdx)
{
    // ORIG: (param_4 - 0x3f < 0xb) && ((1 << ((param_4 - 0x3f) & 0x1f)) & 0x7df) != 0
    unsigned int k = (unsigned int)(itemIdx - 0x3f);
    if (k < 0xb && ((1u << (k & 0x1f)) & 0x7dfu) != 0)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x1971);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<((int)0xb4);
        guard.operator*()->operator<<(-1);
        SIG_SAVE_CRAZY_LEVELUP_EVENT* buf =
            guard->GetInBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT>();
        std::memcpy(buf, "0707", 5);  // DAT_08c25e65 = "0707\0"
        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(buf) + 5) =
            user->get_acc_id();
        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(buf) + 9) =
            user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    if (Is2010ChuseokPacket(itemIdx))
        DB_UpdateChuseokPackagePurchase::makeRequest(user->get_acc_id());
}

// ============================================================================
// _processBoosterSelect —— ORIG 0x08327894 (T)
// 在 booster 表（m_booster.m_map，item+0x24c，key=pair<char,char>(c1,c2)）中定位，
// 收集其 value 中与 items 现有 Inven_Item（m_addInfo==m_itemIdx）匹配的子物品，
// 清空 items 后逐项 _processAdditionalItem；随后限制商品校验 + SaveInventory。
// 返回：find==end→0；item==0 或处理完成→1。
// ============================================================================
int CCeraShop::_processBoosterSelect(CUser* user, int itemIdx, int count, int& result,
                                     long curSec, bool flag,
                                     std::vector<Inven_Item>& items,
                                     stCeraShopItemParam_t& param, char c1, char c2)
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return 1;

    std::vector<stStackableBoosterItemInfo_t> boosterList;
    CStackableItem* stackable = static_cast<CStackableItem*>(item);
    std::map<std::pair<char, char>,
             std::vector<stStackableBoosterItemInfo_t> >& bmap =
        stackable->m_booster.m_map;

    std::map<std::pair<char, char>,
             std::vector<stStackableBoosterItemInfo_t> >::iterator it =
        bmap.find(std::make_pair(c1, c2));
    if (it == bmap.end())
        return 0;

    std::vector<stStackableBoosterItemInfo_t>& src = it->second;
    for (std::vector<Inven_Item>::size_type i = 0; i < items.size(); ++i)
    {
        for (std::vector<stStackableBoosterItemInfo_t>::size_type j = 0;
             j < src.size(); ++j)
        {
            if (src[j].m_itemIdx == items[i].m_addInfo)
                boosterList.push_back(src[j]);
        }
    }

    items.clear();

    for (std::vector<stStackableBoosterItemInfo_t>::iterator b = boosterList.begin();
         b != boosterList.end(); ++b)
    {
        // ORIG 实参顺序：itemIdx(+0), addInfo(+0x8), avatarLevel(+0xe), avatarType(+0xd), items, param, +0xa(flag)
        _processAdditionalItem(user, (unsigned long)b->m_itemIdx, b->m_addInfo,
                               b->m_avatarLevel, b->m_avatarType, items, param,
                               b->m_byteA);
    }

    int restrictCode = G_CDataManager()->getItemList()->GetRestrictCode(itemIdx);
    if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
    {
        user->SetRestrictedGoods(restrictCode);
        SaveFeaturedIdx(user, restrictCode, (int)curSec, 0);
    }
    user->SaveInventory();
    return 1;
}

// ============================================================================
// _processAdditionalItem —— ORIG 0x083267a2 (T)
// itemIdx==1（金币）→ _processCoin + 推入 items；否则按物品类型分派：
//   可叠特殊物品（type 0x10/0x22）→ insertItemIntoInventory/邮件兜底；
//   头像 → AddAvatarItem/邮件兜底 + genIPGNo + findString + SendAradEventItemLog；
//   非头像非造物 → _processItem（0xc1 时嵌套发放 0x10）；造物 → _processCreature。
//   循环前 autoUnsealItem；每轮按可叠/包（_isPackageItem）判断 push_back。
// ============================================================================
void CCeraShop::_processAdditionalItem(CUser* user, unsigned long itemIdx,
                                       unsigned short slot, unsigned char a,
                                       unsigned char b, std::vector<Inven_Item>& items,
                                       stCeraShopItemParam_t& param, unsigned char flag)
{
    if (user->getCurCharacR() == 0)
        return;

    if (itemIdx == 1)
    {
        _processCoin(user, slot, false);
        Inven_Item coin;
        coin.m_addInfo = 1;
        coin.m_addInfo2 = (int)slot;
        items.push_back(coin);
        return;
    }

    // ORIG 栈上 name 缓冲（0x200）与 error 分支局部（local_58 恒为 -1，走 find 路径）。
    char nameBuf[0x200];
    std::memset(nameBuf, 0, sizeof(nameBuf));

    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    // is_stackable && GetItemType() ∈ {0x10, 0x22}
    bool specialStack = false;
    if (item->is_stackable())
    {
        int t = item->GetItemType();
        if (t == 0x10 || t == 0x22)
            specialStack = true;
    }

    if (specialStack)
    {
        Inven_Item invItem;
        invItem.m_addInfo = (int)itemIdx;
        invItem.m_addInfo2 = (int)slot;
        item->make_item(invItem);
        CInventory* inv = user->getCurCharacInvenW();
        int resSlot = inv->insertItemIntoInventory(
            invItem, (eItemAddReason)0xf, true, false);
        if (resSlot != -1)
            user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, resSlot);
        else
        {
            unsigned int characNo = (unsigned int)user->getCurCharacNo();
            CMailBoxHelper::ReqDBSendNewMailCashShop(
                user, invItem, 0, characNo, true, 0, 0);
        }
        items.push_back(invItem);
        return;
    }

    // 一般路径
    unsigned int count = slot;
    if (item->is_stackable())
        count = 1;

    char agency[0x18];
    std::memset(agency, 0, sizeof(agency));
    if (item->IsAvatarItem())
    {
        unsigned int characNo = (unsigned int)user->getCurCharacNo();
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)3, characNo, agency);
    }

    Inven_Item invItem;
    invItem.m_addInfo = (int)itemIdx;
    invItem.m_addInfo2 = (int)slot;
    item->make_item(invItem);

    if (flag != 0)
        invItem.SetUpgrade(flag);
    if (itemIdx == 0x71db || itemIdx == 0x72d0 || itemIdx == 0x742d ||
        itemIdx == 0x7556 || itemIdx == 0x7661)
        invItem.SetUpgrade(0xc);

    autoUnsealItem(item, invItem);

    int local_7c = 0;  // ORIG +0x7c 累积出参（_processItem/_processCreature 使用）

    for (unsigned int i = 0; i < count; ++i)
    {
        if (item->IsAvatarItem())
        {
            int avatarPeriod = ((CEquipItem*)item)->getAvatarPeriod(b);
            CInventory* inv = user->getCurCharacInvenW();
            int avSlot = inv->AddAvatarItem(
                (int)itemIdx, avatarPeriod, false, (char)a, (char)b, agency,
                eAvatarItemAddReason_0, false, 0);
            if (avSlot >= 0)
                user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1, avSlot);
            else
            {
                int expireDate = 0;
                if (avatarPeriod != 0 && avatarPeriod != 0x16d)
                {
                    expireDate = GlobalData::s_systemTime_.getCurSec();
                    expireDate += avatarPeriod * 0x15180;
                }
                const char* msg = g_scriptStringManager_.findString(
                    4, "game_server_msg_104", (bool*)0);
                std::size_t msgLen = msg ? std::strlen(msg) : 0;
                int characNo = user->getCurCharacNo();
                CMailBoxHelper::ReqDBSendNewAvatarMail(
                    user, characNo, (int)itemIdx, expireDate, (char)a, (char)b,
                    agency, msg, (int)msgLen);
            }
            user->SendAradEventItemLog((unsigned long)itemIdx, 1);
        }
        else if (item->IsCreatureItem())
        {
            int creatureSlot = 0;  // ORIG local_94 出参（本调用链内死存储）
            _processCreature(user, invItem, creatureSlot, local_7c, false,
                             item->getUsablePeriod(), item->getExpirationDate());
        }
        else
        {
            long curSec = GlobalData::s_systemTime_.getCurSec();
            _processItem(user, invItem, 0, local_7c, curSec, true, false, items,
                         param);
            // ORIG 嵌套：0xc1 商品追加发放固定物品 0x10（count=0xc352，save=true）。
            if (itemIdx == 0xc1)
            {
                CItem* found = G_CDataManager()->find_item((int)itemIdx);
                CItem* item10 = G_CDataManager()->find_item(0x10);  // ORIG 无空判
                if (found != 0)
                {
                    std::vector<Inven_Item> nestedItems;
                    stCeraShopItemParam_t nestedParam;
                    int nestedRestrict = 0;
                    Inven_Item nestedItem;
                    item10->make_item(nestedItem);
                    nestedItem.m_addInfo = 0x10;
                    nestedItem.m_addInfo2 = 1;
                    _processItem(user, nestedItem, 0xc352, nestedRestrict, curSec,
                                 false, true, nestedItems, nestedParam);
                }
            }
        }
        if (!item->is_stackable())
        {
            items.push_back(invItem);
        }
        else
        {
            if (!_isPackageItem(item))
                items.push_back(invItem);
        }
    }
}

}  // namespace WongWork

// ---- DB_SaveFeaturedGoodsIdx::makeRequest —— ORIG 0x0841b2da (T) ----
// 报告 docs/class_func_reports/DB_SaveFeaturedGoodsIdx/makeRequest.md 全文已读：
// <<0x52<<-1<<param_1(accId)<<param_2(characNo)<<param_3(restrictCode)
//   <<param_5(d)<<param_4(curSec) → put(DB_Q(2))。
void DB_SaveFeaturedGoodsIdx::makeRequest(unsigned int accId, int characNo,
                                          int restrictCode, int curSec, int d)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4dfb);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<((int)0x52);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)accId);
    guard.operator*()->operator<<((int)characNo);
    guard.operator*()->operator<<((int)restrictCode);
    guard.operator*()->operator<<((int)d);
    guard.operator*()->operator<<((int)curSec);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
