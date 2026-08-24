// df_game_r Game/ WongWork::CItemUpgrade 实现。
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CItemAmplifier.h"
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager / CEventBase::GetRepeatEvent
#include "CEnvironment.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "DB_ReqItemUpgradeLog.h"
#include "PvPTypes.h"            // CMTRand（+0x4e8 randInt）
#include "GameTypes.h"           // cMyTrace

#include <cstring>
#include <cstdio>

// 类声明见 CDataManager.h（WongWork::CItemUpgrade）。
// ---- 强化事件类（CEventBase 派生；仅 GetReduceRate 用，ORIG 0x854b56e）----
// ORIG vtable 0x8b4f360，GetReduceRate() 返回 +0x04 short 经 cwtl 符号扩展。
class CReduceUpgradeItemPay : public CEventBase
{
public:
    int GetReduceRate() { return (short)(m_addInfo.m_info0); }
};

namespace WongWork
{
CItemUpgrade::~CItemUpgrade()
{
    // ORIG 0x85462c6：析构 +0x4e8 的 CMTRand*（构造时 new，见 0x8546247）。
    void** pRand = reinterpret_cast<void**>(reinterpret_cast<char*>(this) + 0x4e8);
    if (*pRand)
    {
        ::operator delete(*pRand);
        *pRand = 0;
    }
}

float CItemUpgrade::getUpgradeItemRepairCostRate(int idx, bool flag) const
{
    // ORIG 0x8549148：
    //   flag==true  → GlobalData::s_itemAmplifier_->getUpgradeInfoTable()（=+0x100）
    //   flag==false → this+0x04（CItemUpgradeTable 子对象，getUpgradeInfoTable 返回自身）
    //   返回 table[idx + 0x9c] 的第 4 字节偏移处 float。
    const char* table;
    if (flag)
        table = reinterpret_cast<const char*>(GlobalData::s_itemAmplifier_) + 0x100;
    else
        table = reinterpret_cast<const char*>(this) + 0x04;
    float rate = 0.0f;
    memcpy(&rate, table + (idx + 0x9c) * 4 + 4, sizeof(rate));
    return rate;
}

void CItemUpgrade::make3rdChroniclePacket(CUser* user,
                                          const Inven_Item& item,
                                          PacketGuard* packet)
{
    if (user == 0 || packet == 0)
        return;
    const unsigned char* fields =
        reinterpret_cast<const unsigned char*>(&item) + 0x1f;
    int count = (fields[0] & 1) != 0 ? 1 : 0;
    if (count != 0 && (fields[1] & 1) != 0)
        count = 2;
    if (count == 0)
    {
        packet->put_byte(0);
        return;
    }
    CItem* itemScript = G_CDataManager()->find_item(item.m_addInfo);
    if (itemScript == 0)
        return;
    packet->put_byte(count);
    for (int i = 0; i < count; ++i)
    {
        const unsigned char field = fields[i];
        const int kind = (field >> 1) & 3;
        const int enchantId = kind == 0 ? 0x4e6 :
                              kind == 1 ? 0x4e7 :
                              kind == 2 ? 0x4e8 : 0x4e9;
        packet->put_int(enchantId);
        packet->put_byte(user->get_charac_job());
        packet->put_byte(user->getCurCharFirstGrowType());
        packet->put_byte(itemScript->GetItemType());
        packet->put_byte(field >> 3);
    }
}

// ============================================================================
// enableAmplifyRandomUpgrade @ 0x854af38
//   校验随机增幅券并计算随机强化值写入 int& outCount。
//   返回错误码：0=通过, 4=找不到券/物品或非增幅券, 0xd=upgrade 越界,
//   0x13=物品不可增幅。
// ============================================================================
int CItemUpgrade::enableAmplifyRandomUpgrade(CUser* user, Inven_Item& item,
                                             Inven_Item& ticket,
                                             int& outCount)
{
    int ticketIdx = ticket.m_addInfo;
    CDataManager* dm = G_CDataManager();
    CItem* itemScript = dm->find_item(ticketIdx);
    if (itemScript == 0)
        return 4;

    const upgrade_table_t* table =
        GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    upgrade_info_t info;
    std::memset(&info, 0, sizeof(info));
    int characLevel = user->get_charac_level();
    if (_GetUpgradeInfo(&item, itemScript, *table, &info, characLevel) != 1)
        return 0xd;
    if (itemScript->IsImpossibleAmplify())
        return 0x13;

    int ret = _CheckCondition(user, item, itemScript, ticket, *table, info);
    if (ret != 0)
        return ret;

    int ticketIdx2 = ticket.m_addInfo;
    CItem* ticketScript = dm->find_item(ticketIdx2);
    if (ticketScript == 0)
        return 4;
    if (!ticketScript->is_stackable())
        return 4;
    CStackableItem* stackable = static_cast<CStackableItem*>(ticketScript);
    if (!stackable->is_random_amplify_reinforce_ticket())
        return 4;

    unsigned long range = 100;
    CMTRand* rng = *(CMTRand**)(reinterpret_cast<char*>(this) + 0x4e8);
    int roll = rng->randInt(range);
    outCount = stackable->get_amplify_reinforce_count(roll);
    if (outCount < 0 || outCount > 0x1f)
        return 4;
    return 0;
}

// ============================================================================
// ProcAmplifyRandomUpgrade @ 0x854ac8a
//   处理随机增幅结果：写升级值、统计、日志、广播。
//   返回错误码：0=成功, 4=失败。
// ============================================================================
int CItemUpgrade::ProcAmplifyRandomUpgrade(CUser* user, Inven_Item& item,
                                           Inven_Item& ticket, int update,
                                           const char* msg)
{
    cMyTrace trace(
        "int WongWork::CItemUpgrade::ProcAmplifyRandomUpgrade(CUser*, Inven_Item&, Inven_Item&, int, const char*)",
        0x82c, 0);
    int characNo = user->get_charac_no(-1);
    unsigned int accId = user->get_acc_id();
    char* idStr = NumberToString(accId, 0);
    trace("AmplifyRandomUpgrade m_id:%s char_no(%d) update:%d", idStr, characNo,
          update);

    int itemIdx = ticket.m_addInfo;
    CDataManager* dm = G_CDataManager();
    CItem* itemScript = dm->find_item(itemIdx);
    if (itemScript == 0)
        return 4;

    if (!item.m_amp.hasAbility())
        return 4;

    const upgrade_table_t* table =
        GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    unsigned char oldUpgrade = item.GetUpgrade();
    item.SetUpgrade((unsigned char)update);

    GetInstanceValueStatistic()->AddValueStatistic(
        (VALUE_STATISTIC_FIELD)0xe, user, 0);

    unsigned char newUpgrade = item.GetUpgrade();
    if (newUpgrade >= *(int*)((char*)table + 0x234))
    {
        _NoticeUpgrade(user, item, true);
    }

    unsigned char up2 = item.GetUpgrade();
    if (0xc < up2)
    {
        const char* characName = user->getCurCharacName();
        int characNo2 = user->getCurCharacNo();
        unsigned int itemId = ticket.m_addInfo;
        unsigned int envField = *(unsigned int*)((char*)G_CEnvironment() + 0x378);
        DB_ReqItemUpgradeLog::makeRequest(envField, itemId, characNo2,
                                          characName, up2 & 0xff, true);
    }

    unsigned char up3 = item.GetUpgrade();
    int itemIndex = itemScript->get_index();
    user->m_historyLog.UpgradeItem(itemIndex, oldUpgrade, up3 & 0xff, true);

    if (itemScript->IsRandomOption())
    {
        const char* itemName = itemScript->GetItemName();
        user->makeGuildItemUpgradMessage(itemName, oldUpgrade, up3 & 0xff, true);
    }
    else
    {
        user->makeGuildItemUpgradMessage(msg, oldUpgrade, up3 & 0xff, true);
    }
    return 0;
}

// ============================================================================
// _GetUpgradeInfo @ 0x854678a
//   按 upgrade_table_t 取 upgrade_info 到 out。
//   返回 get_upgrade_info 结果（0=越界/失败, 1=成功）。
// ============================================================================
int CItemUpgrade::_GetUpgradeInfo(const Inven_Item* item,
                                  const CItem* itemScript,
                                  const upgrade_table_t& table,
                                  upgrade_info_t* out, int level)
{
    unsigned char upgrade = item->GetUpgrade();
    if (upgrade >= 0x1f)
        return 0;
    unsigned char growthGrade =
        static_cast<unsigned char>(((CEquipItem*)itemScript)->getGrowthGrade(level));
    int equipType = itemScript->GetItemType();
    unsigned char rarity = static_cast<unsigned char>(itemScript->get_rarity());
    return table.get_upgrade_info(out, rarity, growthGrade, upgrade,
                                  (ENUM_EQUIPMENTTYPE)equipType);
}

// ============================================================================
// _CheckCondition @ 0x8546342
//   校验强化/增幅条件，返回错误码（0=通过）。
// ============================================================================
int CItemUpgrade::_CheckCondition(const CUser* user, const Inven_Item& item,
                                  const CItem* itemScript,
                                  const Inven_Item& ticket,
                                  const upgrade_table_t& table,
                                  const upgrade_info_t& info)
{
    if (!item.m_amp.isIdentified())
        return 0xae;
    if (ticket.m_addInfo != info.m_recipeItem)
        return 0x16;
    if (ticket.m_addInfo2 < info.m_meterialCount)
        return 0x16;

    int discountGroup = 0;
    CEventBase* repeatEvent =
        GlobalData::s_event_manager->GetRepeatEvent(0x32);
    // vtable+0x34（slot 13 = IsEventing(CUser*)）判活 + hasAbility。
    if (repeatEvent != 0 && !item.m_amp.hasAbility())
    {
        CReduceUpgradeItemPay* reduce =
            (CReduceUpgradeItemPay*)repeatEvent;
        int rate = reduce->GetReduceRate();
        if (0 < rate)
        {
            discountGroup = info.m_discountBase -
                            (info.m_discountBase * rate) / 100;
            if (discountGroup < 0)
                discountGroup = 0;
        }
    }
    else
    {
        discountGroup = info.m_discountBase;
    }

    const CInventory* inven = user->getCurCharacInvenR();
    Inven_Item slotItem = inven->GetInvenSlot(0, 0xb);
    int slotItemIdx = slotItem.m_addInfo;
    CDataManager* dm = G_CDataManager();
    CEquipItem* materialScript = static_cast<CEquipItem*>(dm->find_item(slotItemIdx));
    if (materialScript != 0)
    {
        // ORIG：折扣 = ROUND(getUpgradeCostDiscount(material) * discountGroup /
        //                     _DAT_08c9d304 + discountGroup)。_DAT_08c9d304 为该
        // 强化成本基准浮点常量。
        float discount = materialScript->getUpgradeCostDiscount();
        // _DAT_08c9d304 == -100.0f（ORIG ROUND 公式分母）。
        discountGroup =
            (int)((discount * (float)discountGroup) / -100.0f +
                  (float)discountGroup);
    }

    // 货币检查。
    if (inven->get_money() < discountGroup)
        return 10;

    unsigned char upgrade = item.GetUpgrade();
    if (table.m_infos.size() <= upgrade)
        return 0xd;
    if (item.m_field1 != 1)
        return 0x13;

    // 装备等级（vtable+0xc = GetItemType）检查 9<type<0x15 && type!=0xb。
    int itemType = itemScript->GetItemType();
    if (itemType <= 9 || itemType > 0x15 || itemType == 0xb)
        return 0x13;

    unsigned short endurance = item.m_fieldb;
    if (endurance != ((const CEquipItem*)itemScript)->get_endurance())
        return 7;

    int rarity = itemScript->get_rarity();
    int penTable = table.intAtConst((rarity + 0x100) * 4 + 0xc);
    if (penTable != -1)
    {
        if (penTable <= (int)upgrade)
            return 0x5f;
    }

    int penalty = table.GetPenaltyType(upgrade, (ENUM_RARITY)rarity, &info);
    if (penalty != 3)
        goto check_level;
    // 若 penalty 类型==3，预测结果物品数检查（InvenEmptycount+QuickEmptyCount）。
    {
        int predict = 1;
        if (user->getCurCharacInvenR()->InvenEmptycount(Inven_Item::ITEM_TYPE_3) +
                user->getCurCharacInvenR()->QuickEmptyCount() <
            (unsigned)predict)
            return 0x15;
    }
check_level:
    int upType = item.m_amp.hasAbility() ? 2 : 1;
    if (((CEquipItem*)itemScript)->isUpgradableLevel(
            static_cast<ItemUpgradeType::T>(upType),
            user->get_charac_level()) == false)
        return 0x13;
    return 0;
}

// ============================================================================
// _NoticeUpgrade @ 0x8546824
//   广播强化通知包（header 0x56）。
// ============================================================================
void CItemUpgrade::_NoticeUpgrade(const CUser* user, const Inven_Item& item,
                                  bool isAmplify)
{
    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(1);
    packet.put_byte(isAmplify ? 1 : 0);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    packet.put_int(item.m_addInfo);
    packet.put_byte((int)item.GetUpgrade());
    ((const RandomOption*)((const char*)&item + 0x25))
        ->put_packet_random_option(packet);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}
}  // namespace WongWork
