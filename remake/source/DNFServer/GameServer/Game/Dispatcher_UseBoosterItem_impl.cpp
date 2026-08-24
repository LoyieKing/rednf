#include "Dispatchers.h"

// ============================================================================
// Dispatcher_UseBoosterItem 还原（ORIG）
//   dispatch_sig                 0x08208d40 T（762 insn）
//   _onBoosterItemUseResult      0x082099bc T
//   _onBoosterItemUseResult_OnlyOne 0x08209a5e T
//   sendBroadCastItems           0x08209b0e T
// 实现以 docs/class_func_reports/Dispatcher_UseBoosterItem/*.md 为准。
// 本类仅 dispatch_sig/process/read 在 Dispatchers.h 声明；_onBoosterItemUseResult*
// 与 sendBroadCastItems 未在头声明（原 Dispatchers.cpp 桩已删），按 Main 指示以此
// 文件内的文件级静态辅助函数复现（行为与 ORIG 一致；不新增导出符号）。
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "Inven_Item.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventManager / CEventBase（GetRepeatEvent / IsEventing）
#include "CEnvironment.h"       // GlobalInstance<Arad_GiftItem_Set>
#include "CPrivateStoreMgr.h"   // private_store::GetInstancePrivateStoreMgr / IsBusyPrivateStore
#include "CItemList.h"           // Arad_GiftItem_Set::level_check
#include "CCeraShop.h"           // WongWork::CCeraShop::ProcessSpecialItem / SaveFeaturedIdx

#include <vector>
#include <map>
#include <utility>
#include <string>

// GrowthEquipEvent 权威头（ORIG 0x080f74b2 族）：IsEventing W 0x080f7820 /
// SelectGrowthWeapon T 0x080f73be，实现见 GrowthEquipEvent.cpp。
#include "GrowthEquipEvent.h"

// ============================================================================
// 本地静态辅助：_onBoosterItemUseResult / _onBoosterItemUseResult_OnlyOne /
// sendBroadCastItems（对应本类三个辅助方法，行为忠实 ORIG；未写成类成员系因
// Dispatchers.h 无这些声明且不可改。）
// ============================================================================

// sendBroadCastItems(CUser*, Inven_Item const&, int) —— ORIG 0x08209b0e
static void helper_sendBroadCastItems(CUser* user, const Inven_Item& item, int itemId)
{
    if (user == 0)
        return;

    char flag = 0;
    CDataManager* dm = G_CDataManager();
    stBroadCastItemScript& script = dm->m_broadCastItem;

    if (script.isUnCommonItem(itemId))          // isUnCommonItem(itemId)
        flag = script.checkUnCommonNestedItem(itemId, item.m_addInfo);
    else
        flag = script.checkCommonItem(item.m_addInfo);

    if (flag)
    {
        PacketGuard pg;
        pg.put_header(0, 0x56);
        pg.put_byte(2);
        pg.put_byte(1);
        pg.put_short((unsigned short)(user->get_unique_id()));   // put_short(uid & 0xffff)
        pg.put_int(item.m_addInfo);
        pg.put_byte(item.GetUpgrade());
        pg.finalize(true);
        G_GameWorld()->send_all(pg);
    }
}

// _onBoosterItemUseResult(CUser*, vector<Inven_Item>&, int) —— ORIG 0x082099bc
static void helper_onBoosterItemUseResult(CUser* user, std::vector<Inven_Item>& items, int itemId)
{
    for (std::vector<Inven_Item>::const_iterator it = items.begin(); it != items.end(); ++it)
        helper_sendBroadCastItems(user, *it, itemId);
}

// _onBoosterItemUseResult_OnlyOne(CUser*, vector<Inven_Item>&, int) —— ORIG 0x08209a5e
static void helper_onBoosterItemUseResult_OnlyOne(CUser* user, std::vector<Inven_Item>& items, int itemId)
{
    if (items.size() != 1)
        return;

    CEventBase* evt0 = GlobalData::s_event_manager->GetRepeatEvent(100);
    CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(100);

    // ORIG：经 vptr+0x34（GrowthEquipEvent::IsEventing）判活，实参为 0。
    GrowthEquipEvent* g = static_cast<GrowthEquipEvent*>(evt0);
    if (g != 0 && g->IsEventing(0))
    {
        if (evt != 0)
        {
            const Inven_Item& first = items.front();
            // ORIG 0x8209ae8：arg2 = Inven_Item+0x02（m_addInfo，道具 idx），
            // arg3 = 本函数第 4 参（包内 addInfo）。
            static_cast<GrowthEquipEvent*>(evt)->SelectGrowthWeapon(*user, first.m_addInfo, itemId);
        }
    }
}

// ============================================================================
// dispatch_sig —— ORIG 0x08208d40
// ============================================================================
int Dispatcher_UseBoosterItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    short slot = -1;
    if (!packet.get_short(slot))
        return 0;   // LineFunc(0xa436)

    // 交易/私人商店中禁止使用
    if (user->CheckInTrade() ||
        (private_store::GetInstancePrivateStoreMgr() != 0 &&
         private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user)))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
        return 0;
    }

    if (user->getCurCharacR() == 0)
        return 0xa43e;

    int itemId = 0;
    if (const CInventory* inv = user->getCurCharacInvenR())
    {
        Inven_Item item = inv->GetInvenSlot(INVEN_TYPE_INVEN, (int)slot);
        itemId = item.m_addInfo;
    }

    CDataManager* dm = G_CDataManager();
    CStackableItem* stackable = static_cast<CStackableItem*>(dm->find_item(itemId));

    // 物品种类必须为可叠放(stackable 0x10)或选择强化罐(0x22)
    int itemType = 0;
    if (stackable != 0)
        itemType = stackable->GetItemType();
    if (stackable == 0 || (itemType != 0x10 && itemType != 0x22))
        return 0;   // LineFunc(0xa450)

    // 限制条件：罐子类型或 state==3 才允许
    if (itemId != 0x28d3c1 && user->get_state() != 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
        return 0;
    }

    // 转职校验
    char grow = user->getCurCharFirstGrowType();
    char job  = (char)user->get_charac_job();
    if (!stackable->check_change_job_type(grow, job))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x17);
        return 0;
    }

    std::vector<Inven_Item> resultItems;
    char c1 = 0;
    char c2 = 0;

    if (itemType == 0x22)
    {
        // ---- 选择强化罐：先读两个字节（类别/类型），再在 Booster 表中定位 ----
        if (!packet.get_byte(c1))
            return 0;   // LineFunc(0xa473)
        if (!packet.get_byte(c2))
            return 0;   // LineFunc(0xa475)

        // 在 stStackableBooster_t::m_map（+0x24c）里按 (c1,c2) 取值
        std::map<std::pair<char, char>,
                 std::vector<stStackableBoosterItemInfo_t> >& bmap =
            stackable->m_booster.m_map;

        int count = stackable->m_boosterSelect.m_field8;   // +0x26c
        if (count == 0)
        {
            std::map<std::pair<char, char>,
                     std::vector<stStackableBoosterItemInfo_t> >::iterator it =
                bmap.find(std::make_pair(c1, c2));
            if (it != bmap.end() && it->second.empty())
            {
                // cMyTrace(...) "selectionBooster empty : %d, category:%d,%d\n"
                return 0;   // LineFunc(0xa481)
            }
            if (it != bmap.end())
                count = (int)it->second.size();
        }

        int i = 0;
        while (i < count && i < 0x14)
        {
            int itemIdx = 0;
            if (!packet.get_int(itemIdx))
                break;      // LineFunc(0xa48c)

            Inven_Item add;
            add.reset();
            add.m_addInfo  = itemIdx;
            add.m_addInfo2 = 1;
            resultItems.push_back(add);
            ++i;
        }

        user->m_historyLog.UseSelectBooster((unsigned int)itemId, (int)c1, (int)c2, resultItems);
    }

    // ---- 主路径（两种类型汇合于此）----
    {
        Arad_GiftItem_Set* gift = GlobalInstance<Arad_GiftItem_Set>::inst();
        if (gift->level_check(user, stackable))   // 物品等级校验
        {
            unsigned int prot =
                GlobalData::s_pSecuProtectionField->CheckItem(user, (SECURITY_PROTCTION)0x2a, stackable);
            if (prot != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, (unsigned char)(prot & 0xff));
                return 0;
            }

            if (user->get_charac_level() < stackable->getUsableLevel())
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
                return 0;
            }

            CInventory* invW = user->getCurCharacInvenW();

            if (!invW->check_empty_count(Inven_Item::ITEM_TYPE_CREATURE_EQUIP, 1))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0xcb);
                return 0;
            }

            if (user->CheckCoolTimeItem((unsigned long)itemId))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
                return 0;
            }

            if (stackable->IsDeletableItem())
            {
                // 删除物品（INVEN_TYPE_INVEN=1? ORIG 压栈 inven=&invW, 1, slot, 1, reason3, 1）
                // ORIG CInventory::delete_item(inv, 1, slot, 1, eItemDelReason_3, 1)
                if (!invW->delete_item(INVEN_TYPE_INVEN, (int)slot, 1,
                                       eItemDelReason_3, true))
                    return 0;   // LineFunc(0xa4d3)
            }
            user->_processContinuousItem((unsigned int)itemId, stackable);
            user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, (int)slot);

            int nItemIdx  = 0;      // local_2c（作 count 传入，调用后 = 限制道具 index）
            int nProcess  = 0;      // local_78（ProcessSpecialItem 的 restrictCode& 输出）
            long curSec   = GlobalData::s_systemTime_.getCurSec();

            WongWork::CCeraShop::stCeraShopItemParam_t ceraParam;
            WongWork::CCeraShop* cera = GlobalData::s_pCeraShop;
            bool rt = cera->ProcessSpecialItem(
                user, itemId, nItemIdx, nProcess, curSec, true,
                resultItems, ceraParam, c1, c2, false, 1);

            if (!rt)
            {
                return 0;   // LineFunc(0xa4f6)
            }
            else if (nProcess == 0)
            {
                if (nItemIdx == 0 || user->IsRestrictedGoods(nItemIdx))
                {
                    // ok
                }
                else
                {
                    user->SetRestrictedGoods(nItemIdx);
                    cera->SaveFeaturedIdx(user, nItemIdx, (int)curSec, 0);
                    user->SendRestrictedGoods(true, nItemIdx);
                }


                helper_onBoosterItemUseResult(user, resultItems, itemId);
                helper_onBoosterItemUseResult_OnlyOne(user, resultItems, itemId);

                if (itemId == 0x28d1b3)
                    user->SendEventJarOutputItemStatistics(resultItems);

                // ---- 结果包（header 0xa3）----
                CInventory* invR = const_cast<CInventory*>(user->getCurCharacInvenR());
                Inven_Item slotItem;
                if (invR != 0)
                    slotItem = invR->GetInvenSlot(INVEN_TYPE_INVEN, (int)slot);
                else
                    slotItem.reset();

                PacketGuard pg;
                pg.put_header(1, 0xa3);
                pg.put_byte(1);
                pg.put_short((int)slot);
                pg.put_int(slotItem.m_addInfo);
                pg.put_short((int)resultItems.size());
                for (std::vector<Inven_Item>::const_iterator it = resultItems.begin();
                     it != resultItems.end(); ++it)
                {
                    pg.put_int(it->m_addInfo);
                    pg.put_int(it->m_addInfo2);
                }
                pg.finalize(true);
                user->Send(pg);
            }
        }
        else
        {
            // 物品等级校验失败（cMyTrace "stackable item level check charac_no[%d], item_id(%d)"）
            user->get_charac_no(-1);
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
            return 0;
        }
    }

    return 0;
}

int Dispatcher_UseBoosterItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { (void)user; (void)msg; (void)param; return 0; }
int Dispatcher_UseBoosterItem::read(PacketBuf& packet, MSG_BASE& msg) { (void)packet; (void)msg; return 0; }