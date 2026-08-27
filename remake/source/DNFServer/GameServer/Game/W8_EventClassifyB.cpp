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

namespace EventClassify
{

namespace
{

// ORIG DB_SavePcRoomPlayTimeEvent::makeRequest(uint,uint,uchar) T 0x08447bd0；
// 权威头未建，此处最小声明，符号由后续 DB 批次提供（UNLINKED_CALLEE）。
struct DB_SavePcRoomPlayTimeEvent
{
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};

#pragma pack(push, 1)

// Packet_PcRoomPlayTimeReward —— ctor W 0x08110c0a：
// PacketHeader(0x27ec,0x13) + byte@+0xa=0 + dword@+0xb=0 + dword@+0xf=0。
class Packet_PcRoomPlayTimeReward : public PacketHeader
{
public:
    Packet_PcRoomPlayTimeReward()
        : PacketHeader(static_cast<unsigned short>(0x27ec),
                       static_cast<unsigned short>(0x13))
    {
    }

    unsigned char m_field0a;     // +0x0a（ctor 清 0；调用方写 serverGroup）
    unsigned int  m_accId;       // +0x0b
    unsigned int  m_characNo;    // +0x0f
};                               // sizeof == 0x13

// Packet_Emblem_Create_Statistic —— ctor W 0x08110bdc：header(0x17a4,0x36)
// 后清零 dword@+0xa。ORIG 循环内写入地址为 -0x44+4i(value/条数位) 与
// -0x40+4i(grade)：相邻槽交错共享（value_{i+1} 与 grade_i 同槽）。以槽数组
// 原样建模：slots[0] 尾写条数、slots[j]=value_j、slots[j+1]=grade_j。
class Packet_Emblem_Create_Statistic : public PacketHeader
{
public:
    Packet_Emblem_Create_Statistic()
        : PacketHeader(static_cast<unsigned short>(0x17a4),
                       static_cast<unsigned short>(0x36))
    {
        std::memset(m_slots, 0, sizeof(m_slots));
    }

    void put_value(int i, int v) { m_slots[i] = v; }         // slot j=i
    void put_grade(int i, int g) { m_slots[i + 1] = g; }     // slot j=i+1
    void put_count(int n) { m_slots[0] = n; }                // 尾写覆盖首槽

    int m_slots[11];             // +0x0a..（0x2c 字节 = 11×int）
};                               // sizeof == 0x36

#pragma pack(pop)

inline CStatisticServerProxy* statisticProxyGroup0()
{
    return GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)0);
}

// ORIG 取 G_CEnvironment()+0x378 字节 = Server_Envir(+0x198)::m_serverGroup
// (+0x1e0) 低字节。
inline unsigned char envServerGroupByte()
{
    return (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
}

}  // namespace

// ============================================================================
// CObjectBringUp
// ============================================================================

// verifyGiveBringUpReward（ORIG 0x0810fe1a）
bool CObjectBringUp::verifyGiveBringUpReward(
    ObjectBringUpScript::ObjectBringUpRule& rule, int count)
{
    if (rule.m_rewardMutipleNumber == 0)
        return rule.m_rewardStartCount == count;
    if (rule.m_rewardStartCount > count)
        return false;
    if (rule.m_rewardEndCount < count && rule.m_rewardEndCount != 0)
        return false;
    if ((count - rule.m_rewardStartCount) % rule.m_rewardMutipleNumber != 0)
        return false;
    return true;
}

// verifyMaterialItemByRule（ORIG 0x0810fcea）
bool CObjectBringUp::verifyMaterialItemByRule(CUser* user,
                                              unsigned int itemIdx)
{
    ObjectBringUpScript* script =
        static_cast<ObjectBringUpScript*>(m_script1c);
    if (script->m_exchangeRuleFlag == 0)
    {
        for (std::vector<int>::const_iterator it =
                 script->m_materialItemIdxs.begin();
             it != script->m_materialItemIdxs.end(); ++it)
        {
            if ((unsigned int)*it == itemIdx)
                return true;
        }
        return false;
    }

    int idx;
    const unsigned char total = user->getCurUseBringUpMaterialTotal();
    if ((unsigned int)total < script->m_materialItemIdxs.size())
        idx = (int)total;
    else
        idx = (int)script->m_materialItemIdxs.size() - 1;
    return (unsigned int)script->m_materialItemIdxs[idx] == itemIdx;
}

// sendBringUpReward（ORIG 0x0810fe90）
bool CObjectBringUp::sendBringUpReward(CUser* user,
                                       std::vector<int>& rewardVec,
                                       unsigned char& flag)
{
    if (user == 0)
    {
        LogManager::logFormat(
            1, "EventClassify.cpp",
            "bool EventClassify::CObjectBringUp::sendBringUpReward(CUser*, "
            "IntVector&, unsigned char&)", 0x7ad,
            "CObjectBringUp::sendBringUpReward(pUser(NULL))");
        return false;
    }

    ObjectBringUpScript* script =
        static_cast<ObjectBringUpScript*>(m_script1c);

    for (std::vector<std::pair<ObjectBringUpScript::ObjectBringUpRule,
                               Action_SendMail> >::iterator it =
             script->m_sendMails28.begin();
         it != script->m_sendMails28.end(); ++it)
    {
        const unsigned char usedTotal =
            user->getCurUseBringUpMaterialTotal();
        if (!verifyGiveBringUpReward(it->first, (int)usedTotal))
            continue;

        // ORIG：vector::operator[](script→+0x28 向量, 1).second.m_gold 与当前
        // 元素比较（固定索引 1，忠实转写）。
        if (it->second.m_gold == script->m_sendMails28[1].second.m_gold)
        {
            if (!user->getBringUpRewardFlag())
            {
                CEventActionMng actionMng;
                int result = actionMng.process_action_send_mail(
                    user, it->second, false);
                if (result != 0)
                {
                    LogManager::logFormat(
                        1, "EventClassify.cpp",
                        "bool EventClassify::CObjectBringUp::sendBringUpReward"
                        "(CUser*, IntVector&, unsigned char&)", 0x7c3,
                        "CObjectBringUp::sendBringUpReward(m_id:%d, "
                        "charac_no:%d, reward:%d)",
                        it->second.m_gold, (int)user->getCurCharacNo(),
                        (int)user->get_acc_id());
                    return false;
                }
                std::copy(it->second.m_vec38.begin(),
                          it->second.m_vec38.end(),
                          std::back_inserter(rewardVec));
                it->second.m_vec38.clear();
                ++flag;
                user->setBringUpRewardFlag(true);

                Stream* stream =
                    GlobalData::s_stream_pool->Acquire("EventClassify.cpp",
                                                       0x7cc);
                CStreamGuard guard(stream, true);
                guard.operator*()->operator<<((int)0x2ca);
                guard.operator*()->operator<<((int)-1);
                SIG_OBJECT_BRING_UP* sig =
                    guard.GetInBuffer<SIG_OBJECT_BRING_UP>();
                sig->m_accId = user->get_acc_id();
                sig->m_characNo = user->getCurCharacNo();
                sig->m_serverGroup = envServerGroupByte();
                sig->m_usedMaterialCnt =
                    user->getCurUseBringUpMaterialCount();
                sig->m_rewardFlag = user->getBringUpRewardFlag();
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                user->resetCurUseBringUpMaterialCount();
            }
        }
        else
        {
            CEventActionMng actionMng;
            int result = actionMng.process_action_send_mail(
                user, it->second, false);
            if (result != 0)
            {
                LogManager::logFormat(
                    1, "EventClassify.cpp",
                    "bool EventClassify::CObjectBringUp::sendBringUpReward("
                    "CUser*, IntVector&, unsigned char&)", 0x7e2,
                    "CObjectBringUp::sendBringUpReward(m_id:%d, "
                    "charac_no:%d, reward:%d)",
                    it->second.m_gold, (int)user->getCurCharacNo(),
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
    UsedFatigueGiveItemScript* script =
        static_cast<UsedFatigueGiveItemScript*>(m_script1c);
    return (char)script->m_userType;
}

// ============================================================================
// CPcRoomPlayTime::processEventPcRoomPlayTime（ORIG 0x0811089c）
// ============================================================================
int CPcRoomPlayTime::processEventPcRoomPlayTime(CUser* user)
{
    user->collectPcRoomPlayTime();               // UNLINKED_CALLEE（T 待落地）
    const int totalTime = user->getTotalPcRoomPlayTime();
    PcBangPlayTimeScript* script =
        static_cast<PcBangPlayTimeScript*>(m_script1c);
    if (totalTime < script->m_playTime)
        return 0x13;

    user->incPcRoomPlayTimeRewardCnt();
    cMyTrace trace(
        "int EventClassify::CPcRoomPlayTime::processEventPcRoomPlayTime("
        "CUser*)", 0x8ea, 0);
    trace(
        "<PCROOM_PLAY_TIME_EVENT> send_reward m_id(%d), charac_no(%d), "
        "total_time(%d), reward_cnt(%d)",
        user->get_acc_id(), (int)user->getCurCharacNo(),
        user->getTotalPcRoomPlayTime(),
        (int)user->getPcRoomPlayTimeRewardCnt());
    user->resetTotalPcRoomPlayTime();

    DB_SavePcRoomPlayTimeEvent::makeRequest(     // UNLINKED_CALLEE
        (unsigned int)user->get_acc_id(),
        (unsigned int)user->getTotalPcRoomPlayTime(),
        user->getPcRoomPlayTimeRewardCnt());

    Packet_PcRoomPlayTimeReward packet;
    packet.m_field0a =
        G_GameWorld()->IsIntegratedPvPBaseChannel()
            ? (unsigned char)user->GetServerGroup()
            : envServerGroupByte();
    packet.m_accId = user->get_acc_id();
    packet.m_characNo = user->getCurCharacNo();

    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(&packet), 0x13);
    return 0;
}

// ============================================================================
// CEventScriptMng
// ============================================================================

// send_event_init_record_data（ORIG 0x0810cb2c）
void CEventScriptMng::send_event_init_record_data(CUser* user)
{
    if (user == 0)
        return;

    std::map<int, std::pair<int, int> >* history =
        static_cast<std::map<int, std::pair<int, int> >*>(
            user->get_ingame_event_history());

    PacketGuard packet;
    packet.put_header(0, 0x177);
    packet.put_byte((int)(signed char)history->size());
    for (std::map<int, std::pair<int, int> >::const_iterator it =
             history->begin();
         it != history->end(); ++it)
    {
        packet.put_short((int)(short)it->first);
        packet.put_int(it->second.first);
    }
    packet.finalize(true);
    user->Send(packet);
}

// process_update_ingame_history（ORIG 0x0810c82e）
// ORIG 尾部不清 update 表——忠实保留。
void CEventScriptMng::process_update_ingame_history(CUser* user)
{
    if (user == 0)
        return;

    std::map<int, bool>* updates =
        static_cast<std::map<int, bool>*>(
            user->get_ingame_event_history_update());
    std::map<int, std::pair<int, int> >* history =
        static_cast<std::map<int, std::pair<int, int> >*>(
            user->get_ingame_event_history());

    for (std::map<int, bool>::const_iterator it = updates->begin();
         it != updates->end(); ++it)
    {
        if (!it->second)
            continue;
        std::map<int, std::pair<int, int> >::iterator hit =
            history->find(it->first);
        if (hit == history->end())
            continue;
        DB_UpdateInGameEventHistory::makeRequest(  // UNLINKED_CALLEE
            user->get_acc_id(), hit->first,
            hit->second.second, hit->second.first);
    }
}

// ============================================================================
// CExchangeRandomItemReward
// ============================================================================

// getRewardItem（ORIG 0x0810ee40）
int CExchangeRandomItemReward::getRewardItem(
    std::vector<std::pair<int, int> >& out)
{
    ExchangeRandomItemRewardScript* script =
        static_cast<ExchangeRandomItemRewardScript*>(m_script1c);
    for (std::vector<int>::const_iterator it = script->m_ints18.begin();
         it != script->m_ints18.end(); ++it)
    {
        CEventActionMng actionMng;
        int result = actionMng.getResultItemFromLottery((unsigned int)*it,
                                                        out);
        if (result != 0)
            return result;
    }
    return 0;
}

// verifyMaterialItemByRule（ORIG 0x0810eb30）
// flag08==0：内层命中即收集并 break；外层在 materials 非空时终止整个双重循环。
// flag08!=0：全量收集；收集数!=规则数 → 0x16。
int CExchangeRandomItemReward::verifyMaterialItemByRule(
    CUser* user, std::vector<int>& slots,
    std::vector<std::pair<int, int> >& materials)
{
    ExchangeRandomItemRewardScript* script =
        static_cast<ExchangeRandomItemRewardScript*>(m_script1c);
    if (script->m_flag08 == 0)
    {
        size_t r = 0;
        while (r < script->m_pairs0c.size())
        {
            const std::pair<int, int>& rule = script->m_pairs0c[r];
            size_t s = 0;
            while (s < slots.size())
            {
                Inven_Item slotItem =
                    user->getCurCharacInvenR()->GetInvenSlot(
                        1, slots[s]);
                if ((unsigned int)rule.first ==
                        (unsigned int)slotItem.m_addInfo &&
                    rule.second <= slotItem.m_addInfo2)
                {
                    materials.push_back(
                        std::make_pair(slots[s], rule.second));
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
        while (r < script->m_pairs0c.size())
        {
            const std::pair<int, int>& rule = script->m_pairs0c[r];
            size_t s = 0;
            while (s < slots.size())
            {
                Inven_Item slotItem =
                    user->getCurCharacInvenR()->GetInvenSlot(
                        1, slots[s]);
                if ((unsigned int)rule.first ==
                        (unsigned int)slotItem.m_addInfo &&
                    rule.second <= slotItem.m_addInfo2)
                {
                    materials.push_back(
                        std::make_pair(slots[s], rule.second));
                    break;
                }
                ++s;
            }
            ++r;
        }
        if (materials.size() != script->m_pairs0c.size())
            return 0x16;
    }
    if (materials.empty())
        return 0x16;
    return 0;
}

// deleteUsedMaterialItem（ORIG 0x0810eede）
// materials = pair<背包1槽号, 数量>。删除成功逐条记录退回对；遇失败时先按
// ORIG 第二循环将已记录项 create_item+insert 回包（reason 0xd,1,1），随后记
// 失败日志（0x640）返回 8。
int CExchangeRandomItemReward::deleteUsedMaterialItem(
    CUser* user, std::vector<std::pair<int, int> >& materials,
    std::vector<std::pair<int, int> >& returnItems)
{
    size_t i = 0;
    bool failed = false;
    while (materials.size() > i)
    {
        Inven_Item slotItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, materials[i].first);
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, materials[i].first, materials[i].second,
                (eItemDelReason)3, true))
        {
            failed = true;
            break;
        }
        returnItems.push_back(std::make_pair(slotItem.m_addInfo,
                                             materials[i].second));
        ++i;
    }

    if (!returnItems.empty())
    {
        for (std::vector<std::pair<int, int> >::iterator it =
                 returnItems.begin();
             it != returnItems.end(); ++it)
        {
            Inven_Item item;
            item.m_addInfo = it->first;
            G_CDataManager()->getItemList()->create_item(
                (ENUM_ITEM_CREATE_TYPE)0, item, it->second);
            const int newSlot =
                user->getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)0xd, true, true);
            if (newSlot < 0)
            {
                LogManager::logFormat(
                    1, "EventClassify.cpp",
                    "int EventClassify::CExchangeRandomItemReward::"
                    "deleteUsedMaterialItem(CUser*, IntPairVector&, "
                    "IntPairVector&)", 0x63d,
                    "CExchangeRandomItemReward::deleteUsedMaterialItem, "
                    "return_item failed , User ch =%d , %d %d",
                    (int)user->getCurCharacNo(), it->first, it->second);
            }
        }
    }

    if (failed && i < materials.size())
    {
        const std::pair<int, int>& bad = materials[i];
        LogManager::logFormat(
            1, "EventClassify.cpp",
            "int EventClassify::CExchangeRandomItemReward::"
            "deleteUsedMaterialItem(CUser*, IntPairVector&, IntPairVector&)",
            0x640,
            "CExchangeRandomItemReward::deleteUsedMaterialItem failed , "
            "User ch =%d , %d %d",
            (int)user->getCurCharacNo(), bad.first, bad.second);
        return 8;
    }
    return 0;
}

// sendRandomItemReward（ORIG 0x0810f26e）
// set_item(mail,p.first,p.second) → 发信(true) → ++flag；结束把 mail.m_vec38
// （脚本 +0x5c）移交 recordVec 并 clear。
int CExchangeRandomItemReward::sendRandomItemReward(
    CUser* user, std::vector<std::pair<int, int> >& rewards,
    std::vector<int>& recordVec, unsigned char& flag)
{
    ExchangeRandomItemRewardScript* script =
        static_cast<ExchangeRandomItemRewardScript*>(m_script1c);
    for (std::vector<std::pair<int, int> >::const_iterator it =
             rewards.begin();
         it != rewards.end(); ++it)
    {
        script->m_mail.set_item(it->first, it->second);
        CEventActionMng actionMng;
        int result = actionMng.process_action_send_mail(user, script->m_mail,
                                                        true);
        if (result != 0)
            return result;
        ++flag;
    }
    recordVec = script->m_mail.m_vec38;
    script->m_mail.m_vec38.clear();
    return 0;
}

// ============================================================================
// CAvatarDisjointRandomReward
// ============================================================================

// checkAvatarDisjointError（ORIG 0x0810f3a4）
int CAvatarDisjointRandomReward::checkAvatarDisjointError(
    CUser* user, unsigned short slot, Inven_Item& item, CItem const* citem)
{
    const int enable = user->isEnableAvatarSocketAction();
    if (enable != 0)
        return enable;

    if (user->CheckItemLock(2, (int)slot))
        return 0xd5;

    const int grade = citem->get_grade();
    if (grade == 0 || grade > 3)
        return 0x13;

    if (!citem->getUsableItemType(2))            // ORIG vtable slot +0x24
        return 0x17;

    WongWork::CAvatarItemMgr* mgr =
        user->getCurCharacInvenW()->GetAvatarItemMgrR();
    if (mgr->GetExpireDate(item.m_addInfo2) != 0)
        return 0x17;

    // ORIG：isfindEventAvatar(G_CDataManager()+0xb508=CDataManager::
    // m_eventAvatarList, item.m_addInfo)
    if (G_CDataManager()->m_eventAvatarList.isfindEventAvatar(
            item.m_addInfo))
        return 0x17;

    if (item.IsTradeRestriction())
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

namespace
{

#pragma pack(push, 1)
// ORIG Packet_Avater_Disjoint_Statistic C1 W 0x08110bae（与本 TU 唯一；
// CompoundDeathLCB.cpp 另持同源镜像类，彼此内部链接互不影响 ODR 可见性依
// 赖各自 TU——此处以匿名命名空间限定防止跨 TU 冲突）。
struct Packet_Avater_Disjoint_Statistic : public PacketHeader
{
    Packet_Avater_Disjoint_Statistic()
        : PacketHeader(static_cast<unsigned short>(0x17a2),
                       static_cast<unsigned short>(0x28))
    {
    }

    unsigned int  m_field0a;   // +0x0a（调用方写 1）
    int           m_gradeIdx;  // +0x0e：rarity==2?2:grade-1
    int           m_subType;   // +0x12：GetItemType 结果
    unsigned char m_flag;      // +0x16：置 1
};                             // sizeof == 0x28
#pragma pack(pop)

}  // namespace

// SendDisjointAvatarStatistic（ORIG 0x0810f5f4）
void CAvatarDisjointRandomReward::SendDisjointAvatarStatistic(
    CItem const* citem)
{
    Packet_Avater_Disjoint_Statistic packet;
    packet.m_field0a = 1;
    packet.m_gradeIdx =
        citem->get_rarity() == 2 ? 2 : citem->get_grade() - 1;
    packet.m_subType = citem->GetItemType();
    packet.m_flag = 1;
    statisticProxyGroup0()->SendPacket(reinterpret_cast<char*>(&packet),
                                       0x28);
}

// getRewardItem（ORIG 0x0810f694）
int CAvatarDisjointRandomReward::getRewardItem(
    CItem const* citem, std::vector<std::pair<int, int> >& out)
{
    AvatarDisjointRandomRewardScript* script =
        static_cast<AvatarDisjointRandomRewardScript*>(m_script1c);
    const int grade = citem->get_grade();
    std::map<char, unsigned int>::const_iterator it =
        script->m_map08.find((char)grade);
    if (it == script->m_map08.end())
        return 0x15;
    CEventActionMng actionMng;
    return actionMng.getResultItemFromLottery(it->second, out);
}

// SendCreateEmblemStatistic（ORIG 0x0810f742）
// 槽位寻址原样保留：entries[i].value→slots[i]、grade→slots[i+1]、尾部
// slots[0]=条数（ORIG -0x44 覆盖行为）。
void CAvatarDisjointRandomReward::SendCreateEmblemStatistic(
    std::vector<std::pair<int, int> >& rewards)
{
    if (rewards.size() >= 6)
        return;

    Packet_Emblem_Create_Statistic packet;
    int n = 0;
    for (std::vector<std::pair<int, int> >::const_iterator it =
             rewards.begin();
         it != rewards.end(); ++it)
    {
        CItem* citem = G_CDataManager()->find_item(it->first);
        if (citem == 0)
            return;
        if (citem->GetItemType() == 0x14)
        {
            packet.put_value(n, it->second);
            packet.put_grade(n, citem->get_grade());
            ++n;
            if (n > 5)
                break;
        }
    }
    if (n <= 0)
        return;
    packet.put_count(n);
    statisticProxyGroup0()->SendPacket(reinterpret_cast<char*>(&packet),
                                       0x36);
}

// sendRandomItemReward（ORIG 0x0810f88c）
int CAvatarDisjointRandomReward::sendRandomItemReward(
    CUser* user, std::vector<std::pair<int, int> >& rewards)
{
    AvatarDisjointRandomRewardScript* script =
        static_cast<AvatarDisjointRandomRewardScript*>(m_script1c);
    for (std::vector<std::pair<int, int> >::const_iterator it =
             rewards.begin();
         it != rewards.end(); ++it)
    {
        script->m_mail.set_item(it->first, it->second);
        CEventActionMng actionMng;
        int result = actionMng.process_action_send_mail(user, script->m_mail,
                                                        false);
        if (result != 0)
            return result;
    }
    return 0;
}

}  // namespace EventClassify
