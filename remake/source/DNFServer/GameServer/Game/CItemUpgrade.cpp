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
#include "DisJoint.h"            // DisJoint::GetResult/getPredictResultItemTypeCount
#include "CCubeStatistic.h"      // CCubeStatistic::collectCubeStatistics
#include "STStackableScript.h"   // STEnchantSystemMapKey/MapData
#include "PacketTypes.h"         // MSG_ENCHANT 完整类型
#include "CReduceUpgradeItemPay.h"   // CReduceUpgradeItemPay（GetReduceRate 权威声明）

#include <cstring>
#include <cstdio>

// 类声明见 CDataManager.h（WongWork::CItemUpgrade）。
namespace WongWork
{
CItemUpgrade::~CItemUpgrade()
{
    // ORIG 0x85462c6：析构 +0x4e8 的 CMTRand*（构造时 new，见 0x8546247）。
    if (m_rand)
    {
        ::operator delete(m_rand);
        m_rand = 0;
    }
}

float CItemUpgrade::getUpgradeItemRepairCostRate(int idx, bool flag) const
{
    // ORIG 0x8549148：
    //   flag==true  → GlobalData::s_itemAmplifier_->getUpgradeInfoTable()（=+0x100）
    //   flag==false → this+0x04（CItemUpgradeTable 子对象，getUpgradeInfoTable 返回自身）
    //   返回 table[(idx + 0x9c) * 4 + 4] 处 float（混合表按 4 字节步长）。
    float rate;
    if (flag)
    {
        rate = GlobalData::s_itemAmplifier_->getUpgradeInfoTable()
                   ->floatAt((idx + 0x9c) * 4 + 4);
    }
    else
    {
        rate = getUpgradeInfoTable()->floatAt((idx + 0x9c) * 4 + 4);
    }
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
    int itemIdx = item.m_addInfo;   // ORIG 0x854af38：mov 0x10(%ebp); mov 0x2(%eax) 取 item
    CDataManager* dm = G_CDataManager();
    CItem* itemScript = dm->find_item(itemIdx);
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
    int roll = m_rand->randInt(range);
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
        __PRETTY_FUNCTION__,
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
    if (newUpgrade >= table->intAtConst(0x234))
    {
        _NoticeUpgrade(user, item, true);
    }

    unsigned char up2 = item.GetUpgrade();
    if (0xc < up2)
    {
        const char* characName = user->getCurCharacName();
        int characNo2 = user->getCurCharacNo();
        unsigned int itemId = ticket.m_addInfo;
        unsigned int envField = (unsigned int)G_CEnvironment()->m_serverEnvir.m_serverGroup;
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
    if (item->GetUpgrade() >= 0x1f)
        return 0;
    // ORIG 求值序（实参 R2L）：getGrowthGrade 命名局部先求值（-0x1c），
    // return 内联实参 R2L：GetItemType → GetUpgrade（第二次调用，P48 无 CSE）
    // → get_rarity；临时值跨调用存 callee-saved（P52）。
    int growthGrade = ((CEquipItem*)itemScript)->getGrowthGrade(level);
    return table.get_upgrade_info(
        out,
        static_cast<unsigned char>(itemScript->get_rarity()),
        static_cast<unsigned char>(growthGrade),
        item->GetUpgrade(),
        (ENUM_EQUIPMENTTYPE)itemScript->GetItemType());
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
    packet.put_byte(isAmplify);
    packet.put_short((int)user->get_unique_id());
    packet.put_int(item.m_addInfo);
    packet.put_byte((int)item.GetUpgrade());
    item.m_random.m_random.put_packet_random_option(packet);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}

// ============================================================================
// isDoEnchant @ 0x0854977a（isDoEnchant.md）：randInt(100)（经 this+0x4e8
//   CMTRand）与 buf[(item+0x1f & 1) ? 1 : 0]（char 符号扩展）比较；rand<=bufVal
//   返回 true。
// ============================================================================
bool CItemUpgrade::isDoEnchant(const Inven_Item& item, char* buf)
{
    unsigned long range = 100;
    int roll = m_rand->randInt(range);
    int bufVal = 0;
    if ((item.m_random.m_skill0.m_field & 1))
        bufVal = (signed char)buf[1];
    else
        bufVal = (signed char)buf[0];
    if (roll <= bufVal)
        return true;
    return false;
}

// ============================================================================
// ProcUpgrade(MSG_ENCHANT*) @ 0x0854a1ba（ProcUpgrade_0854a1ba.md）
//   三代强化（ENCHANT 消息）主流程。返回错误码；0 成功。
// ============================================================================
int CItemUpgrade::ProcUpgrade(CUser* user, MSG_ENCHANT* msg, PacketGuard* packet)
{
    if (user == 0 || msg == 0 || packet == 0)
        return 4;
    CInventory* invenW = user->getCurCharacInvenW();
    if (invenW == 0)
        return 4;

    Inven_Item slotA = invenW->GetInvenSlot(1, msg->m_0d);
    if (slotA.isEmpty())
        return 4;
    if (slotA.m_addInfo != msg->m_0f)
        return 4;

    Inven_Item material = invenW->GetInvenSlot(1, msg->m_13);
    if (material.isEmpty())
        return 0x16;

    int start = 0, end = 0;
    invenW->getStartEndPos((Inven_Item::ITEM_TYPE)0, start, end);
    if (msg->m_0d < start || msg->m_0d > end)
        return 0x13;

    CItem* itemScript = G_CDataManager()->find_item(slotA.m_addInfo);
    if (itemScript == 0)
        return 4;
    CItem* materialScript = G_CDataManager()->find_item(material.m_addInfo);
    if (materialScript == 0)
        return 4;

    int cond = _CheckCondition(user, slotA, itemScript, material, msg->m_19);
    if (cond != 0)
    {
        LogManager::logFormat(1, "ItemUpgrade.cpp",
            __PRETTY_FUNCTION__,
            0x756,
            "3RD_CHRONICLE_LOG : ENCHANT SYSTEM _CheckCondition() ERROR!! CHARAC NO(%u) ERROR(%u)",
            user->getCurCharacNo(), cond);
        return cond;
    }

    bool doEnchant = isDoEnchant(slotA, ((CStackableItem*)materialScript)->m_chars30d);

    if (!invenW->delete_item(INVEN_TYPE_EQUIP, msg->m_13, 1, (eItemDelReason)0xc, true))
        return 0x11;

    packet->put_byte(1);
    packet->put_short(msg->m_13);
    Inven_Item material2 = invenW->GetInvenSlot(1, msg->m_13);
    packet->put_short((int)(material2.m_addInfo & 0xffff));
    packet->put_byte(doEnchant ? 1 : 0);

    int ret = 0;
    if (doEnchant)
    {
        // ORIG CItem 虚表 +0xc（GetItemType）返回装备类型。
        ENUM_EQUIPMENTTYPE equipType =
            (ENUM_EQUIPMENTTYPE)itemScript->GetItemType();
        ret = ProcUpgrade_3rdChronicle(user, equipType, msg->m_19,
                                       slotA, materialScript);
        if (ret != 1)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x786, 5);
            trace("3RD_CHRONICLE_LOG : ENCHANT SYSTEM ProcUpgrade_3rdChronicle() ERROR!! CHARAC NO(%u)\n",
                  user->getCurCharacNo());
            return 1;
        }
    }
    else
    {
        int sellPrice = 0;
        G_Store()->GetSellItemPrice(slotA, itemScript, (short)msg->m_19, false,
                                    sellPrice);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0xf, user, (unsigned int)sellPrice);
        int penalty = _ProcPenalty(user, &slotA, itemScript, msg->m_0d, packet);
        if (penalty != 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x77e, 5);
            trace("3RD_CHRONICLE_LOG : ENCHANT SYSTEM _ProcPenalty() ERROR!! ERROR CODE(%d)!! CHARAC NO(%u)\n",
                  penalty, user->getCurCharacNo());
            return penalty;
        }
    }

    if (slotA.m_addInfo != 0)
        invenW->update_item(INVEN_TYPE_EQUIP, msg->m_0d, slotA);
    user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, msg->m_0d);

    int count = 0;
    const unsigned char* f = reinterpret_cast<const unsigned char*>(&slotA) + 0x1f;
    if ((f[0] & 1) != 0)  ++count;
    if ((f[1] & 1) != 0)  ++count;

    user->m_historyLog.Enchant3rdChronicleItem(doEnchant,
        itemScript->get_index(), material.m_addInfo, msg->m_19, count);
    return 0;
}

// ============================================================================
// _CheckCondition(ENCHANT 5 参) @ 0x085497ea（_CheckCondition_085497ea.md）
//   三代强化条件校验。返回错误码，0=通过。
// ============================================================================
int CItemUpgrade::_CheckCondition(const CUser* user, const Inven_Item& item,
                                  const CItem* itemScript, const Inven_Item& ticket,
                                  unsigned char param)
{
    // ORIG 0x085497ea 逐段转写（_CheckCondition_085497ea.md）。
    if (!item.m_amp.isIdentified())
        return 0xae;
    if (item.m_field1 != 1)
        return 0x13;
    if (itemScript->GetItemType() == 0xb)
        return 0x13;
    // P48 无 CSE：两臂各独立调用 GetItemType。
    if (itemScript->GetItemType() < 10 || itemScript->GetItemType() > 0x15)
        return 0x13;
    if (item.m_fieldb !=
        ((const CEquipItem*)itemScript)->get_endurance())
        return 7;

    // 空间检查：预测分解结果物品数 + 1，增幅再 +1。
    int need = G_CDataManager()->m_disJoint->getPredictResultItemTypeCount(
        itemScript->get_rarity());
    ++need;
    if (item.m_amp.hasAbility())
        ++need;
    // P48：InvenEmptycount/QuickEmptyCount 各自 getCurCharacInvenR。
    bool insufficient =
        need != 0 &&
        (unsigned int)(user->getCurCharacInvenR()->InvenEmptycount(
                           Inven_Item::ITEM_TYPE_3) +
                       user->getCurCharacInvenR()->QuickEmptyCount()) <
            (unsigned)need;
    if (insufficient)
        return 0x15;

    const CItem* materialScript =
        G_CDataManager()->find_item(ticket.m_addInfo);
    if (itemScript == 0)
        return 0x15;

    // 附魔槽（item+0x1f/0x20 = m_random.m_skill0/1.m_field）：双槽就绪或
    // 首槽就绪且类型/技能匹配材料脚本 +0x30c（P48：逐次重新读字段）。
    if ((item.m_random.m_skill0.m_field & 1) &&
        (item.m_random.m_skill1.m_field & 1))
        return 0x13;
    if ((item.m_random.m_skill0.m_field & 1) &&
        (item.m_random.m_skill0.m_field >> 3) == param &&
        ((item.m_random.m_skill0.m_field >> 1) & 3) ==
            (int)((const CStackableItem*)materialScript)->m_char30c)
        return 0x13;

    // 遍历装备附魔表匹配 job+growType（迭代器直构 P50，end 缓存 P13）。
    const CEquipItem* equip = (const CEquipItem*)itemScript;
    std::vector<STEnchantSystemData>::const_iterator it =
        equip->getEnchantSystemData().begin();
    std::vector<STEnchantSystemData>::const_iterator end =
        equip->getEnchantSystemData().end();
    bool matched = false;
    for (; it != end; ++it)
    {
        if (user->get_charac_job() == it->m_a &&
            (unsigned char)user->getCurCharFirstGrowType() == it->m_b)
        {
            matched = true;
            break;
        }
    }
    if (!matched)
        return 1;
    return 0;
}

// ============================================================================
// ProcUpgrade_3rdChronicle @ 0x0854a006（ProcUpgrade_3rdChronicle.md）
//   三代强化落点：按 key(job/growType/equipType/param) 查 itemScript+0x2f4 附魔表；
//   命中 → item+0x1f 或 item+0x20（按首字节 &1 二选一）写入附魔类型/槽位，返回 1；
//   未命中 → 0。
// ============================================================================
int CItemUpgrade::ProcUpgrade_3rdChronicle(CUser* user, ENUM_EQUIPMENTTYPE equipType,
                                           unsigned char param, Inven_Item& item,
                                           const CItem* itemScript)
{
    // ORIG key：job + growType + equipType + param 组合查 map（CStackableItem+0x2f4）。
    // 该 map 权威键类型 STEnchantSystemMapKey 仅含 m_field0（int），此处用
    // map<int,int>（CStackableItem::m_enchantMap）查 job 键近似。
    const std::map<int, int>* map2f4 =
        &static_cast<const CStackableItem*>(itemScript)->m_enchantMap;
    std::map<int, int>::const_iterator it = map2f4->find(user->get_charac_job());
    bool found = it != map2f4->end();

    unsigned char* f = &item.m_random.m_skill0.m_field;
    if ((f[0] & 1) == 0)
    {
        f[0] |= 1;
        f[0] = (unsigned char)((f[0] & 0xf9) |
                               ((param & 0x1f) * 8));
    }
    else
    {
        f[1] |= 1;
        f[1] = (unsigned char)((f[1] & 0xf9) |
                               ((param & 0x1f) * 8));
    }
    return found ? 1 : 0;
}

// ============================================================================
// _ProcPenalty(ENCHANT 5 参) @ 0x08549ac8（_ProcPenalty_08549ac8.md）
//   三代强化惩罚：删材料 → 分解 → 附加物品入包+插入事件物品+统计。返回错误码。
// ============================================================================
int CItemUpgrade::_ProcPenalty(CUser* user, Inven_Item* item,
                               const CItem* itemScript, unsigned short slot,
                               PacketGuard* packet)
{
    const stAmplifyOption_t& amp =
        item->m_amp;
    const upgrade_table_t* table = amp.hasAbility()
        ? GlobalData::s_itemAmplifier_->getUpgradeInfoTable()
        : static_cast<const upgrade_table_t*>(this);

    CInventory* invenW = user->getCurCharacInvenW();
    if (!invenW->delete_item(INVEN_TYPE_EQUIP, slot, 1, (eItemDelReason)0xd, true))
        return 0x11;

    unsigned int upgrade = (unsigned int)(unsigned char)item->GetUpgrade();
    packet->put_short(slot);

    DisJointResult djr;
    DisJoint* dj = G_CDataManager()->m_disJoint;
    int itemIdx = item->m_addInfo;
    if (dj->GetResult(itemIdx, djr) != 0)
    {
        packet->put_byte(0);
        return 0;
    }

    item->m_addInfo = 0;
    std::vector<std::pair<int, int> > addItems;
    addItems.reserve(6);
    for (int i = 0; i < 5; ++i)
    {
        int fi = reinterpret_cast<int*>(&djr)[4 + i];
        if (fi != 0)
            addItems.push_back(std::make_pair(fi, fi));
    }
    if (amp.hasAbility())
        _getAdditionalDisjointItem(upgrade, itemScript, addItems, *table);
    _getAdditionalDisjointItem(upgrade, itemScript, addItems, *table);

    std::vector<int> outSlots;
    int insertRet = invenW->insert_event_items(addItems, outSlots,
        (eMoneyAddReason)7, (eItemAddReason)0x13, false, "", "");
    packet->put_byte(insertRet);

    // ORIG outSlots 每 3 项一槽（slot/itemIdx/count 三连）。逐槽出品（reduce 精度受限）。
    if (insertRet != 0)
    {
        for (std::vector<int>::const_iterator it = outSlots.begin();
             it != outSlots.end(); it += 3)
        {
            int slotVal = *it;
            int idx = (it + 1 != outSlots.end()) ? *(it + 1) : 0;
            int cnt = (it + 2 != outSlots.end()) ? *(it + 2) : 0;
            packet->put_short(slotVal);
            if (it + 1 != outSlots.end())
                packet->put_item_idx((unsigned long)(idx & 0xffff));
            if (it + 2 != outSlots.end())
                packet->put_int(cnt);
        }
    }
    return 0;
}
}  // namespace WongWork
