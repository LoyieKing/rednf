#include "LogManager.h"
// df_game_r Game/ Store 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/Store.md：
//   GetSellItemPrice(Inven_Item&,...) 0x08618ea0
//   GetSellItemPrice(CUser*,...)      0x08619008
//   repair_equip                      0x08619cec
//   user_buy_item                     0x08618a44
//   user_sell_item                    0x086193f8
#include "CInventory.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CGameManager.h"
#include "CEnvironment.h"   // GlobalInstance<Store>（ORIG G_Store 调 inst_ptr）
#include "CDungeon.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>
#include <vector>
#include "CHackLog_InvitationPaperSellToNPC.h"   // user_sell_item hack 检查
#include "CHackAnalyzer.h"                        // addServerHackCnt
#include "HistoryLog.h"                           // WriteSellItem

#include "CStatisticServerProxy.h"
#include "Packet_DeathTower_Statistic_Value.h"   // 类带 PacketHeader(0xc3b,0x15) ctor（ORIG C1 0x0860ffc2）

// ---- 前向声明 ----
enum DEATHTOWER_VALUE_STATISTIC_FIELD
{
    DEATHTOWER_VALUE_STATISTIC_FIELD_0 = 0,
    DEATHTOWER_VALUE_STATISTIC_FIELD_9 = 9
};

class CDeathTowerValueStatistic
{
public:
    bool SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field, char a, char b, unsigned int c);
};

extern CDeathTowerValueStatistic* GetInstanceDeathTowerValueStatistic()
{
    static CDeathTowerValueStatistic instance;
    return &instance;
}

bool CDeathTowerValueStatistic::SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field,
                                                              char a, char b, unsigned int c)
{
    Packet_DeathTower_Statistic_Value packet;
    packet.m_type = a;
    packet.m_level = (short)b;

    int f = (int)field;
    // ORIG 0x0860f342：field != 10 时直接 return false（jne → mov $0 出口，
    // 不组包不发送）；仅 field==10 才按 c 映射计数器下标。
    if (field != (DEATHTOWER_VALUE_STATISTIC_FIELD)10)
        return false;
    if (c == 1)
    {
        f = 4;
        c = 1;
    }
    else if (c == 0)
    {
        f = 3;
        c = 1;
    }
    else if (c == 2)
    {
        f = 5;
        c = 1;
    }
    else
    {
        if (c != 3)
        {
            // ORIG 0x0860f342：logFormat(1, "Statistics.cpp", __PRETTY_FUNCTION__,
            // 0x5d5, "CDeathTowerPlayDataStatistic ERROR!! deathTower_type (%d) ...", field)
            LogManager::logFormat(
                1, "Statistics.cpp", __PRETTY_FUNCTION__, 0x5d5,
                "CDeathTowerPlayDataStatistic ERROR!! deathTower_type (%d) level (%d) job_ (%d) grow_type_ (%d) clearStage (%d)",
                field);
            return false;
        }
        f = 6;
        c = 1;
    }

    packet.m_counterIdx = f;
    packet.m_value = c;

    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    // ORIG 0x0860f342 无条件 SendPacket（无 proxy 空检查），返回 true。
    proxy->SendPacket((char*)&packet, 0x15);
    return true;
}

// 修理价格系数（由 InitEquipmentScript 从脚本加载）
static float g_repairRate = 0.0f;

int getEquipmentRepairPrice(int baseCost, int curEndurance, int maxEndurance,
                            int growthGrade, bool flag, float rate)
{
    if (maxEndurance == 0 || curEndurance == maxEndurance)
        return 0;

    int base = ((growthGrade + 5) * baseCost) / 10;
    float price = (float)base * g_repairRate / (float)maxEndurance * (float)(maxEndurance - curEndurance);
    price *= rate;
    return (int)price;
}

// getItemSellPrice 的唯一实现归 ItemUpgradeVtableB2.cpp（ORIG 0x0898c86e）；
// 本 TU 的 Store::GetSellItemPrice 仅调用该共享自由函数。
int getItemSellPrice(int basePrice, int curEndurance, int maxEndurance, bool isRandomOption);

Store* G_Store()
{
    return GlobalInstance<Store>::inst_ptr();
}

// ---------------------------------------------------------------------------
// GetSellItemPrice(Inven_Item&, const CItem*, short, bool, int&)
// ---------------------------------------------------------------------------
int Store::GetSellItemPrice(Inven_Item& item, const CItem* pItem, short count,
                            bool flag, int& outPrice)
{
    if (item.isEquipableItemType())
    {
        // 装备类：检查 itemType 是否允许出售
        const CEquipItem* pEquip = static_cast<const CEquipItem*>(pItem);
        int itemType = pEquip->m_itemType;
        if (itemType == 0xb || itemType < 10)
        {
            return 0x17;
        }

        unsigned short endurance = item.m_fieldb;
        int maxEndurance = pEquip->get_endurance();
        if (maxEndurance < (int)endurance)
        {
            item.m_fieldb = (unsigned short)maxEndurance;
        }

        bool isRandom = pItem->IsRandomOption();
        int curEnd = item.m_fieldb;
        int sellPrice = pItem->GetSellPrice();
        outPrice = getItemSellPrice(sellPrice, curEnd, maxEndurance, isRandom);
    }
    else
    {
        // 非装备类：校验数量
        int addInfo = item.get_add_info();
        if (addInfo < count || count < 1)
        {
            return 0x11;
        }

        bool isRandom = pItem->IsRandomOption();
        int sellPrice = pItem->GetSellPrice();
        int price = getItemSellPrice(sellPrice, 0, 0, isRandom);
        outPrice = count * price;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GetSellItemPrice(CUser*, char invenType, short slot, short count, bool flag,
//                  int& outPrice, int& outCount) —— ORIG 0x08619008
// 汇合段（find_item 后共享检查，objdump 实证）：GetFinishPointPrice()!=0 → 0x17；
// GetAttachType()==2 → 0x17；*outCount = pItem->get_index()；再委托 5 参重载
// （count 传参数而非槽数量）。
// ---------------------------------------------------------------------------
int Store::GetSellItemPrice(CUser* user, char invenType, short slot, short count,
                            bool flag, int& outPrice, int& outCount)
{
    Inven_Item item;

    if (invenType == 0)
    {
        item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
    }
    else if (invenType == 2)
    {
        item = user->getCurCharacCargoR()->get_cargo_slot(slot);
    }
    else if (invenType == 3)
    {
        // creature slot
        item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, slot);   // ORIG: 消息 3 → type 0
    }
    else if (invenType == 7)
    {
        // avatar slot
        item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)3, slot);   // ORIG: 消息 7 → type 3
        if (item.m_field1 == 5)   // ORIG 分支独占检查：ITEM_TYPE==5 → 0x11（objdump 0x861932c）
            return 0x11;
    }
    else
    {
        return 0x13;
    }

    if (item.m_addInfo == 0)   // ORIG 共享段检查（find_item 前，m_addInfo 直读）
    {
        return 0x11;
    }
    const CItem* pItem = G_CDataManager()->m_itemList->find_item(item.m_addInfo);

    if (pItem == NULL)
    {
        return 0x11;
    }

    if (pItem->GetFinishPointPrice() != 0)
        return 0x17;
    if (pItem->GetAttachType() == 2)
        return 0x17;
    outCount = pItem->get_index();

    int price = 0;
    int ret = GetSellItemPrice(item, pItem, count, flag, price);
    outPrice = price;
    return ret;
}

// ---------------------------------------------------------------------------
// user_buy_item(CUser*, int itemId, int count)
// ---------------------------------------------------------------------------
int Store::user_buy_item(CUser* user, int itemId, int count)
{
    if (user->CheckInTrade())
    {
        return 0x13;
    }

    CItem* pItem = G_CDataManager()->m_itemList->find_item(itemId);
    if (pItem == NULL)
    {
        return 0x11;
    }

    int totalPrice;
    if (pItem->is_stackable())
    {
        totalPrice = pItem->get_price() * count;
    }
    else
    {
        totalPrice = pItem->get_price();   // ORIG +0x71 分支：不可堆叠同样取价（不乘 count）
    }
    const CInventory* invenR = user->getCurCharacInvenR();
    if (invenR->get_money() < totalPrice)
    {
        return 0xa;
    }

    Inven_Item item;
    item.m_addInfo = itemId;

    if (pItem->is_stackable())
    {
        // 可堆叠物品
        int attachType = pItem->GetAttachType();
        if (attachType == 1)
        {
            item.m_field1 = 3;
        }
        else if (attachType == 2)
        {
            item.m_field1 = 10;
        }
        else if (attachType == 8)
        {
            item.m_field1 = 4;
        }
        item.set_add_info(count);
        if (count > 0x3e7)
        {
            cMyTrace trace("int Store::user_buy_item(CUser*, int, int)", 0x3d, 5);
            trace("Store::user_buy_item,  ITEM #%d, %d", item.m_addInfo, count);
        }
        item.m_fieldb = 0;
        item.ResetItemAttr();
    }
    else
    {
        // 装备类
        CEquipItem* pEquip = static_cast<CEquipItem*>(pItem);
        if (pEquip->m_itemType == 0xb)
        {
            return 0x17;
        }
        if (pItem->GetAttachType() == 3)
        {
            item.m_field0 = 1;
        }
        item.m_field1 = 1;
        item.set_add_info(count);
        item.m_fieldb = (unsigned short)pEquip->get_endurance();
        item.ResetItemAttr();
    }

    CInventory* invenW = user->getCurCharacInvenW();
    int slot = invenW->insertItemIntoInventory(item, eItemAddReason_0, false, true);
    if (slot == -1)
    {
        return 4;
    }

    invenW = user->getCurCharacInvenW();
    if (!invenW->use_money(totalPrice, eMoneySubReason_0, true))
    {
        return 0xa;
    }

    // 统计
    int sellPrice = 0;
    GetSellItemPrice(item, pItem, (short)item.get_add_info(), false, sellPrice);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)6, user, sellPrice);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd, user, totalPrice);

    // 发送结果包
    PacketGuard packet;
    packet.put_header(1, 0x15);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_short(slot);
    packet.put_int(itemId);
    packet.put_int(count);
    packet.finalize(true);
    user->Send(packet);

    return 0;
}

// ---------------------------------------------------------------------------
// user_sell_item(CUser*, char invenType, short slot, short count) —— ORIG 0x86193f8
// ---------------------------------------------------------------------------
int Store::user_sell_item(CUser* user, char invenType, short slot, short count)
{
    if (user->CheckInTrade())
    {
        return 0x13;
    }

    int price = 0;
    int outCount = 0;
    bool isState5 = (user->get_state() == 5);
    int ret = GetSellItemPrice(user, invenType, slot, count, isState5, price, outCount);
    if (ret != 0)
    {
        return ret;
    }

    if (!user->CheckMoney(outCount))
    {
        return 0x16;
    }

    Inven_Item item;
    if (invenType == 2)
    {
        item = user->getCurCharacCargoR()->get_cargo_slot(slot);
    }
    else
    {
        int t = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)invenType);
        item = user->getCurCharacInvenR()->GetInvenSlot(t, slot);
    }

    if (invenType == 0)
    {
        CInventory* inven = user->getCurCharacInvenW();
        if (!inven->delete_item((INVEN_TYPE)1, slot, count, eItemDelReason_0, true)   )   // ORIG: 消息 0 → type 1
        {
            cMyTrace tr("int Store::user_sell_item(CUser*, char, short int, short int)",
                        0x136, 5);
            tr("Store::user_sell_item, delete_item failed , User ch=%d , %d %d",
               user->getCurCharacNo(), slot, count);
            return 0x11;
        }
    }
    else if (invenType == 2)
    {
        CCargo* cargo = user->getCurCharacCargoW();
        if (!cargo->delete_item(slot, count, eItemDelReason_0))
        {
            cMyTrace tr("int Store::user_sell_item(CUser*, char, short int, short int)",
                        0x141, 5);
            tr("Store::user_sell_item, delete_item failed 2, User ch=%d , %d %d",
               user->getCurCharacNo(), slot, count);
            return 0x11;
        }
    }
    else if (invenType == 3)
    {
        CInventory* inven = user->getCurCharacInvenW();
        if (!inven->delete_item((INVEN_TYPE)0, slot, 1, eItemDelReason_0, true)   )   // ORIG: 消息 3 → type 0
        {
            cMyTrace tr("int Store::user_sell_item(CUser*, char, short int, short int)",
                        0x14d, 5);
            tr("Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
               user->getCurCharacNo(), slot);
            return 0x11;
        }
        user->send_equip(slot);
    }
    else if (invenType == 7)
    {
        CInventory* inven = user->getCurCharacInvenW();
        if (!inven->delete_item((INVEN_TYPE)3, slot, count, eItemDelReason_0, true)   )   // ORIG: 消息 7 → type 3
        {
            cMyTrace tr("int Store::user_sell_item(CUser*, char, short int, short int)",
                        0x167, 5);
            tr("Store::user_sell_item, delete_item failed 4, User ch=%d , %d %d",
               user->getCurCharacNo(), slot, count);
            return 0x11;
        }
    }

    if (outCount > 0x7a11f)
    {
        int oldMoney = user->getCurCharacInvenR()->get_money();
        cMyTrace tr("int Store::user_sell_item(CUser*, char, short int, short int)",
                    0x172, 4);
        tr("User(%s) Charac(%s) gain money(%d) by selling item(%d), old money is %d.",
           NumberToString(user->get_acc_id(), 0),
           user->getCurCharacName(), outCount, price, oldMoney);
    }

    user->getCurCharacInvenW()->gain_money(outCount, eMoneyAddReason_0, true, 0);

    if (!item.isEmpty())
    {
        CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
        if (pItem && pItem->is_stackable())
        {
            item.m_addInfo2 = count;   // ORIG movswl count → item+0x7（stackable 覆写数量）
        }
        user->Add_RedeemInfo(item, outCount, true);
    }

    PacketGuard packet;
    packet.put_header(1, 0x18);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_byte(invenType);
    packet.put_short(slot);
    packet.put_short(count);
    packet.finalize(true);
    user->Send(packet);

    CHackLog_InvitationPaperSellToNPC hackLog;
    if (hackLog.IsInvitationPaper(price))
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)hackLog.GetHackType(), 1, 0, 0);
    }
    HistoryLog::WriteSellItem((FILE*)user->m_field796f8, price, count);

    return 0;
}
// ---- repair_equip 辅助类型 ----
struct tagRepairItemInfo
{
    INVEN_TYPE m_type;       // 0=背包, 1=装备栏
    int m_slot;              // 槽位索引
    CEquipItem const* m_item;

    // ORIG C1/C2 0x0861b08e（W）：三形参顺序赋入（INVEN_TYPE, int, CEquipItem const*）。
    tagRepairItemInfo(INVEN_TYPE type, int slot, CEquipItem const* item)
        : m_type(type), m_slot(slot), m_item(item) {}
};

extern int getEquipmentRepairPrice(int baseCost, int curEndurance, int maxEndurance,
                                   int growthGrade, bool flag, float rate);

// ---------------------------------------------------------------------------
// repair_equip(CUser*, char invenType, short slot, ushort count, ushort& out) ——
// ORIG 0x08619cec（docs/class_func_reports/Store/repair_equip.md 汇编+反编译 C）。
// slot==-1：全修 —— 装备栏 3..8（invenType 1）+ 背包 10..0x15（invenType 0）扫描，
//   收集需修装备（maxEnd!=0 且 curEnd!=maxEnd，objdump +0xc00/+0xe90），累计价格后
//   use_money(totalCost, reason 1, log 1)；成功先统计(0x14)+死亡之塔(仅 state==10、
//   dungeon idx!=11000) 上报，再逐件重取槽写满耐久 update_item，最后对背包
//   10..0x15 发 SendUpdateItem(1, space 3)；失败返 10。返回 0x13=交易中。
// 单件：msg invenType 0→装备栏(type 1)/2→cargo/3→背包(type 0)，其余返 0x11；
//   空槽或非 ITEM_TYPE_EQUIP 返 0x11；*out=当前耐久；查表空/满耐久返 0x11；
//   count!=0xffff 时从 GetInvenSlot(1,count) 取工具（须 ITEM_TYPE_2 且
//   m_addInfo==0x1f77/0x201f，否则 0x15）→ use_item(count,1,0) 成功扣 1 并把
//   修理费置 0（失败 0x16）；use_money 失败返 10；耐久写满后按 msg 空间
//   update_item + SendUpdateItem(1, space 0/2/3)。
// 字段口径：判空/查表/工具 id 全程直读 +0x02 m_addInfo（item idx）；耐久为
//   +0x0b m_fieldb。m_addInfo2(+0x07, get_add_info) 是 stackable 数量域，勿混。
// ---------------------------------------------------------------------------
int Store::repair_equip(CUser* user, char invenType, short slot, unsigned short count,
                        unsigned short& outEndurance)
{
    if (user->CheckInTrade())
        return 0x13;

    Inven_Item item;
    int totalCost = 0;

    if (slot == -1)
    {
        std::vector<tagRepairItemInfo> repairList;

        for (int i = 3; i < 9; i++)
        {
            // ORIG _ZNK10CInventory12GetInvenSlotEii（const 值返回）
            Inven_Item slotItem = user->getCurCharacInvenR()->GetInvenSlot(1, i);
            if (slotItem.m_addInfo != 0 && slotItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(slotItem.m_addInfo);
                if (pEquip)
                {
                    int level = user->get_charac_level();
                    int growthGrade = pEquip->getGrowthGrade(level);
                    int growthCost = pEquip->getGrowthRepairCost(level);
                    bool hasAmp = slotItem.m_amp.hasAbility();
                    unsigned char upgrade = slotItem.GetUpgrade();
                    float rate = G_CDataManager()->getUpgradeItemRepairCostRate(upgrade, hasAmp);
                    int maxEnd = pEquip->get_endurance();
                    int cost = getEquipmentRepairPrice(growthCost, slotItem.m_fieldb, maxEnd,
                                                        growthGrade, false, rate);
                    totalCost += cost;

                    int curEnd = slotItem.m_fieldb;
                    // ORIG 判定：maxEnd==0 → 不入列；否则 curEnd!=maxEnd 即需修
                    // （curEnd==0 的损坏装备同样入列）。objdump +0xc00..+0xc2f。
                    bool needRepair = (maxEnd != 0 && curEnd != maxEnd);
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(INVEN_TYPE_EQUIP, i, pEquip));
                }
            }
        }

        for (int i = 10; i < 0x16; i++)
        {
            Inven_Item slotItem = user->getCurCharacInvenR()->GetInvenSlot(0, i);
            if (slotItem.m_addInfo != 0 && slotItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(slotItem.m_addInfo);
                if (pEquip)
                {
                    int level = user->get_charac_level();
                    int growthGrade = pEquip->getGrowthGrade(level);
                    int growthCost = pEquip->getGrowthRepairCost(level);
                    bool hasAmp = slotItem.m_amp.hasAbility();
                    unsigned char upgrade = slotItem.GetUpgrade();
                    float rate = G_CDataManager()->getUpgradeItemRepairCostRate(upgrade, hasAmp);
                    int maxEnd = pEquip->get_endurance();
                    int cost = getEquipmentRepairPrice(growthCost, slotItem.m_fieldb, maxEnd,
                                                        growthGrade, false, rate);
                    totalCost += cost;

                    int curEnd = slotItem.m_fieldb;
                    // ORIG 判定同第一循环（objdump +0xe90..+0xebe）。
                    bool needRepair = (maxEnd != 0 && curEnd != maxEnd);
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(INVEN_TYPE_INVEN, i, pEquip));
                }
            }
        }

        CInventory* invenW = user->getCurCharacInvenW();
        bool ok = invenW->use_money(totalCost, (eMoneySubReason)1, true);
        if (ok)
        {
            GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x14, user, totalCost);

            int state = user->get_state();
            if (state == 10)
            {
                WongWork::CDeathTower* tower = user->getDeathTower();
                if (tower)
                {
                    WongWork::CDeathTower::CDungeonMgr* mgr = tower->getCDungeonMGr();
                    CDungeon* dun = mgr->getDungeon();
                    int idx = dun->get_index();
                    bool notTower = (idx != 11000);
                    int level = user->get_charac_level();
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)9, (char)notTower, (char)level, totalCost);
                }
            }


            for (size_t i = 0; i < repairList.size(); i++)
            {
                const tagRepairItemInfo& info = repairList[i];
                int invenType2 = info.m_type;
                int slotIdx = info.m_slot;
                CEquipItem* pEquip = const_cast<CEquipItem*>(info.m_item);

                // ORIG 迭代内重取：GetInvenSlot(type, slot) 值版；update 走 W。
                Inven_Item curItem =
                    user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)invenType2, slotIdx);
                if (curItem.m_addInfo != 0 && curItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                {
                    int maxEnd = pEquip->get_endurance();
                    curItem.m_fieldb = (unsigned short)maxEnd;
                    user->getCurCharacInvenW()->update_item((INVEN_TYPE)invenType2, slotIdx, curItem);
                }
            }

            for (int i = 10; i < 0x16; i++)
                user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, i);

            return 0;
        }
        else
        {
            return 10;
        }
    }
    else
    {
        if (invenType == 2)
        {
            const CCargo* cargoR = user->getCurCharacCargoR();
            item = cargoR->get_cargo_slot(slot);
        }
        else if (invenType == 3)
        {
            item = user->getCurCharacInvenR()->GetInvenSlot(0, slot);
        }
        else
        {
            if (invenType != 0)
                return 0x11;
            item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        }

        // ORIG 全程直读 item+0x02 的 m_addInfo（item idx）；get_add_info() 对应
        // +0x07 m_addInfo2（stackable 数量域），普通装备为 0，不得用于判空/查表。
        if (item.m_addInfo == 0 || item.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
            return 0x11;

        outEndurance = item.m_fieldb;

        CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
        if (!pEquip)
            return 0x11;

        if (pEquip->get_endurance() == 0)
            return 0x11;

        Inven_Item item2;
        if (count != 0xffff)
        {
            item2 = user->getCurCharacInvenR()->GetInvenSlot(1, count);   // ORIG GetInvenSlot(1, count)
            if (item2.m_addInfo == 0 || item2.m_field1 != Inven_Item::ITEM_TYPE_2)
                return 0x15;
            int toolIdx = item2.m_addInfo;
            if (toolIdx != 0x1f77 && toolIdx != 0x201f)
                return 0x15;
        }

        int level = user->get_charac_level();
        int growthGrade = pEquip->getGrowthGrade(level);
        int growthCost = pEquip->getGrowthRepairCost(level);
        bool hasAmp = item.m_amp.hasAbility();
        unsigned char upgrade = item.GetUpgrade();
        float rate = G_CDataManager()->getUpgradeItemRepairCostRate(upgrade, hasAmp);
        int maxEnd = pEquip->get_endurance();
        int cost = getEquipmentRepairPrice(growthCost, item.m_fieldb, maxEnd,
                                            growthGrade, false, rate);

        if (count != 0xffff && (item2.m_addInfo == 0x1f77 || item2.m_addInfo == 0x201f))
        {
            CInventory* invenW = user->getCurCharacInvenW();
            int ret = invenW->use_item(count, 1, 0);
            if (ret != 0)
                return 0x16;
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, count);
            cost = 0;
        }

        CInventory* invenW = user->getCurCharacInvenW();
        bool ok = invenW->use_money(cost, (eMoneySubReason)1, true);
        if (!ok)
            return 10;

        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x14, user, cost);

        int state = user->get_state();
        if (state == 10)
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower)
            {
                WongWork::CDeathTower::CDungeonMgr* mgr = tower->getCDungeonMGr();
                CDungeon* dun = mgr->getDungeon();
                int idx = dun->get_index();
                bool notTower = (idx != 11000);
                int lvl = user->get_charac_level();
                GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                    (DEATHTOWER_VALUE_STATISTIC_FIELD)9, (char)notTower, (char)lvl, cost);
            }
        }

        int maxEndurance = pEquip->get_endurance();
        item.m_fieldb = (unsigned short)maxEndurance;

        if (invenType == 2)
        {
            CCargo* cargoW = user->getCurCharacCargoW();
            cargoW->update_item(slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)2, slot);
        }
        else if (invenType == 3)
        {
            CInventory* invenW2 = user->getCurCharacInvenW();
            invenW2->update_item((INVEN_TYPE)0, slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, slot);
        }
        else if (invenType == 0)
        {
            CInventory* invenW2 = user->getCurCharacInvenW();
            invenW2->update_item((INVEN_TYPE)1, slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    return 0;
}

// ---- force 发射 tagRepairItemInfo C1/C2（ORIG W 0x0861b08e）----
namespace {
void forceTagRepairItemInfoEmission()
{
    tagRepairItemInfo tri(INVEN_TYPE_EQUIP, 0, 0);
    (void)tri;
}
}  // namespace
