// ============================================================================
// CUserStatC.cpp —— CUser 物品域三符号（Prod_CUserC 批次）
//
//   CUser::delete_item                 ORIG T 0x0866285c _ZN5CUser11delete_itemEcimi14eItemDelReason
//   CUser::exchange_interspace         ORIG T 0x0865f7b0 _ZN5CUser19exchange_interspaceEiimiiimPi
//   CUser::exchange_interspace_expand  ORIG T 0x0865f614 _ZN5CUser26exchange_interspace_expandEiimiiimPicc
//
// 依据 docs/class_func_reports/CUser/{delete_item,exchange_interspace,
// exchange_interspace_expand}.md 的 ## 汇编 + ## 反编译 C 全文逐分支转写；
// 每个被调地址经 df_game_r nm 解析为真实符号后映射到权威头声明：
//   CheckInTrade/getCurCharacInvenR/W/getCurCharacCargoR/W/get_acc_id
//   GetInvenTypeFromItemSpace/Inven_Item::{C2,set_add_info,get_add_info}
//   IsExistAccountCargo/GetAccountCargo/CTradeSpace::{is_empty,IsLocked}
//   getCurChannelEquipslotSwitch/getCurExpandEquipslotSwitch/G_GameWorld
//   GameWorld::IsCheckUnusableEquipmentChannel/CAvatarItemMgr::IsTempKey(int)
//   change_equip/change_equip_expand/_checkItemMovable(声明先行，定义后续批次)
//   cUserHistoryLog::{ItemAddCargo,AccountCargoAddItem,AccountCargoDelItem}
//   online_preliminary::COnlinePreliminary::CheckUnusableEquipment(T 0x08560920)
// 虚调用 vtable+0x40 经 ORIG vtable(CStackableItem@0x08c96ea0) 槽位转储核实
//   = CItem::IsImpossibleCharacCargo() const。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CAvatarItemMgr.h"
#include "CTradeSpace.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "CParty.h"
#include "ServiceRestrictManager.h"
#include "GlobalData.h"
#include "RDARScriptStringManager.h"
#include "PacketGuard.h"

namespace
{

// CCargo 域通用：check_slot_empty(slot)!=1 时改取 get_empty_slot()；负值失败。
inline bool cargoResolveSlot(const CCargo* cargo, int& slot)
{
    if (cargo->check_slot_empty(slot) == 1)
        return true;
    slot = cargo->get_empty_slot();
    return slot >= 0;  // 失败 → 调用方 return 4
}

// CAccountCargo 域同形检查。
inline bool accountCargoResolveSlot(CAccountCargo* cargo, int& slot)
{
    if (cargo->CheckSlotEmpty(slot) == 1)
        return true;
    slot = cargo->GetEmptySlot();
    return slot >= 0;  // 失败 → 调用方 return 4
}

}  // namespace

// ============================================================================
// CUser::delete_item(char space, int position, unsigned long itemIdx,
//                    int count, eItemDelReason reason)
// ORIG 0x0866285c sz 0x25d。返回：0 成功 / 0x11 校验或删除失败 / 0x13 交易中。
// ============================================================================
int CUser::delete_item(char itemSpace, int position, unsigned long itemIdx,
                       int count, eItemDelReason reason)
{
    if (CheckInTrade())
        return 0x13;

    // GetInvenSlot(out, curCharacInvenR(), GetInvenTypeFromItemSpace(space), position)
    Inven_Item slotItem = getCurCharacInvenR()->GetInvenSlot(
        (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)itemSpace), position);
    if ((unsigned long)slotItem.m_addInfo != itemIdx)
        return 0x11;

    // signed char 符号扩展后按无符号 >7 判越界（含负值）→ 直接成功返回。
    if ((unsigned int)(signed char)itemSpace > 7u)
        return 0;

    switch (itemSpace)
    {
    case 0:  // INVEN_TYPE_EQUIP(1) 背包
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, position, count,
                                               reason, true))
            return 0x11;
        break;
    case 1:  // INVEN_TYPE_AVATAR(2)
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_AVATAR, position, count,
                                               reason, true))
            return 0x11;
        break;
    case 2:  // 角色仓库
        if (!getCurCharacCargoW()->delete_item(position, count, reason))
            return 0x11;
        break;
    case 3:  // INVEN_TYPE_INVEN(0) 装备栏 → 需回发装备
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_INVEN, position, count,
                                               reason, true))
            return 0x11;
        send_equip(position);
        break;
    case 7:  // INVEN_TYPE_CREATURE(3)：宠物装备且类型!=5(ITEM_TYPE_CREATURE_EQUIP)
    {
        const Inven_Item* ref =
            getCurCharacInvenW()->GetInvenRef(INVEN_TYPE_CREATURE, position);
        if (ref == 0)
            return 0x11;
        if (ref->m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
            return 0x11;
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_CREATURE, position,
                                               count, reason, true))
            return 0x11;
        break;
    }
    default:  // 4/5/6：仅通过索引校验，无动作即成功
        break;
    }
    return 0;
}

// ============================================================================
// CUser::exchange_interspace(...)
// ORIG 0x0865f7b0 sz 0x2d47。参数（报告语义）：fromSpace/fromSlot/fromItemIdx/
//   addInfo(数量或强化附加)/toSpace/toSlot/toItemIdx/outSlot。
// 返回：0 成功 / 1 参数域不支持 / 4 目标槽不可用 / 0x11 通用失败 /
//       0x13 交易中 / 0x15 movable 校验失败或交易表空 / 0x17 物品限制 /
//       0x5e regist_item==-4 / 0xd1 服务限制。
// ============================================================================
int CUser::exchange_interspace(int fromSpace, int fromSlot, unsigned long fromItemIdx,
                               int addInfo, int toSpace, int toSlot,
                               unsigned long toItemIdx, int* outSlot)
{
    PacketGuard packetGuard;      // ORIG 栈上 RAII 局部（无其它使用）
    Inven_Item item;              // 主工作副本（ORIG -0x131）

    if (!_checkItemMovable(this, (ENUM_ITEMSPACE)fromSpace, fromSlot, fromItemIdx,
                           (ENUM_ITEMSPACE)toSpace, toSlot, toItemIdx))
    {
        *outSlot = -1;
        return 0x15;
    }

    switch (fromSpace)
    {
    // ---------------------------------------------------------------- 背包源
    case 0:
        switch (toSpace)
        {
        case 0:  // 背包→背包
        {
            int ret = getCurCharacInvenW()->move_item(INVEN_TYPE_EQUIP, fromSlot,
                                                      INVEN_TYPE_EQUIP, toSlot);
            if (ret == 0)
            {
                *outSlot = toSlot;
                return 0;
            }
            return ret;
        }
        case 2:  // 背包→角色仓库
        {
            if (CheckInTrade())
                return 0x13;

            item = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, fromSlot);

            CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
            if (pItem == 0)
                return 0x17;
            if (pItem->IsImpossibleCharacCargo())  // ORIG vtable+0x40
                return 0x17;
            if (item.m_addInfo > 0x1963 && item.m_addInfo < 7000)
                return 0x17;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!cargoResolveSlot(getCurCharacCargoR(), toSlot))
                    return 4;
            }
            else
            {
                if (item.m_field1 == 4)
                    return 0x17;
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int exist = getCurCharacCargoR()->check_item_exist(item.m_addInfo);
                if (exist >= 0)
                    toSlot = exist;
                else if (!cargoResolveSlot(getCurCharacCargoR(), toSlot))
                    return 4;
            }

            toSlot = getCurCharacCargoR()->tryInsertItemSpecialSlot(item, toSlot);
            if (toSlot < 0)
                return 4;

            bool delOk = getCurCharacInvenW()->delete_item(
                INVEN_TYPE_EQUIP, fromSlot, addInfo, (eItemDelReason)7, true);
            if (!delOk)
            {
                cMyTrace trace("int CUser::exchange_interspace(int, int, "
                               "itemIndexOnlyServer_t, int, int, int, "
                               "itemIndexOnlyServer_t, int*)",
                               0x3b21, 5);
                trace("delete_item failed %d %d %d", getCurCharacNo(), fromSlot,
                      addInfo);
                break;  // ORIG：删除失败仍返回 0
            }
            *outSlot = getCurCharacCargoW()->insert_item_special_slot(item, toSlot);

            const Inven_Item slotIt = getCurCharacCargoR()->get_cargo_slot(*outSlot);
            if (slotIt.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                m_historyLog.ItemAddCargo(item.m_addInfo, addInfo, 1);
            else
                m_historyLog.ItemAddCargo(item.m_addInfo, addInfo,
                                          slotIt.get_add_info());
            break;
        }
        case 3:  // 背包→装备栏
        {
            if (!CheckInTrade())
            {
                int ret = change_equip(fromSpace, (short)fromSlot, toSlot);
                if (ret == 0)
                {
                    *outSlot = toSlot;
                    void* party = GetParty();
                    if (party != 0)
                        ((CParty*)party)->Reset_party_overlapped_drop_ratio();
                    return 0;
                }
                return ret;
            }
            return 0x13;
        }
        case 4:  // 背包→交易窗口
        {
            CTradeSpace* trade = G_CGameManager()->GetTradeSpace((int)m_field8d002);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_field8d002 = -1;
                return 0x15;
            }
            if (trade->check_trade_possibility() == 1)
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 0, fromSlot, addInfo);
                if (ret < 0)
                {
                    int characNo = getCurCharacNo();
                    char* accStr = NumberToString(get_acc_id(), 0);
                    cMyTrace trace("int CUser::exchange_interspace(int, int, "
                                   "itemIndexOnlyServer_t, int, int, int, "
                                   "itemIndexOnlyServer_t, int*)",
                                   0x3b57, 5);
                    trace("(regist_item(): %d)(m_id: %s) (CharacNo: %d) "
                          "(from_slot: %d) (item_count: %d)",
                          ret, accStr, characNo, fromSlot, addInfo);
                    if (ret == -4)
                        return 0x5e;
                    return 0x11;
                }
                *outSlot = ret;
            }
            else
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            break;
        }
        case 0xc:  // 背包→账号金库
        {
            if (CheckInTrade())
                return 0x13;
            if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                    this, (RestrictCategory::Enum)1, 0x1b))
                return 0xd1;
            int secuRet =
                GlobalData::s_pSecuProtectionField->Check(this,
                                                          (SECURITY_PROTCTION)1);
            if (secuRet != 0)
                return secuRet;
            if (!IsExistAccountCargo())
                return 0x13;

            CAccountCargo* cargo = GetAccountCargo();
            item = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, fromSlot);

            if (IsTradeLimitAttachTypeItem(item))
                return 0x17;
            if (cargo->CheckInsertCondition(item) != 1)
                return 0x11;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!accountCargoResolveSlot(cargo, toSlot))
                    return 4;
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int specific = cargo->GetSpecificItemSlot(item.m_addInfo);
                if (specific >= 0)
                {
                    if (cargo->CheckStackLimit(specific, item.m_addInfo, addInfo)
                        != 1)
                        return 4;
                    toSlot = specific;
                }
                else if (!accountCargoResolveSlot(cargo, toSlot))
                {
                    return 4;
                }
            }

            bool delOk = getCurCharacInvenW()->delete_item(
                INVEN_TYPE_EQUIP, fromSlot, addInfo, (eItemDelReason)0x25, true);
            if (delOk)
            {
                *outSlot = cargo->InsertItem(item, toSlot);
                const Inven_Item slotIt = cargo->GetSlot(*outSlot);
                if (slotIt.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.AccountCargoAddItem(slotIt.m_addInfo, 1, 1,
                                                     (eItemAddReason)0x2d);
                else
                    m_historyLog.AccountCargoAddItem(slotIt.m_addInfo,
                                                     slotIt.m_addInfo2, addInfo,
                                                     (eItemAddReason)0x2d);
            }
            break;
        }
        default:
            return 1;
        }
        break;

    // --------------------------------------------------------------- 时装源
    case 1:
    {
        // ORIG 在分支入口构造两个临时项并按目的域选择性填充。
        Inven_Item itemA;
        Inven_Item itemB;  // toSpace==4 时仅保持构造初值（ORIG 同）
        if (toSpace == 3 || toSpace == 1)
        {
            itemA = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, fromSlot);
            // ORIG：toSpace==3 读装备槽(INVEN_TYPE 0)，toSpace==1 读时装槽(2)，均取目的槽位。
            itemB = getCurCharacInvenR()->GetInvenSlot(
                (INVEN_TYPE)(toSpace == 1 ? INVEN_TYPE_AVATAR : INVEN_TYPE_INVEN),
                toSlot);
        }
        else if (toSpace == 4)
        {
            itemA = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, fromSlot);
        }

        // ORIG 以 IsTempKey(m_addInfo2) 判临时键（实现不读 this，仅看符号位）。
        const WongWork::CAvatarItemMgr* avatarMgr =
            getCurCharacInvenR()->GetAvatarItemMgrR();
        if (avatarMgr->IsTempKey((int)itemA.m_addInfo2)
            || avatarMgr->IsTempKey((int)itemB.m_addInfo2))
            return 0x13;

        if (toSpace == 3)
        {
            int ret = change_equip(fromSpace, (short)fromSlot, toSlot);
            if (ret > 0)
                return ret;
            *outSlot = toSlot;
        }
        else if (toSpace == 4)
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_field8d002);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_field8d002 = -1;
                return 0x15;
            }
            if (trade->check_trade_possibility() == 1)
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 1, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
            }
            else
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
        }
        else if (toSpace == 1)
        {
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_AVATAR, fromSlot, INVEN_TYPE_AVATAR, toSlot);
            if (ret != 0)
                return ret;
            *outSlot = toSlot;
        }
        else
        {
            return 1;
        }
        break;
    }

    // ------------------------------------------------------------ 角色仓库源
    case 2:
        switch (toSpace)
        {
        case 0:  // 仓库→背包
        {
            if (CheckInTrade())
                return 0x13;

            item = getCurCharacCargoR()->get_cargo_slot(fromSlot);
            Inven_Item copy = item;
            if (copy.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
                copy.set_add_info(addInfo);

            if (getCurCharacInvenW()->tryInsertItemIntoInventory(copy) == -1)
                return 0x11;

            bool delOk =
                getCurCharacCargoW()->delete_item(fromSlot, addInfo,
                                                  (eItemDelReason)7);
            if (!delOk)
            {
                cMyTrace trace("int CUser::exchange_interspace(int, int, "
                               "itemIndexOnlyServer_t, int, int, int, "
                               "itemIndexOnlyServer_t, int*)",
                               0x3bea, 5);
                trace("delete_item failed %d %d %d", getCurCharacNo(),
                      item.m_addInfo, addInfo);
            }
            else
            {
                *outSlot = getCurCharacInvenW()->insertItemIntoInventory(
                    copy, (eItemAddReason)7, true, false);
                if (*outSlot == -1)
                {
                    char* accStr = NumberToString(get_acc_id(), 0);
                    cMyTrace trace("int CUser::exchange_interspace(int, int, "
                                   "itemIndexOnlyServer_t, int, int, int, "
                                   "itemIndexOnlyServer_t, int*)",
                                   0x3be5, 5);
                    trace("insertItemIntoInventory failed %s %d %d %d", accStr,
                          getCurCharacNo(), copy.m_addInfo, addInfo);
                }
            }
            return 0;
        }
        case 2:  // 仓库→仓库
            if (getCurCharacCargoW()->move_item(fromSlot, toSlot) == 1)
            {
                *outSlot = toSlot;
                return 0;
            }
            return 0x11;
        case 3:
            return CheckInTrade() ? 0x13 : 1;
        case 7:
            return 1;
        case 0xc:  // 仓库→账号金库
        {
            if (CheckInTrade())
                return 0x13;
            int secuRet =
                GlobalData::s_pSecuProtectionField->Check(this,
                                                          (SECURITY_PROTCTION)1);
            if (secuRet != 0)
                return secuRet;
            if (!IsExistAccountCargo())
                return 0x13;

            CAccountCargo* cargo = GetAccountCargo();
            item = getCurCharacCargoR()->get_cargo_slot(fromSlot);

            if (cargo->CheckInsertCondition(item) != 1)
                return 0x11;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!accountCargoResolveSlot(cargo, toSlot))
                    return 4;
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int specific = cargo->GetSpecificItemSlot(item.m_addInfo);
                if (specific >= 0)
                {
                    if (cargo->CheckStackLimit(specific, item.m_addInfo, addInfo)
                        != 1)
                        return 4;
                    toSlot = specific;
                }
                else if (!accountCargoResolveSlot(cargo, toSlot))
                {
                    return 4;
                }
            }

            bool delOk =
                getCurCharacCargoW()->delete_item(fromSlot, addInfo,
                                                  (eItemDelReason)0x25);
            if (delOk)
            {
                *outSlot = cargo->InsertItem(item, toSlot);
                const Inven_Item slotIt = cargo->GetSlot(*outSlot);
                if (slotIt.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.AccountCargoAddItem(slotIt.m_addInfo, 1, 1,
                                                     (eItemAddReason)0x2e);
                else
                    m_historyLog.AccountCargoAddItem(slotIt.m_addInfo,
                                                     slotIt.m_addInfo2, addInfo,
                                                     (eItemAddReason)0x2e);
            }
            return 0;
        }
        default:
            return 1;
        }

    // -------------------------------------------------------------- 装备栏源
    case 3:
        switch (toSpace)
        {
        case 0:  // 装备→背包
        {
            if (!CheckInTrade())
            {
                int ret = change_equip(toSpace, (short)toSlot, fromSlot);
                if (ret == 0)
                {
                    *outSlot = fromSlot;
                    return 0;
                }
                return ret;
            }
            return 0x13;
        }
        case 1:  // 装备→时装
        {
            Inven_Item equipSlot =
                getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_INVEN, fromSlot);
            Inven_Item avatarSlot =
                getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, toSlot);
            const WongWork::CAvatarItemMgr* avatarMgr =
                getCurCharacInvenR()->GetAvatarItemMgrR();
            if (avatarMgr->IsTempKey((int)equipSlot.m_addInfo2)
                || avatarMgr->IsTempKey((int)avatarSlot.m_addInfo2))
                return 0x13;
            int ret = change_equip(fromSpace, (short)fromSlot, toSlot);
            if (ret < 1)
            {
                *outSlot = toSlot;
                return 0;
            }
            return ret;
        }
        case 2:
            return CheckInTrade() ? 0x13 : 1;
        case 4:  // 装备→交易窗口
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_field8d002);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_field8d002 = -1;
                return 0x15;
            }
            if (trade->check_trade_possibility() == 1)
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 3, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
                send_equip(fromSlot);
            }
            else
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            return 0;
        }
        case 7:  // 装备→宠物
        {
            if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
            {
                const Inven_Item* ref = getCurCharacInvenR()->GetInvenRef(
                    INVEN_TYPE_INVEN, toSlot);
                if (!online_preliminary::COnlinePreliminary::
                        CheckUnusableEquipment(ref))
                {
                    SendNotiPacketMessage(
                        g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                          (bool*)0),
                        (ENUM_MESSAGE_TYPE)0);
                    return 1;
                }
            }
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_INVEN, fromSlot, INVEN_TYPE_CREATURE, toSlot);
            if (ret == 0)
            {
                *outSlot = toSlot;
                send_equip(toSlot);
                return 0;
            }
            return ret;
        }
        default:
            return 1;
        }

    // ------------------------------------------------------------ 交易窗口源
    case 4:
    {
        // 允许目的域位掩码 {0,1,3,7}
        if ((unsigned int)toSpace > 7u || ((1u << toSpace) & 0x8bu) == 0)
            return 1;

        CTradeSpace* trade = G_CGameManager()->GetTradeSpace((int)m_field8d002);
        if (trade == 0)
            return 1;
        if (trade->is_empty())
        {
            m_field8d002 = -1;
            return 0x15;
        }
        if (trade->check_trade_possibility() == 1)
        {
            if (trade->IsLocked())
                return 0x11;
            int ret = trade->remove_item(this, fromSlot, toSpace, toSlot, addInfo);
            if (ret < 0)
                return 0x11;
            *outSlot = ret;
            if (toSpace == 3)
                send_equip(toSlot);
        }
        else
        {
            trade->cancel_trade();
            G_CGameManager()->PutTradeSpace(trade);
        }
        break;
    }

    // -------------------------------------------------------------- 宠物源
    case 7:
        if (toSpace == 4)
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_field8d002);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_field8d002 = -1;
                return 0x15;
            }
            if (trade->check_trade_possibility() == 1)
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 7, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
            }
            else
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
        }
        else if (toSpace == 3)
        {
            if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
            {
                const Inven_Item* ref = getCurCharacInvenR()->GetInvenRef(
                    INVEN_TYPE_CREATURE, fromSlot);
                if (!online_preliminary::COnlinePreliminary::
                        CheckUnusableEquipment(ref))
                {
                    SendNotiPacketMessage(
                        g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                          (bool*)0),
                        (ENUM_MESSAGE_TYPE)0);
                    return 1;
                }
            }
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_CREATURE, fromSlot, INVEN_TYPE_INVEN, toSlot);
            if (ret == 0)
            {
                *outSlot = toSlot;
                send_equip(toSlot);
                return 0;
            }
            return ret;
        }
        else if (toSpace == 7)
        {
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_CREATURE, fromSlot, INVEN_TYPE_CREATURE, toSlot);
            if (ret != 0)
                return ret;
            *outSlot = toSlot;
        }
        else
        {
            return 1;
        }
        break;

    // ----------------------------------------------------------- 账号金库源
    case 0xc:
    {
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                this, (RestrictCategory::Enum)1, 0x1b))
            return 0xd1;
        int secuRet =
            GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)1);
        if (secuRet != 0)
            return secuRet;

        switch (toSpace)
        {
        case 0:  // 金库→背包
        {
            if (CheckInTrade())
                return 0x13;
            if (!IsExistAccountCargo())
                return 0x13;
            CAccountCargo* cargo = GetAccountCargo();

            item = cargo->GetSlot(fromSlot);
            Inven_Item copy = item;
            if (copy.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
                copy.set_add_info(addInfo);

            if (getCurCharacInvenW()->tryInsertItemIntoInventory(copy) == -1)
                return 0x11;

            bool delOk = cargo->DeleteItem(fromSlot, addInfo);
            if (delOk)
            {
                *outSlot = getCurCharacInvenW()->insertItemIntoInventory(
                    copy, (eItemAddReason)7, true, false);
                if (*outSlot == -1)
                {
                    char* accStr = NumberToString(get_acc_id(), 0);
                    cMyTrace trace("int CUser::exchange_interspace(int, int, "
                                   "itemIndexOnlyServer_t, int, int, int, "
                                   "itemIndexOnlyServer_t, int*)",
                                   0x3ed3, 5);
                    trace("insertItemIntoInventory failed %s %d %d %d", accStr,
                          getCurCharacNo(), copy.m_addInfo, addInfo);
                }
                // ORIG：删除后重读槽位供历史日志计数（第二临时副本）。
                const Inven_Item after = cargo->GetSlot(fromSlot);
                if (after.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.AccountCargoDelItem(copy.m_addInfo, 0, 1,
                                                     (eItemDelReason)0x26);
                else
                    m_historyLog.AccountCargoDelItem(copy.m_addInfo,
                                                     after.m_addInfo2, addInfo,
                                                     (eItemDelReason)0x26);
            }
            break;
        }
        case 2:  // 金库→角色仓库
        {
            if (CheckInTrade())
                return 0x13;
            if (!IsExistAccountCargo())
                return 0x13;
            CAccountCargo* cargo = GetAccountCargo();

            item = cargo->GetSlot(fromSlot);

            if (item.m_addInfo > 0x1963 && item.m_addInfo < 7000)
                return 0x17;
            if (item.m_field1 == 4)
                return 0x17;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!cargoResolveSlot(getCurCharacCargoR(), toSlot))
                    return 4;
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int exist = getCurCharacCargoR()->check_item_exist(item.m_addInfo);
                if (exist >= 0)
                    toSlot = exist;
                else if (!cargoResolveSlot(getCurCharacCargoR(), toSlot))
                    return 4;
            }

            toSlot = getCurCharacCargoR()->tryInsertItemSpecialSlot(item, toSlot);
            if (toSlot < 0)
                return 4;

            bool delOk = cargo->DeleteItem(fromSlot, addInfo);
            if (delOk)
            {
                *outSlot =
                    getCurCharacCargoW()->insert_item_special_slot(item, toSlot);
                if (*outSlot == -1)
                {
                    char* accStr = NumberToString(get_acc_id(), 0);
                    cMyTrace trace("int CUser::exchange_interspace(int, int, "
                                   "itemIndexOnlyServer_t, int, int, int, "
                                   "itemIndexOnlyServer_t, int*)",
                                   0x3f17, 5);
                    trace("insertItemIntoInventory failed %s %d %d %d", accStr,
                          getCurCharacNo(), item.m_addInfo, addInfo);
                }
                const Inven_Item after = cargo->GetSlot(fromSlot);
                if (after.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.AccountCargoDelItem(item.m_addInfo, 0, 1,
                                                     (eItemDelReason)0x27);
                else
                    m_historyLog.AccountCargoDelItem(item.m_addInfo,
                                                     after.m_addInfo2, addInfo,
                                                     (eItemDelReason)0x27);
            }
            break;
        }
        case 3:
            return CheckInTrade() ? 0x13 : 1;
        case 7:
            return 1;
        case 0xc:  // 金库→金库
        {
            if (CheckInTrade())
                return 0x13;
            if (!IsExistAccountCargo())
                return 0x13;
            CAccountCargo* cargo = GetAccountCargo();
            if (!cargo->MoveItem(fromSlot, toSlot))
                return 0x11;
            *outSlot = toSlot;
            break;
        }
        default:
            return 1;
        }
        break;
    }
    default:
        return 1;
    }
    return 0;
}

// ============================================================================
// CUser::exchange_interspace_expand(...)
// ORIG 0x0865f614 sz 0x19b。扩容槽位切换包装：ChannelEquipslotSwitch 开启时
// 目的域 3↔0x12 互换校验；双开关状态与请求一致才允许 change_equip_expand。
// 参数序按 ORIG mangled EiimiiimPicc：i,i,m,i,i,i,m,Pi,c,c。
// ============================================================================
int CUser::exchange_interspace_expand(int fromSpace, int fromSlot,
                                      unsigned long fromItemIdx, int addInfo,
                                      int toSpace, int toSlot,
                                      unsigned long toItemIdx, int* outSlot,
                                      char channelSwitchReq, char expandSwitchReq)
{
    int checkToSpace = toSpace;
    if (getCurChannelEquipslotSwitch() == 1)
    {
        if (toSpace == 3)
            checkToSpace = 0x12;
        else if (toSpace == 0x12)
            checkToSpace = 3;
    }

    if (!_checkItemMovable(this, (ENUM_ITEMSPACE)fromSpace, fromSlot, fromItemIdx,
                           (ENUM_ITEMSPACE)checkToSpace, toSlot, toItemIdx))
    {
        *outSlot = -1;
        return 0x15;
    }

    if (getCurChannelEquipslotSwitch() != channelSwitchReq
        || getCurExpandEquipslotSwitch() != expandSwitchReq)
        return 1;

    if (getCurChannelEquipslotSwitch() == 0 && getCurExpandEquipslotSwitch() == 0)
        return change_equip_expand(fromSpace, fromSlot, toSpace, toSlot, outSlot);

    if (getCurChannelEquipslotSwitch() == 1 && getCurExpandEquipslotSwitch() == 0)
        return change_equip_expand(fromSpace, fromSlot, checkToSpace, toSlot,
                                   outSlot);

    return 1;
}
