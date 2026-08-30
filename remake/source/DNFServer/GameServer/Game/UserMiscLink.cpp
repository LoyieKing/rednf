// ============================================================================
// UserMiscLink.cpp —— LinkHelper.UserMiscLink 批次独占 TU。
// 13 个 ORIG 符号 + 直接连带依赖（TotalMission::rewardMission、CRedeemItem
// GetItemPrice/GetRedeemInfoR/RedeemItem/_CheckVaildIndex_N_GetIter、
// LevelupSupportEventManger::GetItemInfoVector）。全部符号逐份读
// docs/class_func_reports/ 对应报告转写；无报告者 objdump 按地址转写并标注。
// 不得编辑：Dispatchers.cpp/DisPatcherCore.cpp/CUser.cpp/CUser.h/ChecksumAlgo.h。
// ============================================================================
#include <cstring>
#include <algorithm>

#include "CUser.h"                          // CUser / m_guildDB / m_levelupEvents / m_historyLog / ENUM_ERROR
#include "CUserCharacInfo.h"                // setCurCharChangingGrowthType/getCurCharFirstGrowType 等
#include "CInventory.h"                     // CInventory::delete_item/insertItemIntoInventory/use_money/update_item、GetInvenTypeFromItemSpace
#include "Inven_Item.h"                     // Inven_Item / RandomOption
#include "CItem.h"                          // CItem::make_item/is_stackable
#include "CDataManager.h"                   // G_CDataManager / find_item / get_GrowthWeaponEventReward / GuildParameterScript
#include "SkillSlot.h"                      // SkillSlot::clear_all_skills_both/set_parent、WongWork::CSkillChanger、addSkillOnCreateCharacter
#include "GameTypes.h"                      // cMyTrace
#include "LogManager.h"                     // LogManager::logFormat
#include "PacketGuard.h"                    // PacketGuard
#include "InterfacePacketBuf.h"             // put_header/put_byte/put_int/finalize
#include "Redeem_Item__CRedeemItem.h"       // Redeem_Item::CRedeemItem
#include "CPrivateStoreMgr.h"               // private_store::CPrivateStoreMgr::IsBusyPrivateStore / GetInstancePrivateStoreMgr
#include "CMailBoxHelper.h"                 // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"        // g_scriptStringManager_ / findString
#include "DB_SaveEventLevelupSupport.h"     // DB_SaveEventLevelupSupport::makeRequest
#include "DB_EventUpdateGrowthWeaponTimepiece.h"  // DB_EventUpdateGrowthWeaponTimepiece::makeRequest
#include "GrowthEquipEvent.h"               // GrowthEquipEvent
#include "HeroMissionEvent.h"               // HeroMissionEvent / HeroMissionValue
#include "HeroMissionCondition.h"           // BaseHeroMissionCondition
#include "TotalMission.h"                   // TotalMission
#include "LevelupSupportEventManger.h"      // LevelupSupportEventManger
#include "RandomOptionScript.h"             // RandomOptionScript / CRandomOptionGive / OptionGroup
#include "CItemGenerator.h"                 // random_option::CRandomOptionItemHandle
#include "PvPTypes.h"                       // CMTRand
#include "GlobalData.h"                     // GlobalData::s_auction_proxy / s_cera_auction_proxy
#include "AuctionProxy.h"                   // CAuctionServerProxy / CCeraAuctionServerProxy::IsRunning
#include "PacketTypes.h"                    // MSG_QUICK_CHAT

// CUser.cpp:140 同型声明（ORIG T 0x08668823，CUser.cpp 定义）
extern void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow);

// CRandomList<unsigned int> —— ORIG W functor（_ZN11CRandomListIjEclEj 0x085f5b6a，
// 16 字节；与 RandomRentalA2.cpp 同型定义，模板定义可跨 TU 重复）。
template <typename T>
class CRandomList
{
public:
    explicit CRandomList(unsigned int n)
        : m_value(0), m_n(n), m_sum(0), m_count(0)
    {
    }

    void operator()(unsigned int weight)
    {
        if (m_sum > m_n)
        {
            return;
        }
        if (m_n - m_sum < weight)
        {
            m_value = m_count;
        }
        m_sum += weight;
        ++m_count;
    }
    void operator()(const OptionGroup& og)
    {
        (*this)(static_cast<unsigned int>(og.m_weight4));
    }

    // ORIG _ZN11CRandomListIjE9get_valueEv W @0x085f49a0（非 const，直读 +0）
    unsigned int get_value() { return m_value; }

private:
    unsigned int m_value;   // +0x00
    unsigned int m_n;       // +0x04
    unsigned int m_sum;     // +0x08
    unsigned int m_count;   // +0x0c
};

// ===========================================================================
// 1) CUser::ChangeGrowType_Item(int, short) —— ORIG T 0x0867b196
//    报告 docs/class_func_reports/CUser/ChangeGrowType_Item.md 全文转写。
// ===========================================================================
int CUser::ChangeGrowType_Item(int newFirst, short slotNo)
{
    if (CUserCharacInfo::getCurCharacR() == 0)
    {
        return 1;
    }

    char firstGrow = CUserCharacInfo::getCurCharFirstGrowType();
    if (firstGrow == 0 || firstGrow == (char)newFirst)
    {
        return 0x13;
    }

    CUserCharacInfo::setCurCharChangingGrowthType((char)newFirst, -1);
    getCurCharacInvenW()->delete_item((INVEN_TYPE)1, (int)slotNo, 1,
                                      (eItemDelReason)0xa, true);
    ((SkillSlot*)CUserCharacInfo::getCurCharacSkillW())->clear_all_skills_both();
    ((SkillSlot*)CUserCharacInfo::getCurCharacSkillW())->set_parent(this);

    WongWork::CSkillChanger changer;
    changer.SkillInitialize(this, 0, true);
    int job = CUserCharacInfo::get_charac_job();
    int level = CUserCharacInfo::get_charac_level();
    addSkillOnCreateCharacter((_Mastered_skill*)(getCurCharacSkillW() + 0x46), job);
    addSkillOnCreateCharacter((_Mastered_skill*)(getCurCharacSkillW() + 0x1de), job);

    int beforeGrow = (int)CUserCharacInfo::getCurCharFirstGrowType();
    unsigned char secondGrow = (unsigned char)CUserCharacInfo::getCurCharSecondGrowType();
    set_grow_type((unsigned char)newFirst, secondGrow, 0,
                  (eChangeGrowTypeReason)1);
    adjust_charac_stat();
    unsigned int characNo = (unsigned int)get_charac_no(-1);
    reset_tag_info_to_me(characNo);
    _postCheckForceChangeGrowType(this, newFirst,
                                  (int)CUserCharacInfo::getCurCharSecondGrowType());
    SendNotiPacket((eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
    send_skill_info();
    SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)0, (int)slotNo);

    cMyTrace trace(__PRETTY_FUNCTION__, 0x756d, 0);
    trace("[USE GROWTYPE ITEM]characNo(%d) before(%d), after(%d)",
          (unsigned int)get_charac_no(-1), beforeGrow, newFirst);
    return 0;
}

// ===========================================================================
// 2) CUser::IsAddableGuildFund(int) —— ORIG T 0x0866c21e
//    报告 CUser/IsAddableGuildFund.md：fundLevel(+0x8d03b)<=0xf 时上限
//    20000000(0x1312d00)，否则 10000000(0x989680)。
// ===========================================================================
bool CUser::IsAddableGuildFund(int amount)
{
    unsigned int limit = (m_guildDB.m_fundLevel > 0xf) ? 10000000u : 20000000u;
    return (unsigned int)(m_guildDB.m_fundMoney + amount) <= limit;
}

// ===========================================================================
// 3) CUser::Redeem_Item(int) —— ORIG T 0x08646df2
//    报告 CUser/Redeem_Item.md 全文转写。
// ===========================================================================
unsigned int CUser::Redeem_Item(int redeemId)
{
    // GetCharacExpandData(6) 即 Redeem_Item::CRedeemItem（槽位 6，CUser.cpp 同型 cast）
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (redeem == 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(this))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x529, 5);
        trace("redeem error : trade : m_id(%s)charac_no(%u)",
              NumberToString(get_acc_id(), 0), (unsigned int)get_charac_no(-1));
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (CheckInTrade())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x52f, 5);
        trace("redeem error : trade : m_id(%s)charac_no(%u)",
              NumberToString(get_acc_id(), 0), (unsigned int)get_charac_no(-1));
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    int price = redeem->GetItemPrice(redeemId);
    if (price < 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (CUserCharacInfo::getCurCharacMoney() < price)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 0x46);
        return 0;
    }

    Redeem_Item::Inven_Item_Expand_RedeemInfo info = redeem->GetRedeemInfoR(redeemId);
    // info 前 0x3d 字节即 Inven_Item 快照（AddRedeemList 复制口径）；m_item 为
    // 真实 Inven_Item 成员（pack(1) 布局一致；ORIG 直接以 local_14 指针消费）。
    Inven_Item& item = *reinterpret_cast<Inven_Item*>(&info.m_item);
    if (item.isEmpty())
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    int slot = CUserCharacInfo::getCurCharacInvenW()->insertItemIntoInventory(
        item, (eItemAddReason)0x29, true, true);
    if (slot < 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 4);
        return 0;
    }

    CUserCharacInfo::getCurCharacInvenW()->use_money(price, (eMoneySubReason)0x25, true);

    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    // 非堆叠 → 历史记录 count=1；堆叠 → count=item.m_addInfo2(+0x07)。
    if (pItem == 0 || !pItem->is_stackable())
    {
        m_historyLog.RedeemItemDel(item.m_addInfo, 1);
    }
    else
    {
        m_historyLog.RedeemItemDel(item.m_addInfo, item.m_addInfo2);
    }

    redeem->RedeemItem(redeemId);

    PacketGuard packet;
    packet.put_header(1, 0x137);
    packet.put_byte(1);
    packet.put_int(slot);
    packet.finalize(true);
    Send(packet);
    return 1;
}

// ===========================================================================
// 4) CUser::get_pvp_exp_of_week() —— ORIG W 0x08230100（报告缺失，objdump 转写）
//    PvpResultType +0x08=m_expPoint、+0x10=m_field10（周重置基准）：
//    field10==-1 → expPoint，否则 expPoint - field10。
// ===========================================================================
int CUser::get_pvp_exp_of_week()
{
    const PvpResultType* result = CUserCharacInfo::getPVPResultRefR();
    if (result->m_field10 == -1)
    {
        return result->m_expPoint;
    }
    return result->m_expPoint - result->m_field10;
}

// ===========================================================================
// 5) GrowthEquipEvent::SendInfinityGrowthWeapon —— ORIG T 0x080f74b2
//    报告 GrowthEquipEvent/SendInfinityGrowthWeapon.md 全文转写。
// ===========================================================================
int GrowthEquipEvent::SendInfinityGrowthWeapon(CUser* user, const Inven_Item* item,
                                               int space, int slot) const
{
    int rewardIdx = G_CDataManager()->get_GrowthWeaponEventReward(item->m_addInfo);
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    if (rewardIdx == 0)
    {
        return 0x17;
    }

    Inven_Item newItem;
    CItem* pItem = G_CDataManager()->find_item(rewardIdx);
    if (pItem != 0)
    {
        pItem->make_item(newItem);                       // vtable 槽 +0x08
        newItem.SetUpgrade(item->GetUpgrade());
        newItem.set_add_info(item->get_add_info());
        // ORIG 0x080f7572..0x080f759c：仅从源 item 覆盖
        //   +0x33 低 5 位（UpgradeSeparate）、+0xd（m_fieldd）、+0x11（m_amp 前 4 字节）。
        newItem.m_upgradeSep.m_field =
            (unsigned char)((newItem.m_upgradeSep.m_field & 0xe0) |
                            (item->m_upgradeSep.m_field & 0x1f));
        newItem.m_fieldd = item->m_fieldd;
        std::memcpy(&newItem.m_amp, &item->m_amp, 4);
    }

    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)invenType, slot, 1, (eItemDelReason)0x36, true))
    {
        return 0x11;
    }

    user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)space, slot);
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)invenType, slot, newItem);
    user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)space, slot);

    char* data = static_cast<char*>(user->getGrowthWeaponEventdata());
    short timepiece = *reinterpret_cast<short*>(data + 0x10);
    DB_EventUpdateGrowthWeaponTimepiece::makeRequest(user->get_acc_id(), timepiece,
                                                     rewardIdx);
    *reinterpret_cast<int*>(data + 0xc) = rewardIdx;
    return 0;
}

// ===========================================================================
// 6) GuildParameterScript::getGuildCargoUpradeInfo(int) —— ORIG T 0x08979726
//    报告 GuildParameterScript/getGuildCargoUpradeInfo.md（拼写 Uprade 为
//    ORIG mangled 原样）：m_vec38 线性查找首字段==capacity。
// ===========================================================================
STGuildStorageUpgradeInfo* GuildParameterScript::getGuildCargoUpradeInfo(int capacity)
{
    for (std::vector<STGuildStorageUpgradeInfo>::iterator it = m_vec38.begin();
         it != m_vec38.end(); ++it)
    {
        STGuildStorageUpgradeInfo* v = &*it;   // ORIG：de 结果存局部
        if (v->m_field0 == capacity)
        {
            return v;
        }
    }
    return 0;
}

// ===========================================================================
// 7) HeroMissionEvent::rewardMission(CUser*, MissionNo::T) —— ORIG T 0x0816748e
//    报告 HeroMissionEvent/rewardMission.md 全文转写。
//    ORIG 入口经 vptr+0x34（CEventBase::IsEventing）判活；本头为布局镜像类，
//    等价读 m_isEventing（头注释口径）。
// ===========================================================================
bool HeroMissionEvent::rewardMission(CUser* user,
                                     HeroMissionCondition::MissionNo::T mission)
{
    if (!m_isEventing)
    {
        return false;
    }

    std::map<HeroMissionCondition::MissionNo::T, BaseHeroMissionCondition*>::iterator it =
        m_conditions.find(mission);
    if (it == m_conditions.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x2df, "[HeroMission] Invalid mission. (charac:%u, mission:%d)",
            (unsigned int)user->get_charac_no(-1), (int)mission);
        user->DisConnSig((DISCONN_SIG)0x3e, true, 0x2e0);
        return false;
    }

    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x2d8, "[HeroMission] User is null.");
        return false;
    }

    bool rewarded = false;
    bool advance = false;
    HeroMissionValue* value = 0;
    if (findMissionValue(user, mission, value))
    {
        BaseHeroMissionCondition* cond = it->second;
        if (cond->getMissionType() == 3)
        {
            // missionType==3 的条件对象即 TotalMission（ORIG ctor 写 vptr
            // 0x8b7f048 实证）；m_values.find 直接消费（ORIG 无 end 检查）。
            std::map<unsigned int, std::vector<HeroMissionValue> >::iterator vit =
                m_values.find((unsigned int)user->get_charac_no(-1));
            rewarded = static_cast<TotalMission*>(cond)->rewardMission(
                user, *value, vit->second);
        }
        else
        {
            unsigned short before = value->m_fieldA;
            rewarded = cond->rewardMission(user, *value);   // vtable 槽 +0x0c
            if (before == 0 && value->m_fieldA != 0)
            {
                advance = true;
            }
        }
    }

    if (advance)
    {
        processMission(user, (HeroMissionCondition::MissionNo::T)6, 1);
    }
    return rewarded;
}

// ===========================================================================
// 7a) TotalMission::rewardMission —— 两参版 ORIG T 0x081663f4（报告全文：
//     函数体恒返回 1）；三参版 ORIG T 0x081663fe（rewardMission_081663fe.md）。
// ===========================================================================
bool TotalMission::rewardMission(CUser* /*user*/, HeroMissionValue& /*value*/)
{
    return true;
}

bool TotalMission::rewardMission(CUser* user, HeroMissionValue& value,
                                 std::vector<HeroMissionValue>& values)
{
    // 非重复任务且当前 value 已完成 → 不再发奖。
    if (!isRepeatMission() || value.m_fieldA != 0)
    {
        return false;
    }

    // 全部子任务（m_mission==6 或已完成）均达成才发奖。
    bool allDone = true;
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        HeroMissionValue& cur = values[i];
        if (cur.m_mission != 6 && cur.m_fieldA == 0)
        {
            allDone = false;
            break;
        }
    }
    if (!allDone)
    {
        return false;
    }

    bool ok = rewardProcess(user);
    value.m_fieldA = (unsigned short)(value.m_fieldA + 1);
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
}

// ===========================================================================
// 8/9/9a) LevelupSupportEventManger —— GetStepIndex 0x08147362 /
//         InsertItem 0x081473cc / GetItemInfoVector 0x08147724
// ===========================================================================
int LevelupSupportEventManger::GetStepIndex(int requestLevel)
{
    std::map<int, int>::iterator it = m_stepIndexMap.find(requestLevel);
    if (it == m_stepIndexMap.end())
    {
        return -1;
    }
    return it->second;
}

std::vector<LevelupSupportSciprt::ItemInfo>*
LevelupSupportEventManger::GetItemInfoVector(int level)
{
    std::map<int, std::vector<LevelupSupportSciprt::ItemInfo>*>::iterator it =
        m_script.m_stepItems.find(level);
    if (it == m_script.m_stepItems.end())
    {
        return 0;
    }
    return it->second;
}

void LevelupSupportEventManger::InsertItem(CUser* user, int eventIdx, ENUM_ERROR& err)
{
    LevelupSupportEventManger::EventInfo& info = user->m_levelupEvents[eventIdx];
    info.m_state = 2;

    // ORIG：内部经 Singleton 重取实例再调（0x08147413）；ORIG 以 const 指针遍历
    //（at/size 用 const 重载），此处 const 指针与 ORIG 同形。
    const std::vector<LevelupSupportSciprt::ItemInfo>* items =
        ARAD::Singleton<LevelupSupportEventManger>::Get()->GetItemInfoVector(
            (int)info.m_level);
    if (items == 0)
    {
        err = (ENUM_ERROR)0x15;
        return;
    }

    for (std::size_t i = 0; i < items->size(); ++i)
    {
        const LevelupSupportSciprt::ItemInfo& itemInfo = items->at(i);
        CItem* pItem = G_CDataManager()->find_item(itemInfo.m_itemIndex);
        if (pItem == 0)
        {
            err = (ENUM_ERROR)0x15;
            ARAD::PRINT_ERROR::find_item(
                (char*)" LevelupSupportEventManger::InsertItem", user,
                itemInfo.m_itemIndex);
            return;
        }

        Inven_Item item;
        // ORIG：make_item 前先置 m_addInfo(+0x02) = get_index()（0x081474c9）
        item.m_addInfo = pItem->get_index();
        pItem->make_item(item);
        item.set_add_info((int)itemInfo.m_itemCount);

        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)0xd, true, true);
        if (slot < 0)
        {
            // 背包不足 → 邮件补发（game_server_msg_241 sender / 242 title）。
            const char* sender = g_scriptStringManager_.findString(
                4, "game_server_msg_241", 0);
            const char* title = g_scriptStringManager_.findString(
                4, "game_server_msg_242", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                sender, item, 0,
                (unsigned int)user->getCurCharacNo(), title,
                (int)std::strlen(title), 0xf,
                user->GetServerGroup(), false, false);
        }
        else
        {
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    DB_SaveEventLevelupSupport::makeRequest((int)user->GetUID(),
                                            (int)user->getCurCharacNo(),
                                            (int)info.m_level, 2);
    LevelupSupportEventManger::SendCharacState(user, 0);
}

// ===========================================================================
// 10) random_option::CRandomOptionItemHandle::set_random_option_give ——
//     ORIG T 0x085f0ea2（报告 3 行：m_optionSet(+0x14) = give）。
// ===========================================================================
namespace random_option
{

void CRandomOptionItemHandle::set_random_option_give(CRandomOptionGive* give)
{
    m_optionSet = give;
}

// ===========================================================================
// 11) random_option::CRandomOptionItemHandle::change_option —— ORIG T 0x085f3340
//     报告 change_option.md 全文转写。参数语义（汇编栈序实证）：
//       itemIdx、rarity、level(→_get_option_type 第3参/_choose 的 optTypeByte)、
//       grade(→_choose 的 grade)、group(→_get_option_type 第4参/_choose 的 level)、
//       slot(0..2；排除第一组已占槽 + seed2 低 2 位)、opts。
//     读取第一组三槽（+0/+3/+6），把命中组写入第二组槽（+0xa..+0xc）与
//     seed2(+0xd)；随机池 m_rand[9]（+0x5808）/m_rand[10]（+0x61d0）。
// ===========================================================================
void* GetRandomOption();   // GlobalData.cpp 定义（T 0x085f0eb0：&G_CDataManager()->m_randomOption）

bool CRandomOptionItemHandle::change_option(unsigned long itemIdx, ENUM_RARITY rarity,
                                            int level, int grade, int group,
                                            int slot, RandomOption* opts)
{
    if (slot < 0 || slot > 2)
    {
        return false;
    }


    unsigned char optType = 0;
    int err = _get_option_type(itemIdx, rarity, level, group, optType);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x356, "_get_option_type (%d,%d,%d,%d,%d) error_line(%d)",
            (int)itemIdx, (int)(unsigned char)rarity, level, group,
            (int)optType, err);
        return false;
    }

    int outGroup = 0;
    err = _choose_option_group(rarity, group, grade,
                               (unsigned char)level, optType, outGroup);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x35d, "_choose_option_group (%d,%d,%d,%d,%d,%d) error_line(%d)",
            (int)(unsigned char)rarity, group, grade, level,
            (int)optType, outGroup, err);
        return false;
    }

    // 不同权重表（GetRandomOption()+0xf4 = RandomOptionScript::m_mapf4）：
    // 键 = 低字节 rarity | 高字节 optType（asm -0x6e/-0x6d 实证，give 同型）。
    RandomOptionScript* script =
        reinterpret_cast<RandomOptionScript*>(GetRandomOption());
    unsigned short dwRaw =
        (unsigned short)(((unsigned short)(unsigned char)rarity) |
                         ((unsigned short)optType << 8));
    std::map<short, DifferentWeightValue>::iterator dw = script->m_mapf4.find((short)dwRaw);
    if (dw == script->m_mapf4.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x369, "different_weight_ (%d,%d) ",
            (int)(unsigned char)rarity, (int)optType);
        return false;
    }
    int dwA = dw->second.m_field0;
    int dwB = dw->second.m_field4;

    // 组值表（GetRandomOption()+0x160 = m_map160）。
    std::map<int, OptionGroupValue>::iterator ogIt = script->m_map160.find(outGroup);
    if (ogIt == script->m_map160.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x371, "option_group_ (%d) ", outGroup);
        return false;
    }

    int remaining = ogIt->second.m_fieldC;
    std::vector<OptionGroup> groupVec = ogIt->second.m_vec0;   // 值拷贝后 erase
    unsigned char* optB = reinterpret_cast<unsigned char*>(opts);

    // 第一组已占槽（非本次 slot）按其组类型扣权重并移出候选。
    for (int i = 0; i <= 2; ++i)
    {
        if (i == slot || optB[i * 3] == 0)
        {
            continue;
        }
        for (std::vector<OptionGroup>::iterator it = groupVec.begin();
             it != groupVec.end(); ++it)
        {
            if ((unsigned int)it->m_type0 == (unsigned int)optB[i * 3])
            {
                remaining -= it->m_weight4;
                groupVec.erase(it);
                break;
            }
        }
    }

    unsigned long bound = (unsigned long)remaining;
    int pick = m_rand[9].randInt(bound);          // m_rand+0x5808 → 池[9]

    CRandomList<unsigned int> chooser((unsigned int)pick);
    std::for_each(groupVec.begin(), groupVec.end(), chooser);

    unsigned int sel = chooser.get_value();
    if (sel < groupVec.size())
    {
        OptionGroup& og = groupVec[sel];
        optB[0xa] = (unsigned char)og.m_type0;    // 第二组槽首字节=组类型
        optB[0xb] = (unsigned char)_get_option_value(dwA, dwB);
        optB[0xc] = (unsigned char)_get_option_value(dwA, dwB);
        optB[0xd] = (unsigned char)((optB[0xd] & 0xfc) | (slot & 3));
        unsigned long seedBound = 0x40;
        int seedRoll = m_rand[10].randInt(seedBound);  // m_rand+0x61d0 → 池[10]
        optB[0xd] = (unsigned char)((optB[0xd] & 3) | ((seedRoll & 0x3f) << 2));
    }
    else
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x390, "_option_group.size() %d, %d ", sel,
            (unsigned int)groupVec.size());
        return false;
    }
    return true;
}

}  // namespace random_option

// ===========================================================================
// 12) IsAuctionServerDisconnected(unsigned char) —— ORIG T 0x082138f2
//     （报告缺失，objdump 转写）：payType==0 检查 GlobalData::s_auction_proxy、
//     payType==1 检查 GlobalData::s_cera_auction_proxy；对应代理未运行 → true。
// ===========================================================================
bool IsAuctionServerDisconnected(unsigned char payType)
{
    if (payType == 0)
    {
        if (!GlobalData::s_auction_proxy->IsRunning())
        {
            return true;
        }
    }
    if (payType == 1)
    {
        if (!GlobalData::s_cera_auction_proxy->IsRunning())
        {
            return true;
        }
    }
    return false;
}

// ===========================================================================
// 13) MSG_QUICK_CHAT::isLastMessage() —— ORIG W 0x0822dc98（报告缺失，objdump：
//     返回 m_11 == 1）。声明 PacketTypes.h:1106（HdrFieldA 域），定义外联至此。
// ===========================================================================
bool MSG_QUICK_CHAT::isLastMessage()
{
    return m_11 == 1;
}

// ===========================================================================
// 3a) Redeem_Item::CRedeemItem 四方法 —— Redeem_Item 直接连带依赖
//     （报告 Redeem_Item__CRedeemItem/{GetItemPrice,GetRedeemInfoR,RedeemItem,
//      _CheckVaildIndex_N_GetIter}.md 全文转写）。
// ===========================================================================
namespace Redeem_Item
{

std::list<Inven_Item_Expand_RedeemInfo>::const_iterator
CRedeemItem::_CheckVaildIndex_N_GetIter(int idx) const
{
    // 越界（idx<0 / 空 / idx+1>size）→ end()；否则 begin()+idx。
    if (idx < 0 || m_redeemList.empty() ||
        (std::size_t)(idx + 1) > m_redeemList.size())
    {
        return m_redeemList.end();
    }
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it = m_redeemList.begin();
    std::advance(it, idx);
    return it;
}

int CRedeemItem::GetItemPrice(int idx)
{
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it =
        _CheckVaildIndex_N_GetIter(idx);
    if (it == m_redeemList.end())
    {
        return -1;
    }
    return it->m_count40;
}

Inven_Item_Expand_RedeemInfo CRedeemItem::GetRedeemInfoR(int idx) const
{
    Inven_Item_Expand_RedeemInfo out;   // ORIG ctor 清零副本
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it =
        _CheckVaildIndex_N_GetIter(idx);
    if (it != m_redeemList.end())
    {
        std::memcpy(&out, &*it, 0x44);
    }
    return out;
}

Inven_Item_Expand_RedeemInfo CRedeemItem::RedeemItem(int idx)
{
    Inven_Item_Expand_RedeemInfo out;   // ORIG ctor 清零副本
    alter();                            // charac_expand::CData::alter 置脏

    // ORIG 以 const_iterator 定位 idx 后 advance 到非常量迭代器再 erase。
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator cit =
        _CheckVaildIndex_N_GetIter(idx);
    std::list<Inven_Item_Expand_RedeemInfo>::iterator it = m_redeemList.begin();
    std::advance(it, std::distance<std::list<Inven_Item_Expand_RedeemInfo>::const_iterator>(
                         m_redeemList.begin(), cit));
    if (it != m_redeemList.end())
    {
        std::memcpy(&out, &*it, 0x44);
        m_redeemList.erase(it);
    }
    return out;
}

}  // namespace Redeem_Item

// P147 显式实例化：发射 ORIG W _ZN11CRandomListIjE9get_valueEv @0x085f49a0。
template unsigned int CRandomList<unsigned int>::get_value();
