// df_game_r 还原 —— EventClassify 域仅 ORIG 符号批次（W8_Mid.EventBatch）。
// 覆盖：
//   CObjectBringUp::{verifyGiveBringUpReward 0x0810fe1a, verifyMaterialItemByRule
//     0x0810fcea, sendBringUpReward 0x0810fe90}
//   CEventScriptMng::{send_event_init_record_data 0x0810cb2c,
//     process_update_ingame_history 0x0810c82e}
//   CPcRoomPlayTime::processEventPcRoomPlayTime 0x0811089c
//   CUsedFatigueGiveItem::get_user_type 0x08110eec（objdump·报告缺失）
//   CExchangeRandomItemReward::{verifyMaterialItemByRule 0x0810eb30,
//     deleteUsedMaterialItem 0x0810eede, getRewardItem 0x0810ee40,
//     sendRandomItemReward 0x0810f26e}
//   CAvatarDisjointRandomReward::{checkAvatarDisjointError 0x0810f3a4,
//     deleteAvatarItem 0x0810f4e2, SendDisjointAvatarStatistic 0x0810f5f4,
//     getRewardItem 0x0810f694, SendCreateEmblemStatistic 0x0810f742,
//     sendRandomItemReward 0x0810f88c}
// 每函数依据 docs/class_func_reports/EventClassify__*/ 对应 md 的
// ## 汇编 + ## 反编译 C 完整转写；类布局见 EventClassify_Action.h /
// InGameEventScripts.h / CEventScriptMng.h。

#include <cstring>
#include <utility>

#include "CEventScriptMng.h"         // 实体类 + CEventScriptMng
#include "InGameEventScripts.h"      // ObjectBringUpScript / ExchangeRandom... 等
#include "EventClassify_Action.h"    // CEventActionMng
#include "CUser.h"                   // CUserCharacInfo getter / Send / get_acc_id
#include "CInventory.h"
#include "CItem.h"                   // GetItemType/get_grade/get_index
#include "CAvatarItemMgr.h"          // UnRegistItem/GetExpireDate
#include "CStatisticServerProxy.h"
#include "Packet_PcRoomPlayTimeReward.h"      // 全局 Packet_PcRoomPlayTimeReward（ORIG W C1 0x08110c0a）
#include "Packet_Emblem_Create_Statistic.h"   // 全局 Packet_Emblem_Create_Statistic（ORIG W C1 0x08110bdc）
#include "Packet_Avater_Disjoint_Statistic.h" // 全局 Packet_Avater_Disjoint_Statistic（ORIG W C1 0x08110bae）

#include "CDataManager.h"            // G_CDataManager/find_item/m_eventAvatarList
#include "GlobalData.h"              // s_stream_pool/s_msgq_mgr/s_*_proxy_mgr
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "PacketGuard.h"             // PacketGuard + PacketHeader（传递可见）
#include "SigTypes.h"                // SIG_OBJECT_BRING_UP
#include "CEnvironment.h"            // G_CEnvironment / m_serverEnvir.m_serverGroup
#include "GameWorld.h"               // IsIntegratedPvPBaseChannel / G_GameWorld
#include "GameTypes.h"               // cMyTrace
#include "CServerProxyMgr.h"         // CServerProxyMgr<T>::GetServerProxy
#include "CMonitorServerProxy.h"
#include "DB_UpdateAvatarHistory.h"
#include "DB_UpdateInGameEventHistory.h"

// ORIG 全局作用域真实类 DB_SavePcRoomPlayTimeEvent
// （_ZN26DB_SavePcRoomPlayTimeEvent11makeRequestEjjh，T 0x08447bd0；
//   权威类定义与实例注册在 W8LT_DBTask.cpp，makeRequest 定义同 TU）。
struct DB_SavePcRoomPlayTimeEvent
{
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};

namespace EventClassify
{

// ============================================================================
// CObjectBringUp
// ============================================================================

// verifyGiveBringUpReward（ORIG 0x0810fe1a）
// ORIG 分支形态：multiple==0 分支 if/return（P56）；否则 OR 短路链直接 jcc
// 汇聚统一 fail 出口（P133），Ghidra else-if 链。
bool CObjectBringUp::verifyGiveBringUpReward(
    ObjectBringUpScript::ObjectBringUpRule& rule, int count)
{
    if (rule.m_rewardMutipleNumber == 0)
    {
        if (rule.m_rewardStartCount == count)
            return true;
        return false;
    }
    if (rule.m_rewardStartCount > count ||
        (rule.m_rewardEndCount < count && rule.m_rewardEndCount != 0) ||
        (count - rule.m_rewardStartCount) % rule.m_rewardMutipleNumber != 0)
        return false;
    return true;
}

// verifyMaterialItemByRule（ORIG 0x0810fcea）
// 不缓存 script（P36：ORIG 每次重读 this+0x1c）；flag!=0 分支 ORIG 对
// size()/getCurUseBringUpMaterialTotal 各调用两次（P48 无 CSE），条件先 size
// 后 total（setbe 方向）。
bool CObjectBringUp::verifyMaterialItemByRule(CUser* user,
                                              unsigned int itemIdx)
{
    if (static_cast<ObjectBringUpScript*>(m_script1c)->m_exchangeRuleFlag == 0)
    {
        for (std::vector<int>::iterator it =
                 static_cast<ObjectBringUpScript*>(m_script1c)
                     ->m_materialItemIdxs.begin();
             it != static_cast<ObjectBringUpScript*>(m_script1c)
                       ->m_materialItemIdxs.end();
             ++it)
        {
            if ((unsigned int)*it == itemIdx)
                return true;
        }
        return false;
    }

    int idx;
    if (static_cast<ObjectBringUpScript*>(m_script1c)
            ->m_materialItemIdxs.size() <=
        (unsigned int)user->getCurUseBringUpMaterialTotal())
        idx = (int)static_cast<ObjectBringUpScript*>(m_script1c)
                  ->m_materialItemIdxs.size() - 1;
    else
        idx = (int)user->getCurUseBringUpMaterialTotal();
    if ((unsigned int)static_cast<ObjectBringUpScript*>(m_script1c)
            ->m_materialItemIdxs[idx] != itemIdx)
        return false;
    return true;
}

// sendBringUpReward（ORIG 0x0810fe90）
// ORIG 固定索引 1 比较 pair.second+0x2c = Action_SendMail::m_itemIdx（+0x20，
// 相对 pair 为 +0x2c；非 m_gold）。比较装载序：先 operator[]([1]) 后
// operator->(当前元素)。getBringUpRewardFlag() 用 if(flag) continue 布局
// （P59：call;test;jne 循环尾）。日志参数与 __PRETTY_FUNCTION__ 照 ORIG。
bool CObjectBringUp::sendBringUpReward(CUser* user,
                                       std::vector<int>& rewardVec,
                                       unsigned char& flag)
{
    if (user == 0)
    {
        LogManager::logFormat(
            1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7ad,
            "CObjectBringUp::sendBringUpReward(pUser(NULL))");
        return false;
    }

    for (::std::vector< ::std::pair<ObjectBringUpScript::ObjectBringUpRule,
                                  ::Action_SendMail> >::iterator it =
             static_cast<ObjectBringUpScript*>(m_script1c)
                 ->m_sendMails28.begin();
         it != static_cast<ObjectBringUpScript*>(m_script1c)
                   ->m_sendMails28.end();
         ++it)
    {
        if (!verifyGiveBringUpReward(
                it->first, (int)user->getCurUseBringUpMaterialTotal()))
            continue;

        if (static_cast<ObjectBringUpScript*>(m_script1c)
                ->m_sendMails28[1].second.m_itemIdx == it->second.m_itemIdx)
        {
            if (user->getBringUpRewardFlag())
                continue;

            CEventActionMng actionMng;
            int result = actionMng.process_action_send_mail(
                user, it->second, false);
            if (result != 0)
            {
                LogManager::logFormat(
                    1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7c3,
                    "CObjectBringUp::sendBringUpReward(m_id:%d, "
                    "charac_no:%d, reward:%d)",
                    it->second.m_itemIdx, (int)user->getCurCharacNo(),
                    (int)user->get_acc_id());
                return false;
            }
            std::copy(it->second.m_vec38.begin(),
                      it->second.m_vec38.end(),
                      std::back_inserter(rewardVec));
            it->second.m_vec38.clear();
            ++flag;
            user->setBringUpRewardFlag(true);

            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("EventClassify.cpp",
                                                   0x7cc),
                true);
            guard.operator*()->operator<<((int)0x2ca);
            guard.operator*()->operator<<((int)-1);
            SIG_OBJECT_BRING_UP* sig =
                guard.operator->()->GetInBuffer<SIG_OBJECT_BRING_UP>();
            sig->m_accId = user->get_acc_id();
            sig->m_characNo = user->getCurCharacNo();
            sig->m_serverGroup =
                (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
            sig->m_usedMaterialCnt =
                user->getCurUseBringUpMaterialCount();
            sig->m_rewardFlag = user->getBringUpRewardFlag();
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            user->resetCurUseBringUpMaterialCount();
        }
        else
        {
            CEventActionMng actionMng;
            int result = actionMng.process_action_send_mail(
                user, it->second, false);
            if (result != 0)
            {
                LogManager::logFormat(
                    1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7e2,
                    "CObjectBringUp::sendBringUpReward(m_id:%d, "
                    "charac_no:%d, reward:%d)",
                    it->second.m_itemIdx, (int)user->getCurCharacNo(),
                    (int)user->get_acc_id());
                return false;
            }
            std::copy(it->second.m_vec38.begin(), it->second.m_vec38.end(),
                      std::back_inserter(rewardVec));
            it->second.m_vec38.clear();
            ++flag;
        }
    }
    return true;
}

// ============================================================================
// CUsedFatigueGiveItem —— get_user_type（ORIG 0x08110eec；报告缺失，
// objdump 直接转写：movzbl 0xc(script)=UsedFatigueGiveItemScript::m_userType）
// ============================================================================
char CUsedFatigueGiveItem::get_user_type() const
{
    return static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_userType;
}

// ============================================================================
// CPcRoomPlayTime::processEventPcRoomPlayTime（ORIG 0x0811089c）
// ============================================================================
int CPcRoomPlayTime::processEventPcRoomPlayTime(CUser* user)
{
    user->collectPcRoomPlayTime();               // UNLINKED_CALLEE（T 待落地）
    if ((unsigned int)user->getTotalPcRoomPlayTime() <
        (unsigned int)static_cast<PcBangPlayTimeScript*>(m_script1c)
            ->m_playTime)
        return 0x13;

    user->incPcRoomPlayTimeRewardCnt();
    register unsigned char rewardCnt =
        user->getPcRoomPlayTimeRewardCnt();
    register int totalTime = user->getTotalPcRoomPlayTime();
    register int characNo = (int)user->getCurCharacNo();
    register int accId = (int)user->get_acc_id();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x8ea, 0);
    trace(
        "<PCROOM_PLAY_TIME_EVENT> send_reward m_id(%d), charac_no(%d), "
        "total_time(%d), reward_cnt(%d)",
        accId, characNo, totalTime, (int)rewardCnt);
    user->resetTotalPcRoomPlayTime();

    ::DB_SavePcRoomPlayTimeEvent::makeRequest(
        (unsigned int)user->get_acc_id(),
        (unsigned int)user->getTotalPcRoomPlayTime(),
        user->getPcRoomPlayTimeRewardCnt());

    Packet_PcRoomPlayTimeReward packet;
    packet.m_field0a =
        G_GameWorld()->IsIntegratedPvPBaseChannel()
            ? (unsigned char)user->GetServerGroup()
            : (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    packet.m_accId = user->get_acc_id();
    packet.m_characNo = user->getCurCharacNo();

    GlobalData::s_monitor_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x13);
    return 0;
}

// ============================================================================
// CEventScriptMng
// ============================================================================

// send_event_init_record_data（ORIG 0x0810cb2c）
// history 强制 const map*（ORIG begin/end 走 const 版本，无 iterator 转换）。
void CEventScriptMng::send_event_init_record_data(CUser* user)
{
    if (user == 0)
        return;

    const std::map<int, std::pair<int, int> >* history =
        static_cast<const std::map<int, std::pair<int, int> >*>(
            user->get_ingame_event_history());

    PacketGuard packet;
    packet.put_header(0, 0x177);
    packet.put_byte((int)(signed char)history->size());
    std::map<int, std::pair<int, int> >::const_iterator it =
        history->begin();
    std::map<int, std::pair<int, int> >::const_iterator end =
        history->end();
    while (it != end)
    {
        packet.put_short((int)(short)it->first);
        packet.put_int(it->second.first);
        ++it;
    }
    packet.finalize(true);
    user->Send(packet);
}

// process_update_ingame_history（ORIG 0x0810c82e）
// updates/history 均强制 const map*（ORIG begin/end/find 全为 const 版本，
// _Rb_tree_const_iterator 直返无转换）。makeRequest 参数装载：右→左
// hit->second.second、hit->second.first、it->first（updates 迭代器 key）、
// get_acc_id —— 与 ORIG +0x0b8..+0x0ed 压栈一致（当前实现曾误用 hit->first
// 且 second.first/second.second 颠倒）。ORIG 的 ++it 位于外层 if 体内
// （second==false 时不递增——忠实转写 ORIG 控制流）。尾部不清 update 表。
void CEventScriptMng::process_update_ingame_history(CUser* user)
{
    if (user == 0)
        return;

    std::map<int, bool>::const_iterator it =
        static_cast<const std::map<int, bool>*>(
            user->get_ingame_event_history_update())->begin();

    while (it != static_cast<const std::map<int, bool>*>(
                      user->get_ingame_event_history_update())->end())
    {

        if (it->second)
        {
            std::map<int, std::pair<int, int> >::const_iterator hit =
                static_cast<const std::map<int, std::pair<int, int> >*>(
                    user->get_ingame_event_history())
                    ->find(it->first);
            std::map<int, std::pair<int, int> >::const_iterator hend =
                static_cast<const std::map<int, std::pair<int, int> >*>(
                    user->get_ingame_event_history())->end();
            if (hit != hend)
            {
                DB_UpdateInGameEventHistory::makeRequest(  // UNLINKED_CALLEE
                    user->get_acc_id(), it->first,
                    hit->second.first, hit->second.second);
            }
            ++it;
        }
    }
}

// ============================================================================
// CExchangeRandomItemReward
// ============================================================================

// getRewardItem（ORIG 0x0810ee40）
// result 声明于循环前（ORIG local_10=0）；非 const iterator（P96）。
int CExchangeRandomItemReward::getRewardItem(
    std::vector<std::pair<int, int> >& out)
{
    int result = 0;
    for (std::vector<int>::iterator it =
             static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                 ->m_ints18.begin();
         it != static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                   ->m_ints18.end();
         ++it)
    {
        CEventActionMng actionMng;
        result = actionMng.getResultItemFromLottery((unsigned int)*it,
                                                    out);
        if (result != 0)
            return result;
    }
    return 0;
}

// verifyMaterialItemByRule（ORIG 0x0810eb30）
// flag08==0：内层命中即收集并 break；外层在 materials 非空时终止整个双重循环。
// flag08!=0：全量收集；收集数!=规则数 → 0x16。不缓存 script/rule（P36/P48：
// ORIG 对 pairs0c[r] 三次 operator[]）。命中条件两元素装载：first 先、second 后。
int CExchangeRandomItemReward::verifyMaterialItemByRule(
    CUser* user, std::vector<int>& slots,
    std::vector<std::pair<int, int> >& materials)
{
    if (static_cast<ExchangeRandomItemRewardScript*>(m_script1c)->m_flag08 == 0)
    {
        size_t r = 0;
        while (r < static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                       ->m_pairs0c.size())
        {
            size_t s = 0;
            while (s < slots.size())
            {
                Inven_Item slotItem =
                    user->getCurCharacInvenR()->GetInvenSlot(1, slots[s]);
                if ((unsigned int)static_cast<ExchangeRandomItemRewardScript*>(
                        m_script1c)->m_pairs0c[r].first ==
                        (unsigned int)slotItem.m_addInfo &&
                    static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                            ->m_pairs0c[r].second <= slotItem.m_addInfo2)
                {
                    materials.push_back(std::make_pair(
                        slots[s],
                        static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                            ->m_pairs0c[r].second));
                    break;
                }
                ++s;
            }
            if (!materials.empty())
                break;
            ++r;
        }
    }
    else
    {
        size_t r = 0;
        while (r < static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                       ->m_pairs0c.size())
        {
            size_t s = 0;
            while (s < slots.size())
            {
                Inven_Item slotItem =
                    user->getCurCharacInvenR()->GetInvenSlot(1, slots[s]);
                if ((unsigned int)static_cast<ExchangeRandomItemRewardScript*>(
                        m_script1c)->m_pairs0c[r].first ==
                        (unsigned int)slotItem.m_addInfo &&
                    static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                            ->m_pairs0c[r].second <= slotItem.m_addInfo2)
                {
                    materials.push_back(std::make_pair(
                        slots[s],
                        static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                            ->m_pairs0c[r].second));
                }
                ++s;
            }
            ++r;
        }
        if (materials.size() !=
            static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                ->m_pairs0c.size())
            return 0x16;
    }
    if (materials.empty())
        return 0x16;
    return 0;
}

// deleteUsedMaterialItem（ORIG 0x0810eede）
// ORIG 无 failed 标志：delete_item 失败路径内联处理（回包循环 + logFormat
// 0x640 + return 8），成功路径 fall-through push_back+++i。回包循环
// 内 newSlot<0 时 logFormat 0x63d。全部 __PRETTY_FUNCTION__。
int CExchangeRandomItemReward::deleteUsedMaterialItem(
    CUser* user, std::vector<std::pair<int, int> >& materials,
    std::vector<std::pair<int, int> >& returnItems)
{
    size_t i = 0;
    while (materials.size() > i)
    {
        Inven_Item slotItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, materials[i].first);
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, materials[i].first, materials[i].second,
                (eItemDelReason)3, true))
        {
            if (!returnItems.empty())
            {
                for (std::vector<std::pair<int, int> >::iterator it =
                         returnItems.begin();
                     it != returnItems.end(); ++it)
                {
                    Inven_Item item;
                    item.m_addInfo = it->first;
                    G_CDataManager()->m_itemList->create_item(
                        (ENUM_ITEM_CREATE_TYPE)0, item, it->second);
                    const int newSlot =
                        user->getCurCharacInvenW()->insertItemIntoInventory(
                            item, (eItemAddReason)0xd, true, true);
                    if (newSlot < 0)
                    {
                        LogManager::logFormat(
                            1, "EventClassify.cpp", __PRETTY_FUNCTION__,
                            0x63d,
                            "CExchangeRandomItemReward::"
                            "deleteUsedMaterialItem, return_item failed , "
                            "User ch =%d , %d %d",
                            (int)user->getCurCharacNo(), it->first,
                            it->second);
                    }
                }
            }
            const std::pair<int, int>& bad = materials[i];
            LogManager::logFormat(
                1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x640,
                "CExchangeRandomItemReward::deleteUsedMaterialItem failed "
                ", User ch =%d , %d %d",
                (int)user->getCurCharacNo(), bad.first, bad.second);
            return 8;
        }
        returnItems.push_back(std::make_pair(slotItem.m_addInfo,
                                             materials[i].second));
        ++i;
    }
    return 0;
}

// sendRandomItemReward（ORIG 0x0810f26e）
// set_item(mail,p.first,p.second) → 发信(true) → ++flag；结束把 mail.m_vec38
// （脚本 +0x5c）移交 recordVec 并 clear。非 const iterator（P96）。
int CExchangeRandomItemReward::sendRandomItemReward(
    CUser* user, std::vector<std::pair<int, int> >& rewards,
    std::vector<int>& recordVec, unsigned char& flag)
{
    int result = 0;
    for (std::vector<std::pair<int, int> >::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
            ->m_mail.set_item(it->first, it->second);
        CEventActionMng actionMng;
        result = actionMng.process_action_send_mail(
            user, static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                      ->m_mail, true);
        if (result != 0)
            return result;
        ++flag;
    }
    recordVec = static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                    ->m_mail.m_vec38;
    static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
        ->m_mail.m_vec38.clear();
    return 0;
}

// ============================================================================
// CAvatarDisjointRandomReward
// ============================================================================

// checkAvatarDisjointError（ORIG 0x0810f3a4）
// get_grade 两次独立调用（P48：ORIG +0x059/+0x068 双 call）。
// 注意：ORIG 虚调用 getUsableItemType 在 vtable+0x24，当前 CItem.h 虚序生成
// +0x20 —— 共享头（CItem.h）虚函数序与 ORIG vtable 差 2 槽，需全局修复，
// 见报告；此处保持头现状。
int CAvatarDisjointRandomReward::checkAvatarDisjointError(
    CUser* user, unsigned short slot, Inven_Item& item, CItem const* citem)
{
    const int enable = user->isEnableAvatarSocketAction();
    if (enable != 0)
        return enable;

    if (user->CheckItemLock(2, (int)slot))
        return 0xd5;

    if (citem->get_grade() == 0 || citem->get_grade() > 3)
        return 0x13;

    if (!citem->getUsableItemType(2))            // ORIG vtable slot +0x24
        return 0x17;

    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetExpireDate(
            item.m_addInfo2) != 0)
        return 0x17;

    // ORIG：isfindEventAvatar(G_CDataManager()+0xb508=CDataManager::
    // m_eventAvatarList, item.m_addInfo)
    if (G_CDataManager()->m_eventAvatarList.isfindEventAvatar(
            item.m_addInfo))
        return 0x17;

    if (item.m_upgradeSep.IsTradeRestriction())
        return 0x17;
    return 0;
}

// deleteAvatarItem（ORIG 0x0810f4e2）
int CAvatarDisjointRandomReward::deleteAvatarItem(
    CUser* user, unsigned short slot, Inven_Item& item, CItem const* citem)
{
    if (!user->getCurCharacInvenW()->delete_item((INVEN_TYPE)2, (int)slot, 1,
                                                 (eItemDelReason)6, true))
        return 0x16;
    if (!user->getCurCharacInvenW()->GetAvatarItemMgrW()->UnRegistItem(
            item.m_addInfo2))
        return 0x16;

    user->getCurCharacInvenW()->SendAvatarEvent(0x36, item.m_addInfo2, 5,
                                                (int)slot + 10, 0);
    DB_UpdateAvatarHistory::makeRequest(user->getCurCharacNo(),
                                        item.m_addInfo2, "");
    SendDisjointAvatarStatistic(citem);
    return 0;
}

// SendDisjointAvatarStatistic（ORIG 0x0810f5f4）
// ORIG 用 if/else 赋值局部 m_gradeIdx（rarity==2 ? 2 : grade-1）——P47 按
// if-return/if-else 形态；GetServerProxy(s_statistic_proxy_mgr,0) 直链。
void CAvatarDisjointRandomReward::SendDisjointAvatarStatistic(
    CItem const* citem)
{
    Packet_Avater_Disjoint_Statistic packet;
    packet.m_field0a = 1;
    int gradeIdx;
    if (citem->get_rarity() == 2)
        gradeIdx = 2;
    else
        gradeIdx = citem->get_grade() - 1;
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = citem->GetItemType();
    packet.m_flagA = 1;
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x28);
}

// getRewardItem（ORIG 0x0810f694）
// grade 为 char 局部直存（ORIG mov %al,-0x11）；map 迭代器非 const。
int CAvatarDisjointRandomReward::getRewardItem(
    CItem const* citem, std::vector<std::pair<int, int> >& out)
{
    const char grade = (char)citem->get_grade();
    std::map<char, unsigned int>::iterator it =
        static_cast<AvatarDisjointRandomRewardScript*>(m_script1c)
            ->m_map08.find(grade);
    if (it == static_cast<AvatarDisjointRandomRewardScript*>(m_script1c)
                  ->m_map08.end())
        return 0x15;
    CEventActionMng actionMng;
    return actionMng.getResultItemFromLottery(it->second, out);
}

// SendCreateEmblemStatistic（ORIG 0x0810f742）
// ORIG 写入序：先 put_grade（slots[i+1]）后 put_value（slots[i]）——get_grade
// 调用在 value 读取之前（+0x09c vs +0x0ae）。size>5 判断（cmp $5;seta）。
void CAvatarDisjointRandomReward::SendCreateEmblemStatistic(
    std::vector<std::pair<int, int> >& rewards)
{
    Packet_Emblem_Create_Statistic packet;
    if (rewards.size() > 5)
        return;

    int n = 0;
    for (std::vector<std::pair<int, int> >::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        CItem* citem = G_CDataManager()->find_item(it->first);
        if (citem == 0)
            return;
        if (citem->GetItemType() == 0x14)
        {
            packet.m_slots[n + 1] = citem->get_grade();
            packet.m_slots[n] = it->second;
            ++n;
            if (n > 5)
                break;
        }
    }
    if (n <= 0)
        return;
    packet.m_slots[0] = n;
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
}

// sendRandomItemReward（ORIG 0x0810f88c）
// m_mail @ 脚本+0x20；发信(false)；非 const iterator。
int CAvatarDisjointRandomReward::sendRandomItemReward(
    CUser* user, std::vector<std::pair<int, int> >& rewards)
{
    int result = 0;
    for (std::vector<std::pair<int, int> >::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        static_cast<AvatarDisjointRandomRewardScript*>(m_script1c)
            ->m_mail.set_item(it->first, it->second);
        CEventActionMng actionMng;
        result = actionMng.process_action_send_mail(
            user, static_cast<AvatarDisjointRandomRewardScript*>(m_script1c)
                      ->m_mail, false);
        if (result != 0)
            return result;
    }
    return 0;
}

}  // namespace EventClassify
