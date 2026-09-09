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

// ============================================================================
// CUser::delete_item(char space, int position, unsigned long itemIdx,
//                    int count, eItemDelReason reason)
// ORIG 0x0866285c sz 0x25d。返回：0 成功 / 0x11 校验或删除失败 / 0x13 交易中。
// [b28d-solo-CUserStatC 判词·AE] P167 CLEAR（17/17），orig 162 = our 162 逐条对齐
//   （仅跳表地址/jump-table 重定位属 AE 口径）。本件修法：
//   ① 0x086628c1 比较：ORIG 为 4.1 bool 寄存器物化 `setne %al; test %al,%al; je`，
//      if 形 4.4 恒折 `je` 直跳 ⇒ 改 P225 形 `while (cond) { return 0x11; }`
//      （探针 /tmp/b28d_cstatc_probe2.cpp fJ 逐条命中）。
//   ② switch 叶块物理序 = case 书写序（P46）：ORIG 序 case0,case2,case3,case1,case7
//      ⇒ 源码 case 1 移至 case 3 之后、case 7 之前（原序 case0,1,2,3,7 致块错位）。
// ============================================================================
int CUser::delete_item(char itemSpace, int position, unsigned long itemIdx,
                       int count, eItemDelReason reason)
{
    // b28d-49 判词：N 封顶（N82 帧/槽 + N89）。P167 CLEAR（17/17）。@0x866285c
    if (CheckInTrade())
        return 0x13;

    // GetInvenSlot(out, curCharacInvenR(), GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), position)
    Inven_Item slotItem = getCurCharacInvenR()->GetInvenSlot(
        (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)itemSpace), position);
    // b28d-solo-CUserStatC：ORIG 0x086628c1 该比较为 4.1 的 bool 寄存器物化
    // （setne %al; test %al,%al; je，无栈槽）；if 形 4.4 恒折 je 直跳。
    // P225 形（while+return）精确复现四段物化（探针 /tmp/b28d_cstatc_probe2.cpp fJ）。
    while ((unsigned long)slotItem.m_addInfo != itemIdx)
    {
        return 0x11;
    }

    switch (itemSpace)
    {
    case 0:  // INVEN_TYPE_EQUIP(1) 背包
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, position, count,
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
    // b28d-solo-CUserStatC：ORIG 叶块物理序 = case0,case2,case3,case1,case7
    // （P46：叶块排放顺序 = case 书写顺序），故 case 1 紧跟 case 3。
    case 1:  // INVEN_TYPE_AVATAR(2)
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_AVATAR, position, count,
                                               reason, true))
            return 0x11;
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
// [b28d-solo-CUserStatC 判词·DIFF→N 封顶] §6.2 三件套：①P167 CLEAR 265/265
//   [0 缺 0 多]；②gap 2637−2619 = 18（our−18）；③帧/槽：ORIG `sub $0x45c` +
//   push edi/esi/ebx vs 我们 `sub $0x490` + push ebx/esi（N82 槽复用/帧分区族）。
//   本波次修正（语义级，均已 fresh 复编核销）：
//   ① 真语义 bug：case 3→7 的 GetInvenRef 实参原写 INVEN_TYPE_INVEN(0)；
//      ORIG 0x08660ec4 为 INVEN_TYPE_CREATURE(3)（目标为宠物槽）——已修。
//   ② 5 处 `check_trade_possibility() == 1` 改反极性 `if (!f())`（P272/P292）：
//      ORIG 为 `call; xor $0x1; test %al,%al; je <then>`，`==1` 形 4.4 恒产 cmp $1。
//   ③ 13 处 bool callee 的 `!= 1`/`== 1` 改 `!f()`/真值（P292 档①）：
//      CCargo::check_slot_empty、CAccountCargo::{CheckInsertCondition,CheckSlotEmpty,
//      CheckStackLimit} 声明+定义 int→bool（已 R2 广播；CUser.h/CAccountCargo.cpp
//      /CCargo.cpp 定义端由 Main 代落同步）。
//   ④ case 1 源两组 if-else 链、case 7 源 if-else 链改 switch（P286/P208 推广）：
//      ORIG 为 `mov 0x1c(%ebp),%eax; cmp $K,%eax; je` 载入寄存器比较链，4.4 对
//      if 链恒折内存直比；switch 按 case 书写序复现（case1: 3,4,1；case7: 4,7,3）。
//   ⑤ case 3→1 的 `ret < 1` 改正守卫早返（ORIG 0x0866168a 为 `cmpl $0; jle <then>`，
//      `ret<1`/`ret<=0` 均使 4.4 反产 `jg`）。
//   ⑥ 5 处 move_item 返回检查改 P225 形 `while (ret != 0) { return ret; }`：
//      ORIG 为 `cmpl $0x0,槽; setne %al; test %al,%al; je`（4.1 bool 物化），
//      if 形 4.4 恒折 `cmpl; jne/je`。
//   ⑦ 2 处 `if (!delOk)` 反写为 `if (delOk) {成功} else {trace}`（N122：4.4 对 bool
//      局部取反恒加 `movzbl; xor $1`；ORIG 为 `mov %al,槽; cmpb $0x0,槽; je <失败>`）。
//   ⑧ CCargo::move_item / CAccountCargo::DeleteItem int→bool（P292 档①；定义端
//      CCargo.cpp:376 / CAccountCargo.cpp:247 由 Main 同步）。
//   残余 N：4.1 `mov 槽,%eax; shr $0x1f; test %al,%al; je`（12 处 toSlot<0 符号位
//   lowering；4.4 恒 test/jns，探针 /tmp/b28d_cstatc_probe3.cpp 六形全败）+ N82 帧/槽
//   + 跳表数据地址（AE 伪影）。
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
    // b28d-solo-CUserStatC 判词：N 封顶（N82 帧/槽 + 4.1 shr $0x1f 符号位 lowering）。
    // P167 CLEAR（265/265）；gap 18/2637；真语义 bug 1 处已修。@0x865f7b0
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
            // b28d-solo-CUserStatC：ORIG 0x0865f8a0 为 `cmpl $0x0,槽; setne %al;
            // test %al,%al; je <成功>`（4.1 bool 物化）⇒ P225 形 while+return 复现。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            return 0;
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
                if (!getCurCharacCargoR()->check_slot_empty(toSlot))
                {
                    toSlot = getCurCharacCargoR()->get_empty_slot();
                    if (toSlot < 0)
                        return 4;
                }
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
                else
                {
                    if (!getCurCharacCargoR()->check_slot_empty(toSlot))
                    {
                        toSlot = getCurCharacCargoR()->get_empty_slot();
                        if (toSlot < 0)
                            return 4;
                    }
                }
            }

            toSlot = getCurCharacCargoR()->tryInsertItemSpecialSlot(item, toSlot);
            if (toSlot < 0)
                return 4;

            bool delOk = getCurCharacInvenW()->delete_item(
                INVEN_TYPE_EQUIP, fromSlot, addInfo, (eItemDelReason)7, true);
            // b28d-solo-CUserStatC：ORIG 0x0865fbdf 为 `mov %al,槽; cmpb $0x0,槽;
            // je <失败 trace 块>`（4.1 bool 局部直测）；4.4 对 `if (!delOk)` 恒加
            // `movzbl; xor $1`（N122）⇒ 反写 `if (delOk)` 复现。
            if (delOk)
            {
                *outSlot =
                    getCurCharacCargoW()->insert_item_special_slot(item, toSlot);

                const Inven_Item slotIt =
                    getCurCharacCargoR()->get_cargo_slot(*outSlot);
                if (slotIt.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.ItemAddCargo(item.m_addInfo, addInfo, 1);
                else
                    m_historyLog.ItemAddCargo(item.m_addInfo, addInfo,
                                              slotIt.get_add_info());
            }
            else
            {
                // ORIG：删除失败仍返回 0
                cMyTrace trace("int CUser::exchange_interspace(int, int, "
                               "itemIndexOnlyServer_t, int, int, int, "
                               "itemIndexOnlyServer_t, int*)",
                               0x3b21, 5);
                trace("delete_item failed %d %d %d", getCurCharacNo(), fromSlot,
                      addInfo);
            }
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
            CTradeSpace* trade = G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_tradeSpaceIdx = -1;
                return 0x15;
            }
            // b28d-solo-CUserStatC：ORIG 0x0865fdfd 为 `xor $1; test %al,%al;
            // je <then>`（bool callee 取反段）⇒ 源码是反极性 if(!f()){else 体}
            // else{then 体}（P272/P292）；写 ==1 时 4.4 恒产 cmp $1。
            if (!trade->check_trade_possibility())
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            else
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
            if (!cargo->CheckInsertCondition(item))
                return 0x11;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!cargo->CheckSlotEmpty(toSlot))
                {
                    toSlot = cargo->GetEmptySlot();
                    if (toSlot < 0)
                        return 4;
                }
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int specific = cargo->GetSpecificItemSlot(item.m_addInfo);
                if (specific >= 0)
                {
                    if (!cargo->CheckStackLimit(specific, item.m_addInfo, addInfo))
                        return 4;
                    toSlot = specific;
                }
                else
                {
                    if (!cargo->CheckSlotEmpty(toSlot))
                    {
                        toSlot = cargo->GetEmptySlot();
                        if (toSlot < 0)
                            return 4;
                    }
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
            // b28d-solo-CUserStatC：ORIG 0x08660624 为 `mov %al,槽; cmpb $0x0,槽;
            // je <失败 trace 块>`；4.4 对 `if (!delOk)` 恒加 `movzbl; xor $1`（N122）
            // ⇒ 反写 `if (delOk)` 复现。
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
                                   0x3be5, 5);
                    trace("insertItemIntoInventory failed %s %d %d %d", accStr,
                          getCurCharacNo(), copy.m_addInfo, addInfo);
                }
            }
            else
            {
                cMyTrace trace("int CUser::exchange_interspace(int, int, "
                               "itemIndexOnlyServer_t, int, int, int, "
                               "itemIndexOnlyServer_t, int*)",
                               0x3bea, 5);
                trace("delete_item failed %d %d %d", getCurCharacNo(),
                      item.m_addInfo, addInfo);
            }
            return 0;
        }
        case 2:  // 仓库→仓库
            // b28d-solo-CUserStatC：ORIG 0x0866080d 为 `call; xor $0x1,%eax;
            // test %al,%al; je <then>` ⇒ 反极性早返形（P292/P272）。
            if (!getCurCharacCargoW()->move_item(fromSlot, toSlot))
                return 0x11;
            *outSlot = toSlot;
            return 0;
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

            if (!cargo->CheckInsertCondition(item))
                return 0x11;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!cargo->CheckSlotEmpty(toSlot))
                {
                    toSlot = cargo->GetEmptySlot();
                    if (toSlot < 0)
                        return 4;
                }
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int specific = cargo->GetSpecificItemSlot(item.m_addInfo);
                if (specific >= 0)
                {
                    if (!cargo->CheckStackLimit(specific, item.m_addInfo, addInfo))
                        return 4;
                    toSlot = specific;
                }
                else
                {
                    if (!cargo->CheckSlotEmpty(toSlot))
                    {
                        toSlot = cargo->GetEmptySlot();
                        if (toSlot < 0)
                            return 4;
                    }
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
            if (WongWork::CAvatarItemMgr::IsTempKey((int)equipSlot.m_addInfo2)
                || WongWork::CAvatarItemMgr::IsTempKey((int)avatarSlot.m_addInfo2))
                return 0x13;
            int ret = change_equip(fromSpace, (short)fromSlot, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x0866168a 为 `cmpl $0x0,槽; jle <then>`
            // （then 为跳转目标、else 直落返回 ret）⇒ 必须写成「正守卫早返」形，
            // `if (ret < 1) {then}` 与 `if (ret <= 0) {then}` 均使 4.4 反产 `jg`。
            if (ret > 0)
                return ret;
            *outSlot = toSlot;
            return 0;
        }
        case 2:
            return CheckInTrade() ? 0x13 : 1;
        case 4:  // 装备→交易窗口
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_tradeSpaceIdx = -1;
                return 0x15;
            }
            // b28d-solo-CUserStatC：ORIG 0x0866103e 同为 bool 取反段 ⇒ 反极性 if。
            if (!trade->check_trade_possibility())
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            else
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 3, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
                send_equip(fromSlot);
            }
            return 0;
        }
        case 7:  // 装备→宠物
        {
            if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
            {
                // b28d-solo-CUserStatC：真语义 bug 修正——ORIG 0x08660ec4 实参为
                // INVEN_TYPE_CREATURE(3)（目标为宠物槽），原写 INVEN_TYPE_INVEN(0) 取错域。
                const Inven_Item* ref = getCurCharacInvenR()->GetInvenRef(
                    INVEN_TYPE_CREATURE, toSlot);
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
            // b28d-solo-CUserStatC：ORIG 0x08660f6d 同 setne 物化 ⇒ P225 形。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            send_equip(toSlot);
            return 0;
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

        CTradeSpace* trade = G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
        if (trade == 0)
            return 1;
        if (trade->is_empty())
        {
            m_tradeSpaceIdx = -1;
            return 0x15;
        }
        // b28d-solo-CUserStatC：ORIG 0x086619f6 同为 bool 取反段 ⇒ 反极性 if。
        if (!trade->check_trade_possibility())
        {
            trade->cancel_trade();
            G_CGameManager()->PutTradeSpace(trade);
        }
        else
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
        break;
    }

    // --------------------------------------------------------------- 时装源
    // [BigUserLogic] ORIG fromSpace 跳表(0x8cef0f4)体物理序 = 0,2,3,4,1,7,12
    // （case1 体 @+0x195e 在 case4 @+0x17ff 后、case7 @+0x2066 前）；4.4 -O0 的
    // switch 体序 = case 书写序 ⇒ 整块移到 case4 之后，语义零改动。
    case 1:
    {
        // ORIG 在分支入口构造两个临时项并按目的域选择性填充。
        Inven_Item itemA;
        Inven_Item itemB;  // toSpace==4 时仅保持构造初值（ORIG 同）
        // b28d-solo-CUserStatC：ORIG 0x0866112a/0x08661641 为「形参载入寄存器 + cmp 链」
        // （mov 0x1c(%ebp),%eax; cmp $3/$4/$1; je），4.4 对 if-else 链恒折内存直比；
        // 3-case switch 按书写序复现该链（P286/P208 推广，探针 /tmp/b28d_cstatc_probe4.cpp）。
        switch (toSpace)
        {
        case 3:
            itemA = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, fromSlot);
            // ORIG：toSpace==3 各子分支独立读两个槽（itemA/itemB 不合并条件）。
            itemB = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_INVEN, toSlot);
            break;
        case 4:
            itemA = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, fromSlot);
            break;
        case 1:
            itemA = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, fromSlot);
            itemB = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, toSlot);
            break;
        default:
            break;
        }

        // ORIG:IsTempKey 为静态语义（值符号位判定），直接静态调用，不经 mgr 访问器。
        if (WongWork::CAvatarItemMgr::IsTempKey((int)itemA.m_addInfo2)
            || WongWork::CAvatarItemMgr::IsTempKey((int)itemB.m_addInfo2))
            return 0x13;

        switch (toSpace)
        {
        case 3:
        {
            int ret = change_equip(fromSpace, (short)fromSlot, toSlot);
            if (ret > 0)
                return ret;
            *outSlot = toSlot;
            break;
        }
        case 4:
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_tradeSpaceIdx = -1;
                return 0x15;
            }
            // b28d-solo-CUserStatC：ORIG 0x08660de9 同为 bool 取反段 ⇒ 反极性 if。
            if (!trade->check_trade_possibility())
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            else
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 1, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
            }
            break;
        }
        case 1:
        {
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_AVATAR, fromSlot, INVEN_TYPE_AVATAR, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x086616e3 同 setne 物化 ⇒ P225 形。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            break;
        }
        default:
            return 1;
        }
        break;
    }

    // -------------------------------------------------------------- 宠物源
    case 7:
        // b28d-solo-CUserStatC：ORIG 0x08661816 为「形参载入寄存器 + cmp 链 4/7/3」
        // （cmp 序 = case 书写序），if-else 链 4.4 恒折内存直比 ⇒ 改 switch。
        switch (toSpace)
        {
        case 4:
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_tradeSpaceIdx = -1;
                return 0x15;
            }
            // b28d-solo-CUserStatC：ORIG 0x08661767 同为 bool 取反段 ⇒ 反极性 if。
            if (!trade->check_trade_possibility())
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            else
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 7, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
            }
            break;
        }
        case 7:
        {
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_CREATURE, fromSlot, INVEN_TYPE_CREATURE, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x08661900 同 setne 物化 ⇒ P225 形。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            break;
        }
        case 3:
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
            // b28d-solo-CUserStatC：ORIG 0x08661972 同 setne 物化 ⇒ P225 形。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            send_equip(toSlot);
            return 0;
        }
        default:
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
                if (!getCurCharacCargoR()->check_slot_empty(toSlot))
                {
                    toSlot = getCurCharacCargoR()->get_empty_slot();
                    if (toSlot < 0)
                        return 4;
                }
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int exist = getCurCharacCargoR()->check_item_exist(item.m_addInfo);
                if (exist >= 0)
                    toSlot = exist;
                else
                {
                    if (!getCurCharacCargoR()->check_slot_empty(toSlot))
                    {
                        toSlot = getCurCharacCargoR()->get_empty_slot();
                        if (toSlot < 0)
                            return 4;
                    }
                }
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

    // b28d-49 @0x865f6ba：ORIG `call getCurChannelEquipslotSwitch; cmp -0x1c(%ebp),%al;
    // jne` = 8 位同型比较；getter 返 unsigned char + 形参 char 触发 4.4 32 位扩展
    //（movzbl+movsbl+cmp reg），调用点 (char) 收窄即复现（探针 g4）。
    if ((char)getCurChannelEquipslotSwitch() != channelSwitchReq
        || (char)getCurExpandEquipslotSwitch() != expandSwitchReq)
        return 1;

    int unusedSlot = 0;
    if (getCurChannelEquipslotSwitch() == 0 && getCurExpandEquipslotSwitch() == 0)
        return change_equip_expand(fromSpace, fromSlot, toSpace, toSlot, outSlot);

    if (getCurChannelEquipslotSwitch() == 1 && getCurExpandEquipslotSwitch() == 0)
        return change_equip_expand(fromSpace, fromSlot, checkToSpace, toSlot,
                                   outSlot);

    return 1;
}
