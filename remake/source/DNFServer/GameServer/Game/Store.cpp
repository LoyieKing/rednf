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
#include "CDungeon.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>
#include <vector>

#include "CStatisticServerProxy.h"

// ---- 前向声明 ----
enum DEATHTOWER_VALUE_STATISTIC_FIELD
{
    DEATHTOWER_VALUE_STATISTIC_FIELD_0 = 0,
    DEATHTOWER_VALUE_STATISTIC_FIELD_9 = 9
};

class CDeathTowerValueStatistic
{
public:
    void SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field, char a, char b, unsigned int c);
};

extern CDeathTowerValueStatistic* GetInstanceDeathTowerValueStatistic()
{
    static CDeathTowerValueStatistic s;
    return &s;
}

struct Packet_DeathTower_Statistic_Value
{
    char m_pad0[0xa];
    char m_a;
    short m_b;
    int m_field;
    int m_c;
};

void CDeathTowerValueStatistic::SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field,
                                                              char a, char b, unsigned int c)
{
    Packet_DeathTower_Statistic_Value packet;
    memset(&packet, 0, sizeof(packet));
    packet.m_a = a;
    packet.m_b = (short)b;

    int f = (int)field;
    if (field == (DEATHTOWER_VALUE_STATISTIC_FIELD)10)
    {
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
                LogManager::logFormat(1, "Statistics.cpp",
                    "bool CDeathTowerValueStatistic::SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD, char, char, unsigned int)",
                    0x5d5, 0, 10);
                return;
            }
            f = 6;
            c = 1;
        }
    }

    packet.m_field = f;
    packet.m_c = c;

    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    if (proxy)
    {
        proxy->SendPacket((char*)&packet, 0x15);
    }
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

// 出售价格系数表（由 InitEquipmentScript 从脚本加载，索引 0=非随机选项，2=随机选项）
static int g_itemSellPriceRate[3] = {0};

// ---- 自由函数（ORIG 符号 _Z16getItemSellPriceiiib） ----
int getItemSellPrice(int basePrice, int curEndurance, int maxEndurance, bool isRandomOption)
{
    int rateIndex = isRandomOption ? 2 : 0;
    if (curEndurance > maxEndurance)
        curEndurance = maxEndurance;

    int rate = g_itemSellPriceRate[rateIndex];
    int price = (basePrice * rate) / 100;

    if (maxEndurance != 0)
        price = (price * curEndurance) / maxEndurance;

    return price;
}

Store* G_Store()
{
    static Store s;
    return &s;
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
int Store::GetSellItemPrice(CUser* user, char invenType, short slot, short count,
                            bool flag, int& outPrice, int& outCount)
{
    Inven_Item item;
    const CItem* pItem = NULL;
    int itemCount = 0;

    if (invenType == 0)
    {
        CInventory* inven = user->getCurCharacInvenW();
        inven->GetInvenSlotByRef(1, slot, item);
        pItem = G_CDataManager()->m_itemList->find_item(item.m_addInfo);
        itemCount = item.get_add_info();
    }
    else if (invenType == 2)
    {
        const CCargo* cargo = user->getCurCharacCargoR();
        item = cargo->get_cargo_slot(slot);
        pItem = G_CDataManager()->m_itemList->find_item(item.m_addInfo);
        itemCount = item.get_add_info();
    }
    else if (invenType == 3)
    {
        // creature slot
        CInventory* inven = user->getCurCharacInvenW();
        inven->GetInvenSlotByRef(INVEN_TYPE_CREATURE, slot, item);
        pItem = G_CDataManager()->m_itemList->find_item(item.m_addInfo);
        itemCount = item.get_add_info();
    }
    else if (invenType == 7)
    {
        // avatar slot
        CInventory* inven = user->getCurCharacInvenW();
        inven->GetInvenSlotByRef(INVEN_TYPE_AVATAR, slot, item);
        pItem = G_CDataManager()->m_itemList->find_item(item.m_addInfo);
        itemCount = item.get_add_info();
    }
    else
    {
        return 0x13;
    }

    if (pItem == NULL)
    {
        return 0x11;
    }

    int price = 0;
    int ret = GetSellItemPrice(item, pItem, (short)itemCount, flag, price);
    outPrice = price;
    outCount = itemCount;
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
// user_sell_item(CUser*, short slot, short invenType)
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
    if (invenType == 0)
    {
        CInventory* inven = user->getCurCharacInvenW();
        inven->delete_item(INVEN_TYPE_INVEN, slot, count, eItemDelReason_0, true);
    }
    else if (invenType == 2)
    {
        CCargo* cargo = user->getCurCharacCargoW();
        cargo->delete_item(slot, count, eItemDelReason_0);
    }
    else if (invenType == 3)
    {
        CInventory* inven = user->getCurCharacInvenW();
        inven->delete_item(INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_0, true);
        user->send_equip(slot);
    }
    else if (invenType == 7)
    {
        CInventory* inven = user->getCurCharacInvenW();
        inven->delete_item(INVEN_TYPE_AVATAR, slot, count, eItemDelReason_0, true);
    }

    CInventory* invenW = user->getCurCharacInvenW();
    invenW->gain_money(outCount, eMoneyAddReason_0, true, 0);

    if (!item.isEmpty())
    {
        CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
        if (pItem && pItem->is_stackable())
        {
            // stackable item handling
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

    return 0;
}
// ---- repair_equip 辅助类型 ----
struct tagRepairItemInfo
{
    int m_type;       // 0=背包, 1=装备栏
    int m_slot;       // 槽位索引
    CEquipItem* m_item;

    tagRepairItemInfo(int type, int slot, CEquipItem* item)
        : m_type(type), m_slot(slot), m_item(item) {}
};

extern int getEquipmentRepairPrice(int baseCost, int curEndurance, int maxEndurance,
                                   int growthGrade, bool flag, float rate);

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
            CInventory* inven = user->getCurCharacInvenW();
            Inven_Item slotItem;
            inven->GetInvenSlotByRef(1, i, slotItem);
            if (slotItem.get_add_info() != 0 && slotItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(slotItem.get_add_info());
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
                    bool needRepair = (curEnd != 0 && curEnd != maxEnd);
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(1, i, pEquip));
                }
            }
        }

        for (int i = 10; i < 0x16; i++)
        {
            CInventory* inven = user->getCurCharacInvenW();
            Inven_Item slotItem;
            inven->GetInvenSlotByRef(0, i, slotItem);
            if (slotItem.get_add_info() != 0 && slotItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(slotItem.get_add_info());
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
                    bool needRepair = (curEnd != 0 && curEnd != maxEnd);
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(0, i, pEquip));
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
                CEquipItem* pEquip = info.m_item;

                CInventory* inven2 = user->getCurCharacInvenW();
                Inven_Item curItem;
                inven2->GetInvenSlotByRef(invenType2, slotIdx, curItem);
                if (curItem.get_add_info() != 0 && curItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                {
                    int maxEnd = pEquip->get_endurance();
                    curItem.m_fieldb = (unsigned short)maxEnd;
                    inven2->update_item((INVEN_TYPE)invenType2, slotIdx, curItem);
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
            CInventory* inven = user->getCurCharacInvenW();
            inven->GetInvenSlotByRef(0, slot, item);
        }
        else
        {
            if (invenType != 0)
                return 0x11;
            CInventory* inven = user->getCurCharacInvenW();
            inven->GetInvenSlotByRef(1, slot, item);
        }

        if (item.get_add_info() == 0 || item.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
            return 0x11;

        outEndurance = item.m_fieldb;

        CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(item.get_add_info());
        if (!pEquip)
            return 0x11;

        if (pEquip->get_endurance() == 0)
            return 0x11;

        Inven_Item item2;
        if (count != 0xffff)
        {
            CInventory* inven = user->getCurCharacInvenW();
            inven->GetInvenSlotByRef(0, count, item2);
            if (item2.get_add_info() == 0 || item2.m_field1 != 2)
                return 0x15;
            int addInfo = item2.get_add_info();
            if (addInfo != 0x1f77 && addInfo != 0x201f)
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

        if (count != 0xffff && (item2.get_add_info() == 0x1f77 || item2.get_add_info() == 0x201f))
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
