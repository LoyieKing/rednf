// ============================================================================
// ExpertJobOnlyMgr.cpp — expert_job OnlyOrig 清零批（EJMgrSide 批次）
//
// 主符号（ORIG nm -S / objdump 核对，逐份报告完整读后忠实转写）：
//   expert_job::CAlchemist
//     UseDelilah                    T 0x085d18ae  UseRevivalItem       T 0x085d1694
//     GetAssaultPrevent(CUser*,b*)  T 0x085d137c  OnUseStackableItem   T 0x085d1712
//     GetQuestItemDropRate          T 0x085d12ee  CheckUsableStackable T 0x085d18e2
//   expert_job::CDisjointer
//     OnEnterDisjointStore          T 0x085d309a  OnRepairDisjointMachine T 0x085d35ce
//     OnUpgradeDisjointMachine      T 0x085d37e8
//   expert_job::CDollController 生命周期 C1/C2 0x085d45b4、D1/D2 0x085d45d0、
//     D0 0x085d4600（ORIG vtable 四业务槽全 __cxa_pure_virtual → 抽象类，
//     ctor 仅置 vptr、dtor 仅置 vptr 后链基类析构；由类定义自然生成）
//   expert_job::CExpertJobMgr
//     OnLeaveUser T 0x0849e214  OnRequestPeer  T 0x0849e256
//     OnResponsePeer T 0x0849e362  IsAvailableRequestPeer T 0x0849e55e
//     OnEnchantByBead T 0x0849ed1a  OnGiveup  T 0x0849e60c
//   expert_job::CCharacExpertJob
//     SendExpertJobInfo             T 0x0832b454
//     set_disjoint_machine_grade    W 0x85d3e9a【objdump】
//   expert_job::CDisjointMachine::include(village_object::IObject const&) const
//                                   W 0x85d3de6【objdump】
//
// SKIP（已有唯一 owner）：CExpertJobMgr::AcquireExpertJob → ExpertJobLink4.cpp
//
// 阻塞依赖（见 yield 清单；调用点以 BLOCKED 注释精确保留）：
//   OnEnchantByBead: CStackableItem::GetMonsterCardId W 0x849f530 /
//     InstanceRentalSystem::CheckRentableItem(int,unsigned) T 0x82702dc /
//     GetInstanceExpertJobStatistic T 0x860f940 + increaseItemCount T 0x860f9d4 /
//     cUserHistoryLog::EnchantItem(int,int,int,bool,const char*,const char*)
//   OnUseStackableItem/CheckUsableStackableItem: CStackableItem::get_sub_type
//     W 0x0822c9d2（头缺声明）
// ============================================================================

#include "CExpertJob.h"

#include "CBattle_Field.h"     // CBattle_Field::SetAssignNPCByPotion（UseDelilah）
#include "CDataManager.h"      // G_CDataManager / GetExpertJobScript / find_item
#include "CParty.h"            // GetParty/get_user（+0xb24 内嵌战场区）
#include "CPrivateStoreMgr.h"  // private_store 单例 / IsBusyPrivateStore
#include "CStackableItem.h"    // m_int2d0（APC 药水持续秒）
#include "CSystemTime.h"       // CSystemTime::getCurSec
#include "GameTypes.h"         // cMyTrace
#include "GlobalData.h"        // s_systemTime_
#include "PacketGuard.h"
#include "SkillSlot.h"      // SkillSlot::delete_skill
#include "GameWorld.h"      // G_GameWorld / send_all

// CExpertJobList.cpp 定义的专家炼金信息表（B 0x094f7778，8 字节；
// [1] 即 ORIG 高半区 _DAT_094f777c）。
extern int g_alchemistInfo[2];

namespace expert_job
{

namespace
{

// ORIG OnGiveup 经 __cxa_throw 携带 ENUM_ERROR 类实例；仓库将 ENUM_ERROR 建
// 模为 enum，暂以等宽 int 载荷抛出（恢复类建模时替换）。
void ThrowEnumError(int code)
{
    throw code;
}

// 自身扩展数据槽（charac_expand 槽 0）快捷读取，与各报告两次独立取址序一致。
inline CCharacExpertJob* GetSelfExpand(CUser* user)
{
    return (CCharacExpertJob*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0);
}

}  // namespace

// ===========================================================================
// expert_job::CAlchemist
// ===========================================================================

// ---- UseDelilah（报告 UseDelilah.md，ORIG T 0x085d18ae sz 0x33）----
void CAlchemist::UseDelilah(CUser* user)
{
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        CBattle_Field* field =
            reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);
        field->SetAssignNPCByPotion(true);
    }
}

// ---- UseRevivalItem（报告 UseRevivalItem.md，ORIG T 0x085d1694 sz 0x7d）----
void CAlchemist::UseRevivalItem(CUser* user)
{
    int curSec = GlobalData::s_systemTime_.getCurSec();
    user->SetRevivalTime(curSec + 0x4b0);
    long revivalTime = user->GetRevivalTime();
    unsigned int uid = static_cast<unsigned int>(user->get_unique_id());
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, uid, (TIMER_MESSAGE)0x82,
                                0x4b0, revivalTime, 0);
}

// ---- GetQuestItemDropRate（报告，ORIG T 0x085d12ee sz 0x71）----
int CAlchemist::GetQuestItemDropRate(CUser* user, int rate)
{
    bool boosted = IsExistContinuousEffectItem(user, 0x27ac52);
    if (!boosted)
    {
        boosted = IsExistContinuousEffectItem(user, 0x1f8e);
    }
    if (boosted)
    {
        int infoPlus = g_alchemistInfo[1];
        rate = (infoPlus + 10000) * rate / 10000;
    }
    return rate;
}

// ---- CheckUsableStackableItem（报告，ORIG T 0x085d18e2 sz 0x96）----
int CAlchemist::CheckUsableStackableItem(CUser* user, CItem const* item)
{
    // ORIG：CStackableItem::get_sub_type()==2（APC 强力药水）时检查全队
    // GetUseAPCPotionTime 未到者存在则禁用；其余放行。
    const CStackableItem* stacked = static_cast<const CStackableItem*>(item);
    if (stacked->get_sub_type() == 2)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = party->get_user(i);
                if (member != 0 &&
                    member->GetUseAPCPotionTime() >
                        GlobalData::s_systemTime_.getCurSec())
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

// ---- GetAssaultPrevent（报告，ORIG T 0x085d137c sz 0xa7）----
int CAlchemist::GetAssaultPrevent(CUser* user, bool* peerFlag)
{
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (IsExistContinuousEffectItem(user, 0x27ac4a))
    {
        return 1;
    }
    if (party != 0)
    {
        for (int i = 0; i < 4; ++i)
        {
            CUser* member = party->get_user(i);
            if (member != 0 && member != user &&
                IsExistContinuousEffectItem(member, 0x27ac4a))
            {
                if (peerFlag != 0)
                {
                    *peerFlag = true;
                }
                return 1;
            }
        }
    }
    return 0;
}

// ---- OnUseStackableItem（报告，ORIG T 0x085d1712 sz 0x19b）----
int CAlchemist::OnUseStackableItem(CUser* user, unsigned long itemUniqueId,
                                   CItem const* item)
{
    if (item->is_stackable())
    {
        // vtbl+0xc = GetItemType()==3 且对象 +0x199 字节非零 → 可学 recipe。
        // +0x199 位于 CStackableItem 未建模区段【ORIG 裸字节判读】。
        if (((const CStackableItem*)item)->GetItemType() == 3 &&
            *(const unsigned char*)(reinterpret_cast<const char*>(item) + 0x199) != 0)
        {
            STExpertJobScript* script =
                (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(
                    user->GetCurCharacExpertJobType());
            if (script != 0)
            {
                int exp = user->GetCurCharacExpertJobExp();
                int level = user->GetCurExpertJobLevel(exp);
                CCharacExpertJob* expand = GetSelfExpand(user);
                bool learned = expand->learnRecipe(itemUniqueId, script, level);
                if (!learned)
                {
                    return 0xd;
                }
                expand = GetSelfExpand(user);
                expand->SendExpertJobInfo(user);
            }
        }

        // 【BLOCKED】APC 药水段（ORIG get_sub_type()==2 → SetUseAPCPotionTime
        // (curSec + m_int2d0/1000))；恢复方式同 CheckUsableStackableItem 注释。
    }

    // 固定道具唯一 id 分派（分支序照 ORIG 汇编比较树）：
    //   0x28e7cf / 0x27ac54 / 0x29f287 / 0x29f2ed → UseRevivalItem
    //   0x27acda → UseDelilah；其余不处理。
    if (itemUniqueId == 0x28e7cfUL || itemUniqueId == 0x27ac54UL ||
        itemUniqueId == 0x29f287UL || itemUniqueId == 0x29f2edUL)
    {
        UseRevivalItem(user);
    }
    else if (itemUniqueId == 0x27acdaUL)
    {
        UseDelilah(user);
    }
    return 0;
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDisjointer（分解术士三接口）
// ===========================================================================

namespace expert_job
{

// ---- OnEnterDisjointStore（报告，ORIG T 0x085d309a sz 0x101）----
// 包 0xf5：byte(1) byte(等级+1) int(修理费) int(耐久)。
int CDisjointer::OnEnterDisjointStore(CUser* user)
{
    PacketGuard packet;
    packet.put_header(1, 0xf5);
    packet.put_byte(1);
    packet.put_byte(get_disjoint_machine_grade() + 1);
    packet.put_int(get_disjoint_cost());
    CUser* owner = m_machine.get_user();
    CCharacExpertJob* expand = (CCharacExpertJob*)owner->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0);
    packet.put_int(expand->get_disjoint_machine_endurance());
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ---- OnRepairDisjointMachine（报告，ORIG T 0x085d35ce sz 0x218）----
// 分支 A：现价修理；分支 B（钱不够）：GetDisjointMachineRepair 以全部现金
// 换算可修值并回写现价。新耐久 <1 → 错误 0x16。
int CDisjointer::OnRepairDisjointMachine(CUser* user)
{
    int grade = GetSelfExpand(user)->get_disjoint_machine_grade();
    int endurance = GetSelfExpand(user)->get_disjoint_machine_endurance();
    int currentPrice =
        GetScript()->GetDisjointMachineRepairCost(grade + 1, endurance);
    int newEndurance = 0;

    if (user->getCurCharacMoney() < currentPrice)
    {
        // 现金不足：以全部金钱换算，返回基础值 + 当前耐久（出参回写现价）。
        newEndurance =
            GetScript()->GetDisjointMachineRepair(
                GetSelfExpand(user)->get_disjoint_machine_grade() + 1,
                user->getCurCharacMoney(), currentPrice) +
            GetSelfExpand(user)->get_disjoint_machine_endurance();
    }
    else
    {
        stEnduranceRepairCost* cost =
            GetScript()->_GetEnduranceRepairCost((unsigned)(grade + 1));
        if (cost != 0)
        {
            newEndurance = cost->m_field4;   // 满值槽
        }
    }

    if (newEndurance < 1)
    {
        return 0x16;
    }

    user->getCurCharacInvenW()->use_money(currentPrice, (eMoneySubReason)0x20, true);
    GetSelfExpand(user)->set_disjoint_machine_endurance(newEndurance);

    PacketGuard packet;
    packet.put_header(1, 0xf0);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacMoney());
    packet.put_int(GetSelfExpand(user)->get_disjoint_machine_endurance());
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ---- OnUpgradeDisjointMachine（报告，ORIG T 0x085d37e8 sz 0x2e2）----
int CDisjointer::OnUpgradeDisjointMachine(CUser* user)
{
    int grade = GetSelfExpand(user)->get_disjoint_machine_grade();

    if (!(grade + 1 < GetScript()->GetDisjointMachineMaxGrade()))
    {
        return 0x13;
    }
    int expertLevel = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    if (grade + 1 >= expertLevel)
    {
        return 0x13;
    }

    // Etc 脚本 [expertjob level limit] 表：key=grade+2 → 所需角色等级。
    char keyByte = (char)(grade + 2);
    std::map<unsigned char, short>& levelMap = GetEtcScript()->m_levelMap;
    std::map<unsigned char, short>::const_iterator it =
        levelMap.find((unsigned char)keyByte);
    if (it == levelMap.end())
    {
        return 0x13;
    }
    if (user->get_charac_level() < it->second)
    {
        return 0xe;
    }

    ++grade;   // 升级
    int upgradeCost = GetScript()->GetUpgradeCost((unsigned)grade);
    if (upgradeCost <= 0 || user->getCurCharacMoney() < upgradeCost)
    {
        return 0x16;
    }

    user->getCurCharacInvenW()->use_money(upgradeCost, (eMoneySubReason)0x20, true);
    GetSelfExpand(user)->set_disjoint_machine_grade(grade);

    // 新等级满值耐久若存在则一并重置。
    stEnduranceRepairCost* full =
        GetScript()->_GetEnduranceRepairCost((unsigned)(grade + 1));
    if (full != 0)
    {
        GetSelfExpand(user)->set_disjoint_machine_endurance(full->m_field4);
    }

    PacketGuard packet;
    packet.put_header(1, 0xf4);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacMoney());
    packet.put_int(grade);   // 已升后的新等级（ORIG put_int(local-1c 即 grade））
    packet.put_int(GetSelfExpand(user)->get_disjoint_machine_endurance());
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDollController —— 抽象派生（ORIG vtable @0x8cc08e0：4 业务槽
// 全 __cxa_pure_virtual + D1/D0）。ctor 仅基类构造+vptr 置位，dtor 链基类析构，
// 由类定义自然生成；完整类布局建议见 decl 文件。
// ===========================================================================

namespace expert_job
{

CDollController::CDollController()
{
}

CDollController::~CDollController()
{
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CExpertJobMgr
// ===========================================================================

namespace expert_job
{

// ---- OnLeaveUser（报告，ORIG T 0x0849e214 sz 0x41）----
void CExpertJobMgr::OnLeaveUser(CUser* user)
{
    CExpertJob* job = user->GetCurCharacExpertJob();
    if (job != 0)
    {
        // ORIG 虚调用 vptr+4（本域 vtable 业务槽 1 = OnCloseMission）。
        job->OnCloseMission(user, ENUM_CLOSE_MISSION_TYPE_1);
    }
}

// ---- IsAvailableRequestPeer（报告，ORIG T 0x0849e55e sz 0xad）----
unsigned int CExpertJobMgr::IsAvailableRequestPeer(CUser* requester,
                                                   CUser* target,
                                                   ENUM_PEER_REQUEST_TYPE type,
                                                   PacketGuard& packet)
{
    (void)type;
    (void)packet;
    if (target->getCurCharacR() != 0 && requester->getCurCharacR() != 0)
    {
        if (requester->GetCurCharacExpertJob() == 0 &&
            target->GetCurCharacExpertJob() == 0)
        {
            if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(target))
            {
                return 0x3d;
            }
            if (target->IsCurCharacGhost())
            {
                return 0x13;
            }
            return 0;
        }
        return 0x13;
    }
    return 3;
}

// ---- OnRequestPeer（报告，ORIG T 0x0849e256 sz 0x10b）----
void CExpertJobMgr::OnRequestPeer(CUser* requester, CUser* target,
                                  ENUM_PEER_REQUEST_TYPE type,
                                  PacketGuard& packet)
{
    unsigned int err = IsAvailableRequestPeer(requester, target, type, packet);
    if (err != 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)10, type,
                                          (unsigned char)err, packet);
        return;
    }
    packet.clear();
    packet.put_header(0, 7);
    packet.put_short((int)(requester->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.put_int((int)(target->get_unique_id() & 0xffffffff));
    packet.finalize(true);
    target->Send(packet);
}

// ---- OnResponsePeer（报告，ORIG T 0x0849e362 sz 0x1fb）----
void CExpertJobMgr::OnResponsePeer(CUser* requester, CUser* peerTarget,
                                   CUser* jobOwnerA, CUser* jobOwnerB,
                                   int cookie, ENUM_PEER_REQUEST_TYPE type,
                                   PacketGuard& packet)
{
    unsigned int err = IsAvailableRequestPeer(requester, peerTarget, type, packet);
    if (err != 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)0xb, type,
                                          (unsigned char)err, packet);
        return;
    }

    CEnchanter* enchanter =
        static_cast<CEnchanter*>(AcquireExpertJob((ENUM_EXPERT_JOB_TYPE)1));
    if (enchanter == 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)0xb, type, 0x16, packet);
        return;
    }

    enchanter->OnStartMission(jobOwnerA, jobOwnerB);   // ORIG vptr+0 业务槽 0
    enchanter->SetPeerRequestType(type);

    packet.clear();
    packet.put_header(0, 8);
    packet.put_short((int)(requester->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.put_int(cookie & 0xffffffff);
    packet.finalize(true);
    peerTarget->Send(packet);

    packet.clear();
    packet.put_header(1, 0xb);
    packet.put_byte(1);
    packet.put_short((int)(peerTarget->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.finalize(true);
    requester->Send(packet);
}

// ---- OnEnchantByBead（报告，ORIG T 0x0849ed1a sz 0x474）----
int CExpertJobMgr::OnEnchantByBead(CUser* user, int param2, int beadSlot,
                                   int param4, int ringSlot)
{
    (void)param2;
    (void)param4;
    CDataManager* dm = G_CDataManager();

    Inven_Item* beadRef = user->getCurCharacInvenW()->GetInvenRef(1, beadSlot);
    if (beadRef == 0)
    {
        return 0x11;
    }
    Inven_Item* ringRef = user->getCurCharacInvenW()->GetInvenRef(1, ringSlot);
    if (ringRef == 0)
    {
        return 0x11;
    }

    // 【BLOCKED】租赁两连查 InstanceRentalSystem::CheckRentableItem(int,unsigned)
    // T 0x82702dc —— 权威头仅单参虚槽；ORIG kind=1/kind=2 任一命中 → 0x11。

    CItem* beadItem = dm->find_item(beadRef->m_addInfo);
    if (beadItem == 0)
    {
        return 0x11;
    }
    CItem* ringItem = dm->find_item(ringRef->m_addInfo);
    if (ringItem == 0)
    {
        return 0x11;
    }
    if (!beadItem->is_stackable())
    {
        return 0x11;
    }

    // 【BLOCKED】怪物卡主表链（恢复步骤按 ORIG 序）：
    //   cardId     = ((CStackableItem*)beadItem)->GetMonsterCardId(); // W 0x849f530
    //   masterItem = dm->find_item(cardId); 为空 → 0x11
    //   EnableWorld(ringItem, beadItem) != 1 → 0x13
    //   masterItem->getUsableItemType(
    //       ((CStackableItem*)ringItem)->GetItemType()) != 1 → 0x17

    bool deleted =
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, beadSlot, 1,
                                                (eItemDelReason)8, true);
    if (!deleted)
    {
        cMyTrace trace("int expert_job::CExpertJobMgr::OnEnchantByBead(CUser*, "
                       "int, int, int, int)", 0x6cd, 5);
        trace("CExpertJobMgr::OnEnchantByBead, getCurCharacInvenW()->delete_item "
              "failed , %d %d", user->getCurCharacNo(), beadSlot);
        return 0x11;
    }

    int savedCardIdx = ringRef->m_fieldd;
    int replacedCardIdx =
        savedCardIdx; /* BLOCKED: 恢复为 dm->find_item(cardId)->get_index() */

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(2);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, ringSlot, packet);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, beadSlot, packet);
    packet.finalize(true);
    user->Send(packet);

    // 【BLOCKED】统计与历史：
    //   GetInstanceExpertJobStatistic() T 0x860f940 → increaseItemCount(
    //       (ENUM_EXPERT_JOB_TYPE)1, masterIndex, 1)      // T 0x860f9d4
    //   (user+0x79700 历史对象)->EnchantItem(ringRef->m_addInfo, savedCardIdx,
    //       replacedCardIdx, true, get_acc_name(), getCurCharacName())

    ringRef->m_fieldd = replacedCardIdx;

    packet.clear();
    packet.put_header(1, 0x113);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ---- OnGiveup（报告，ORIG T 0x0849e60c sz 0x70d）----
void CExpertJobMgr::OnGiveup(CUser* user)
{
    if (user->getCurCharacR() == 0)
    {
        ThrowEnumError(3);
    }
    STExpertJobScript* script =
        (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(
            user->GetCurCharacExpertJobType());
    if (script == 0)
    {
        ThrowEnumError(8);
    }

    unsigned int giveupCount = GetSelfExpand(user)->get_giveup_count() & 0xffff;
    if ((unsigned int)script->m_giveupCost.size() <= giveupCount)
    {
        ThrowEnumError(1);
    }
    int giveupCost = script->m_giveupCost.at(giveupCount);
    if (user->getCurCharacMoney() < giveupCost)
    {
        ThrowEnumError(0x15);
    }

    // 前置校验：[clear quest] 清单全部已达标，任一未清 → 错误 8。
    for (std::vector<int>::iterator it = script->m_clearQuests.begin();
         it != script->m_clearQuests.end(); ++it)
    {
        if (user->getCurCharacQuestRefR()->isClearQuest(*it) != true)
        {
            ThrowEnumError(8);
        }
    }
    for (std::vector<int>::iterator it = script->m_clearQuests.begin();
         it != script->m_clearQuests.end(); ++it)
    {
        user->getCurCharacQuestW()->resetClearQuest(*it);
    }

    PacketGuard packet;
    CInventory* inven = user->getCurCharacInvenW();
    inven->use_money(giveupCost, (eMoneySubReason)0x1e, true);

    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, packet);
    packet.finalize(true);
    user->Send(packet);

    // 删除双树技能（[skill] 表 first = skillIdx）。
    SkillSlot* skillSlot = (SkillSlot*)user->getCurCharacSkillW();
    for (unsigned int i = 0; i < (unsigned int)script->m_growSkillVec.size(); ++i)
    {
        int skillIdx = script->m_growSkillVec[i].first;
        skillSlot->delete_skill(skillIdx, user->get_charac_job(),
                                (ENUM_SKILL_TREE_KIND)0);
        skillSlot->delete_skill(skillIdx, user->get_charac_job(),
                                (ENUM_SKILL_TREE_KIND)1);
    }
    user->send_skill_info();

    user->LogHistory("ExpertJob-,%d,%d,%d", user->get_charac_level(),
                     user->GetCurCharacExpertJobType(), (int)giveupCount);
    user->SetCurCharacExpertJobType(0);
    user->SetCurCharacExpertJobExp(-1);

    UserQuest* quest = user->getCurCharacQuestW();
    quest->get_quest_info(reinterpret_cast<char*>(&packet));   // 复用包缓冲
    user->Send(packet);

    ++giveupCount;
    if ((unsigned int)script->m_giveupCost.size() <= giveupCount)
    {
        giveupCount = script->m_giveupCost.size() - 1;
    }
    static_cast<charac_expand::CData*>(GetSelfExpand(user))->reset();
    GetSelfExpand(user)->set_giveup_count((unsigned short)giveupCount);

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);

    packet.clear();
    packet.put_header(1, 0xf3);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacMoney());
    packet.put_byte((int)giveupCount);
    packet.finalize(true);
    user->Send(packet);
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CCharacExpertJob 补充
// ===========================================================================

namespace expert_job
{

// ---- SendExpertJobInfo（报告，ORIG T 0x0832b454 sz 0x201）----
int CCharacExpertJob::SendExpertJobInfo(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0xcd);
    packet.put_byte(m_giveupCount);
    int jobType = user->GetCurCharacExpertJobType();
    packet.put_byte(jobType);

    if (jobType == 3)
    {
        packet.put_int(m_enchantCount + 1);
        packet.put_int(m_endurance);
    }
    else if (jobType >= 1 && jobType <= 4)
    {
        int count = 0;
        int countIndex = packet.get_index();
        packet.put_byte(0);
        STExpertJobScript* script =
            (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(jobType);
        if (script != 0)
        {
            for (int i = 0; i < 0xf0; ++i)
            {
                if (get_recipe(i) == 1)
                {
                    stRecipeInfo* info = script->GetRecipeInfo(i);
                    if (info != 0 &&
                        *reinterpret_cast<const int*>(info->m_pad) != 0)
                    {
                        packet.put_int(*reinterpret_cast<const int*>(info->m_pad));
                        ++count;
                    }
                }
            }
        }
        packet.put_byte(countIndex, count);   // 回填 recipe 计数
    }
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

// ---- set_disjoint_machine_grade【objdump】----
// ORIG W 0x85d3e9a sz 0x1b：call charac_expand::CData::alter 后写 +0x0c。
void CCharacExpertJob::set_disjoint_machine_grade(int value)
{
    alter();
    m_enchantCount = value;   // +0x0c
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDisjointMachine::include【objdump】
// ORIG W 0x85d3de6 sz 0x20：this+0x04 与 other+0x04 两矩形转调
// collision_detection::Rectangle::include（W 0x85cb492）。
// ===========================================================================

namespace expert_job
{

bool CDisjointMachine::include(const village_object::IObject& other) const
{
    const CDisjointMachine& rhs = reinterpret_cast<const CDisjointMachine&>(other);
    return m_rect.include(rhs.m_rect);
}

}  // namespace expert_job
