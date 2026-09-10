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
    // ORIG 形参按值（_ZN11CRandomListIjEclE11OptionGroup，无 RK 前缀）。
    // [L31 round2] ORIG 0x085f58d2（sz 0x58 = 34 条）**不是转发**，而是与
    // unsigned 版相同的体**在本函数内联展开**：`mov 0x8(%eax),%edx; mov 0x4(%eax),%eax;
    // cmp; ja` 起手，权重直接两次读形参槽 `0x10(%ebp)`（P0：无具名局部）。旧码
    // `(*this)(static_cast<unsigned int>(og.m_weight4));` 让 4.4 发 call（我方仅 10 条）。
    void operator()(OptionGroup og)
    {
        if (m_sum > m_n)
        {
            return;
        }
        if (m_n - m_sum < (unsigned int)og.m_weight4)
        {
            m_value = m_count;
        }
        m_sum += (unsigned int)og.m_weight4;
        ++m_count;
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

    // [b23S3C] P48/P0：ORIG 双臂各重取 getCurCharFirstGrowType（+0x39/+0x48 两 call，无 firstGrow 槽）
    if (CUserCharacInfo::getCurCharFirstGrowType() == 0 ||
        CUserCharacInfo::getCurCharFirstGrowType() == newFirst)
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
    addSkillOnCreateCharacter((_Mastered_skill*)((int)getCurCharacSkillW() + 0x46), job);
    addSkillOnCreateCharacter((_Mastered_skill*)((int)getCurCharacSkillW() + 0x1de), job);

    int beforeGrow = (int)CUserCharacInfo::getCurCharFirstGrowType();
    // [b23S3C] P183/P0：ORIG secondGrow 无槽，movzbl %al 直入 0x8(%esp) 实参
    set_grow_type((unsigned char)newFirst,
                  (unsigned char)CUserCharacInfo::getCurCharSecondGrowType(), 0,
                  (eChangeGrowTypeReason)1);
    adjust_charac_stat();
    // [b28d-solo-UserMiscLink] ORIG 0x0867b36b：get_charac_no 结果直入实参，无具名局部
    reset_tag_info_to_me((unsigned int)get_charac_no(-1));
    _postCheckForceChangeGrowType(this, newFirst,
                                  (int)CUserCharacInfo::getCurCharSecondGrowType());
    SendNotiPacket((eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
    send_skill_info();
    SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)0, (int)slotNo);

    // [b28d-solo-UserMiscLink] P227：ORIG 0x0867b428 在 cMyTrace ctor 之前求 get_charac_no
    // 并以 %ebx 跨 ctor 保活 ⇒ 全表达式临时链（具名 trace 会退化为 ctor 后求值 + 槽漂移）
    cMyTrace(__PRETTY_FUNCTION__, 0x756d, 0)(
        "[USE GROWTYPE ITEM]characNo(%d) before(%d), after(%d)",
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
    // [b28d-solo-UserMiscLink] ORIG 0x0866c21e：两个具名指针局部（-0x8 资金/-0x4 等级，
    // P0 槽往返证据）+ if/else 双分支各自比较；两个 return 0 独立块、return 1 共享尾块。
    unsigned int* pFund = &m_guildDB.m_fundMoney;
    unsigned char* pLevel = &m_guildDB.m_fundLevel;
    if (*pLevel > 0xf)
    {
        if (*pFund + amount > 10000000u)
            return 0;
    }
    else
    {
        if (*pFund + amount > 20000000u)
            return 0;
    }
    return 1;
}

// ===========================================================================
// 3) CUser::Redeem_Item(int) —— ORIG T 0x08646df2
//    报告 CUser/Redeem_Item.md 全文转写。
// [SIGFIX:CUser::Redeem_Item] ORIG __PRETTY_FUNCTION__ 串为 bool（Main 117 件
// 清单）；返回值仅 0/1，CUser.h:3537 已声明 bool ⇒ 定义同步（mangled 不含返回类型）。
// ===========================================================================
bool CUser::Redeem_Item(int redeemId)
{
    // [b28d-solo-UserMiscLink] 判词（N 封顶）：301/293、p167 CLEAR(44=44)；
    // 残差=①4.4 局部槽基整体深 4（redeem -0x1c vs -0x18、info -0xd0 vs -0xcc；
    // 引用改指针无效）⇒ N95/N82；②4.4 位折叠 `(unsigned)price>>31` 折成 cmpl/jns
    // （ORIG `shr $0x1f; test %al,%al`）⇒ N6/N99。P198① 预置 -1 与 P221 trace 临时链已复现。
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
        // [b28d-solo-UserMiscLink] P221/P227：两个互斥分支的 cMyTrace 改全表达式临时链，
        // 4.4 具名对象恒「先声明深→浅」与 ORIG（-0x7c/-0x6c）相反。
        cMyTrace(__PRETTY_FUNCTION__, 0x529, 5)(
            "redeem error : trade : m_id(%s)charac_no(%u)",
            NumberToString(get_acc_id(), 0), (unsigned int)get_charac_no(-1));
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (CheckInTrade())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x52f, 5)(
            "redeem error : trade : m_id(%s)charac_no(%u)",
            NumberToString(get_acc_id(), 0), (unsigned int)get_charac_no(-1));
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    // [b28d-solo-UserMiscLink] ORIG 0x08646f97：`movl $0xffffffff,-0x14` 预置 +
    // 独立赋值（P198①）；负值判定为 `shr $0x1f,%eax; test %al,%al; je`（4.1 无符号移位形）。
    int price = -1;
    price = redeem->GetItemPrice(redeemId);
    if (((unsigned int)price >> 31) != 0)
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
    // 真实 Inven_Item 成员（pack(1) 布局一致；ORIG 以具名指针 local_14(-0x10) 消费，
    // 引用形会多占一槽使全帧深 4）。
    Inven_Item* item = reinterpret_cast<Inven_Item*>(&info.m_item);
    if (item->isEmpty())
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    int slot = CUserCharacInfo::getCurCharacInvenW()->insertItemIntoInventory(
        *item, (eItemAddReason)0x29, true, true);
    if (slot < 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 4);
        return 0;
    }

    CUserCharacInfo::getCurCharacInvenW()->use_money(price, (eMoneySubReason)0x25, true);

    CItem* pItem = G_CDataManager()->find_item(item->m_addInfo);
    // 非堆叠 → 历史记录 count=1；堆叠 → count=item.m_addInfo2(+0x07)。
    if (pItem == 0 || !pItem->is_stackable())
    {
        m_historyLog.RedeemItemDel(item->m_addInfo, 1);
    }
    else
    {
        m_historyLog.RedeemItemDel(item->m_addInfo, item->m_addInfo2);
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
    // ORIG 0x08230100：getPVPResultRefR 每字段访问各取一次（P48 无缓存，
    // 结果经 %ebx 跨字段读）。ORIG `cmp $-1; jne 后置减形岛` + fall expPoint。
    // 【b25c】4.4 对 if(call链值==K) 恒物化 sete（探针 probe_tern2 u1），
    // 三目形才直分支（u3 与 ORIG 逐条同构）。
    return CUserCharacInfo::getPVPResultRefR()->m_field10 == -1
               ? CUserCharacInfo::getPVPResultRefR()->m_expPoint
               : CUserCharacInfo::getPVPResultRefR()->m_expPoint
                     - CUserCharacInfo::getPVPResultRefR()->m_field10;
}

// ===========================================================================
// 5) GrowthEquipEvent::SendInfinityGrowthWeapon —— ORIG T 0x080f74b2
//    报告 GrowthEquipEvent/SendInfinityGrowthWeapon.md 全文转写。
// ===========================================================================
int GrowthEquipEvent::SendInfinityGrowthWeapon(CUser* user, const Inven_Item* item,
                                               int space, int slot) const
{
    // [b28d-solo-UserMiscLink] 判词（N 封顶）：166/172、p167 CLEAR(20=20)；
    // 残差=①4.1 栈槽重叠——rewardIdx 落 newItem.m_addInfo 槽 -0x4b（4.4 独立 -0x14），
    // 4.4 不可能重叠 ⇒ 结构性 N；②4.4 帧量化 0xa4 vs ORIG 0xa0（P190 附注）；
    // ③rewardIdx 未入 %esi 保活（P226 语句表达式实测无效）；④N7 间接 call %eax vs %edx。
    // 待查（推断）：ORIG 末尾 makeRequest/写回若读重叠槽，实参为 newItem.m_addInfo 而非 rewardIdx。
    // ORIG 序言 +0x00b 无条件构造 Inven_Item（先于 reward 判定）——声明即构造位置
    Inven_Item newItem;
    int rewardIdx = G_CDataManager()->get_GrowthWeaponEventReward(item->m_addInfo);
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    if (rewardIdx == 0)
    {
        return 0x17;
    }

    CItem* pItem = G_CDataManager()->find_item(rewardIdx);
    if (pItem != 0)
    {
        pItem->make_item(newItem);                       // vtable 槽 +0x08
        newItem.SetUpgrade(item->GetUpgrade());
        newItem.set_add_info(item->get_add_info());
        // ORIG 0x080f7572..0x080f759c：仅从源 item 覆盖
        //   +0x33 低 5 位（UpgradeSeparate）、+0xd（m_fieldd）、+0x11（m_amp 前 4 字节）。
        // [b28d-solo-UserMiscLink] ORIG 0x080f7572：源侧先读（and $0x1f 后 mov+and 冗余），
        // 目标侧后读；操作数交换即复现读序。
        newItem.m_upgradeSep.m_field =
            (unsigned char)((item->m_upgradeSep.m_field & 0x1f) |
                            (newItem.m_upgradeSep.m_field & 0xe0));
        // ORIG 0x080f758d 先 m_amp(+0x11) 后 m_fieldd(+0xd)；P235 匿名 union
        // 的 m_amp0Int 具名视角产 `mov 0x11(%eax),%eax; mov %eax,-0x3c(%ebp)` 直拷。
        newItem.m_amp.m_amp0Int = item->m_amp.m_amp0Int;
        newItem.m_fieldd = item->m_fieldd;
    }

    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)invenType, slot, 1, (eItemDelReason)0x36, true))
    {
        return 0x11;
    }

    user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)space, slot);
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)invenType, slot, newItem);
    user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)space, slot);

    // [b28d-solo-UserMiscLink] P200：short 表达式作普通实参直传（movswl %ax,%ebx），
    // 禁具名 timepiece 局部（会退化为 mov %ax,-N; movswl -N,%ebx 两段）；
    // P52/P48：rewardIdx 先入 %esi 跨 get_acc_id 保活。
    DB_EventUpdateGrowthWeaponTimepiece::makeRequest(
        user->get_acc_id(),
        static_cast<GrowthWeaponEvent*>(user->getGrowthWeaponEventdata())->m_field10,
        ({ rewardIdx; }));
    static_cast<GrowthWeaponEvent*>(user->getGrowthWeaponEventdata())->m_fieldc =
        (unsigned int)rewardIdx;
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
    // [b28d-solo-UserMiscLink] 判词（N 封顶）：166=166 条、p167 CLEAR(17=17)；
    // 残差=①4.4 槽分配方向（characNo 占 -0x24 而 ORIG -0x18，连带 value/it/end 各浅 4；
    // P212 全头部声明实测 181 条反劣、P50 先声明后赋值无效）⇒ N82/N98 槽序；
    // ②N7 间接 call 目标 %eax(4.4.7) vs %edx(4.1)。
    // ORIG 0x0816748e 判活为虚调用 IsEventing(0)
    // （vptr+0x34 = CEventBase::IsEventing），非直读 m_isEventing 字段。
    if (!IsEventing(0))
    {
        return false;
    }

    // ORIG 顺序（Fix18_E 实证）：user==0 检查在 find 之前（0x32 处），
    // 否则 null 用户会在 find 失败分支执行 get_charac_no/DisConnSig。
    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x2d8, "[HeroMission] User is null.");
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

    bool rewarded = false;
    bool advance = false;
    HeroMissionValue* value = 0;
    // ORIG 0x08167619：characNo 槽 -0x18（浅于 value/it）⇒ 声明提到分支外。
    unsigned int characNo;
    if (findMissionValue(user, mission, value))
    {
        // ORIG 0x081675f9：missionType 判定与两臂各重新 it->second（ptEv 各一次，
        // P48 无 CSE），不缓存 cond 局部。
        if (it->second->getMissionType() == 3)
        {
            // missionType==3 的条件对象即 TotalMission（ORIG ctor 写 vptr
            // 0x8b7f048 实证）；m_values.find 直接消费（ORIG 无 end 检查）。
            characNo = (unsigned int)user->get_charac_no(-1);
            std::map<unsigned int, std::vector<HeroMissionValue> >::iterator vit =
                m_values.find(characNo);
            BaseHeroMissionCondition* cond = it->second;
            rewarded = static_cast<TotalMission*>(cond)->rewardMission(
                user, *value, vit->second);
        }
        else
        {
            unsigned short before = value->m_fieldA;
            rewarded = it->second->rewardMission(user, *value);   // vtable 槽 +0x0c
            // ORIG 0x081676c5：两个独立守卫（jne/jbe 直跳统一出口，P133）；
            // 第二条件为 > before（无符号 cmp，非 !=0 的 test）。
            if (before == 0)
            {
                if (value->m_fieldA > before)
                {
                    advance = true;
                }
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
    // [b28d-solo-UserMiscLink] ORIG 0x081663fe。真语义修复：条件为
    // !isRepeatMission() && value.m_fieldA != 0（旧码误写 ||，重复任务被误拒）。
    // && 的 4.1 bool 物化岛（je L_false; je L_false; mov $1; jmp; mov $0; test）
    // 由 P226 语句表达式唯一复现（4.4 裸 && 恒折直跳）。
    if (({ !isRepeatMission() && value.m_fieldA != 0; }))
    {
        return false;
    }

    // 全部子任务（m_mission==6 或已完成）均达成才发奖。ORIG 走迭代器
    // begin/end/ne/ptEv/ppEv（-0x14 it / -0x10 end），非下标 operator[]。
    bool allDone = true;
    std::vector<HeroMissionValue>::iterator it = values.begin();
    while (it != values.end())
    {
        if (({ it->m_mission != 6 && it->m_fieldA == 0; }))
        {
            allDone = false;
            break;
        }
        ++it;
    }

    // ORIG 0x081664c9：ok 预清 0 + cmpb $0,allDone 直比跳尾（P222 正向 if 包裹）。
    bool ok = false;
    if (allDone)
    {
        ok = rewardProcess(user);
        value.m_fieldA = (unsigned short)(value.m_fieldA + 1);
        value.m_field6 = 1;
        if (isRepeatMission())
        {
            value.reset();
        }
    }
    return ok;
}

// ===========================================================================
// 8/9/9a) LevelupSupportEventManger —— GetStepIndex 0x08147362 /
//         InsertItem 0x081473cc / GetItemInfoVector 0x08147724
// ===========================================================================
int LevelupSupportEventManger::GetStepIndex(int requestLevel)
{
    // ORIG T 0x08147362：find(-0x10) 先、end(-0xc) 后、operator!= + test/je
    // 落 `mov $-1` 岛 ⇒ 正极性 `if (it != end) return it->second;`（P220）。
    std::map<int, int>::iterator it = m_stepIndexMap.find(requestLevel);
    if (it != m_stepIndexMap.end())
        return it->second;
    return -1;
}

std::vector<LevelupSupportSciprt::ItemInfo>*
LevelupSupportEventManger::GetItemInfoVector(int level)
{
    std::map<int, std::vector<LevelupSupportSciprt::ItemInfo>*>::iterator it =
        m_script.m_stepItems.find(level);
    // 同 GetStepIndex：ORIG T 0x08147724 为 op!= + je 落 mov $0 岛（P220）。
    if (it != m_script.m_stepItems.end())
        return it->second;
    return 0;
}

void LevelupSupportEventManger::InsertItem(CUser* user, int eventIdx, ENUM_ERROR& err)
{
    // [b28d-solo-UserMiscLink] 判词（N 封顶）：218/217、p167 CLEAR(28=28)；
    // 残差=①4.4 整数提升致 `movzwl %ax,%eax; sar $0xf,%eax`（ORIG 16 位 `shr $0xf,%ax`）；
    // ②N7 间接 call 目标 %eax vs %edx。slot<0 判定以 (unsigned short)(~slot)>>15 复现
    // ORIG 的 not/shr 形（cmpw/js 直比差 3 条）。
    // ORIG 0x081473cc：P157 每次重取
    // user->m_levelupEvents[eventIdx]（无 info 缓存，operator[] 两处独立调用），
    // items->at(i) 每字段重调（m_itemIndex/m_itemCount 各一次）。
    user->m_levelupEvents[eventIdx].m_state = 2;

    const std::vector<LevelupSupportSciprt::ItemInfo>* items =
        ARAD::Singleton<LevelupSupportEventManger>::Get()->GetItemInfoVector(
            (int)user->m_levelupEvents[eventIdx].m_level);
    if (items == 0)
    {
        err = (ENUM_ERROR)0x15;
        return;
    }

    // ORIG 0x0814743b：size() 缓存于 -0x1c，循环尾 `addl $1; cmp; setb; test; jne`
    // （while 形，非 4.4 for 的直跳形）。
    std::size_t i = 0;
    std::size_t n = items->size();
    while (i < n)
    {
        CItem* pItem = G_CDataManager()->find_item(items->at(i).m_itemIndex);
        if (pItem == 0)
        {
            err = (ENUM_ERROR)0x15;
            ARAD::PRINT_ERROR::find_item(
                (char*)" LevelupSupportEventManger::InsertItem", user,
                items->at(i).m_itemIndex);
            return;
        }

        Inven_Item item;
        // ORIG：make_item 前先置 m_addInfo(+0x02) = get_index()（0x081474c9）
        item.m_addInfo = pItem->get_index();
        pItem->make_item(item);
        // ORIG 0x0814753c：`movzwl 0x4(%eax),%eax; cwtl` ⇒ 原码为 (short) 截断后转 int
        item.set_add_info((int)(short)items->at(i).m_itemCount);

        // ORIG 0x0814752b：返回值截断为 short 存 -0x12（`mov %ax,-0x12`）。
        short slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)0xd, true, true);
        // ORIG 0x081475ec：slot<0 跳后置 mail 块（`not/shr $0xf/test/je`），
        // slot>=0 直落 SendUpdateItem ⇒ 正极性 if/else 交换块序（P59/P222）。
        // ORIG 0x081475ec：`movzwl -0x12,%eax; not %eax; shr $0xf,%ax; test %al,%al; je`
        // ⇒ 原码为 (unsigned short)(~slot) >> 15（非 cmpw/js 直比形）。
        if ((unsigned char)((unsigned short)(~slot) >> 15) != 0)
        {
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
        }
        else
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
        ++i;
    }

    // ORIG 0x08147622：再次 operator[] 取 m_level（无 info 缓存）。
    DB_SaveEventLevelupSupport::makeRequest((int)user->GetUID(),
                                            (int)user->getCurCharacNo(),
                                            (int)user->m_levelupEvents[eventIdx].m_level, 2);
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
    // [L31 round2 实测回滚 · 判据⑫/P198①] ORIG 0x085f3365 对 err 有预清（`int err = 0;`
    // 两句式），但实测把 err/i/pick/sel 四处预清全补后 clean 差异 **406→416 行**（反劣；
    // clr_l 计数虽由 3→6 匹配 ORIG 的 5，但槽位连锁错位加剧）⇒ 四处全部回滚。
    int err = _get_option_type(itemIdx, rarity, level, group, optType);
    // [b28d-solo-UserMiscLink] P225：ORIG 0x085f33a1 为 `cmpl $0; setne %al; test %al,%al;
    // je`（bool 物化岛）⇒ while 形复现（4.4 裸 if 恒直跳）。
    while (err != 0)
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
    while (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x35d, "_choose_option_group (%d,%d,%d,%d,%d,%d) error_line(%d)",
            (int)(unsigned char)rarity, group, grade, level,
            (int)optType, outGroup, err);
        return false;
    }

    // [b28d-solo-UserMiscLink] 判词（N 封顶）：443/432、p167 CLEAR(47=47)；
    // 残差=①帧 0xc0 vs 0xd0 + 局部槽深差（optType -0x53 vs -0x65、err -0x28 vs -0x1c）
    // ⇒ N82/N95；②4.4 lea 折叠（optB[i] 字节写 vs ORIG disp 直写）；③寄存器分配。
    // 已复现：const_iterator（P96/P270 翻转 13 项 p167 差集）、begin()+sel 的 deEv/plERKi
    // （P167 判据）、setne 物化岛（P225 while 形）、union 字节键、GetRandomOption 逐点重取（P157）。
    // 不同权重表（GetRandomOption()+0xf4 = RandomOptionScript::m_mapf4）：
    // 键 = 低字节 rarity | 高字节 optType（ORIG 0x085f34d3/da 两次字节写 -0x6e/-0x6d
    // ⇒ 原码 union 字节视角）；P157：GetRandomOption() 每次重取（ORIG 四处独立 call）。
    DifferentWeightValue dwv;
    union
    {
        unsigned short m_w;
        unsigned char m_b[2];
    } dwRaw;
    dwRaw.m_b[0] = (unsigned char)rarity;
    dwRaw.m_b[1] = optType;
    std::map<short, DifferentWeightValue>::const_iterator dw =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.find(dwRaw.m_w);
    if (dw == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x369, "different_weight_ (%d,%d) ",
            (int)(unsigned char)rarity, (int)optType);
        return false;
    }
    dwv = dw->second;

    // 组值表（GetRandomOption()+0x160 = m_map160）。
    std::map<int, OptionGroupValue>::const_iterator ogIt =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.find(outGroup);
    if (ogIt == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.end())
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
    // ORIG 0x085f3705：迭代器默认构造（C1Ev）在循环之前 ⇒ 先声明后赋值（P50 复活）。
    std::vector<OptionGroup>::iterator selIt;

    // 第一组已占槽（非本次 slot）按其组类型扣权重并移出候选。
    // （i 的 ORIG 预清形 0x085f3674 `movl $0x0,-0xc` 属上述已回滚组。）
    for (int i = 0; i <= 2; ++i)
    {
        if (i == slot || optB[i * 3] == 0)
        {
            continue;
        }
        // ORIG 0x085f36b7：begin 在 if 内、while 形、`(*it).m` 走 deEv（P167 新判据）。
        std::vector<OptionGroup>::iterator it = groupVec.begin();
        while (it != groupVec.end())
        {
            if ((unsigned int)(*it).m_type0 == (unsigned int)optB[i * 3])
            {
                remaining -= (*it).m_weight4;
                groupVec.erase(it);
                break;
            }
            ++it;
        }
    }

    unsigned long bound = (unsigned long)remaining;
    int pick = m_rand[9].randInt(bound);          // m_rand+0x5808 → 池[9]

    CRandomList<unsigned int> chooser((unsigned int)pick);
    std::for_each(groupVec.begin(), groupVec.end(), chooser);

    unsigned int sel = chooser.get_value();
    if (sel < groupVec.size())
    {
        // ORIG 0x085f38f8：begin()+sel 再 deEv（非 operator[]；P167 判据）。
        selIt = groupVec.begin() + sel;
        optB[0xa] = (unsigned char)(*selIt).m_type0;    // 第二组槽首字节=组类型
        optB[0xb] = (unsigned char)_get_option_value(dwv.m_field0, dwv.m_field4);
        optB[0xc] = (unsigned char)_get_option_value(dwv.m_field0, dwv.m_field4);
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
    // [b28d-solo-UserMiscLink] ORIG 0x082138f2：真块(+0x40)居中、假块(+0x47) fall-through
    // leave；共享真出口由「return 短路 or 表达式」唯一复现（if+or 多一层 bool 物化 31 条、
    // 平铺双 return 28 条、goto 尾标签 29 条）。本形 26=26 与 ORIG 逐条同形。
    return (payType == 0 && !GlobalData::s_auction_proxy->IsRunning()) ||
           (payType == 1 && !GlobalData::s_cera_auction_proxy->IsRunning());
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
    // ORIG 0x085f7130：`call size` **先**、`mov idx,%edx; add $1,%edx` 后，
    // `cmp %edx,%eax; jge`（有符号，eax=size）⇒ 条件写成 `(int)size < idx+1`
    // 才能让 4.4 右操作数后置并出 jge（写 (size_t)(idx+1) > size 出无符号 jbe
    // + 提前 esi 缓存，帧 0x20 vs ORIG 0x24）。
    if (idx < 0 || m_redeemList.empty() ||
        (int)m_redeemList.size() < idx + 1)
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
    // ORIG 0x085f73cd 调自由函数 _ZSteqI…RKSt14_List_iteratorIT_
    // RKSt20_List_const_iteratorIS3_（iterator 在前、const_iterator 在后）⇒
    // 源码必须写 `end() == it`；写 `it == end()` 走 const_iterator::eq 并多一次
    // iterator→const_iterator 转换构造（+5 条）。
    if (m_redeemList.end() == it)
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
        std::memmove(&out, &*it, 0x44);
    }
    return out;
}

Inven_Item_Expand_RedeemInfo CRedeemItem::RedeemItem(int idx)
{
    // [b28d-solo-UserMiscLink] AE：distance 复用 begin 结果（ORIG 0x085f72d8 只调一次
    // begin，旧码二次 m_redeemList.begin() 多 1 槽/4 条）+ memcpy→memmove 对齐 ORIG 符号。
    Inven_Item_Expand_RedeemInfo out;   // ORIG ctor 清零副本
    alter();                            // charac_expand::CData::alter 置脏

    // ORIG 以 const_iterator 定位 idx 后 advance 到非常量迭代器再 erase。
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator cit =
        _CheckVaildIndex_N_GetIter(idx);
    std::list<Inven_Item_Expand_RedeemInfo>::iterator it = m_redeemList.begin();
    std::advance(it, std::distance<std::list<Inven_Item_Expand_RedeemInfo>::const_iterator>(
                         it, cit));
    if (it != m_redeemList.end())
    {
        std::memmove(&out, &*it, 0x44);
        m_redeemList.erase(it);
    }
    return out;
}

}  // namespace Redeem_Item

// P147 显式实例化：发射 ORIG W _ZN11CRandomListIjE9get_valueEv @0x085f49a0。
template unsigned int CRandomList<unsigned int>::get_value();
