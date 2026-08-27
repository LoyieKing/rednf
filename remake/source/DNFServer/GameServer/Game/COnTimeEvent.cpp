// COnTimeEvent.cpp —— 按报告 docs/class_func_reports/COnTimeEvent/*.md 忠实转写。
#include "COnTimeEvent.h"

#include <cstdio>

#include "GameTypes.h"          // cMyTrace
#include "CMyFileLog.h"
#include "CEnvironment.h"
#include "CUser.h"
#include "GameWorld.h"
#include "DB_InsertOnTimeRewardUserLog.h"

// ORIG 自由函数（权威声明 CInventory.h:408；按项目惯例局部声明）。
char* NumberToString(unsigned int value, int radix);

// ===================== 生命周期 =====================

// ORIG C1 @0x081bb826：基类 ctor → CRewardUserList ctor(+0x14) →
// +0x0c=0、+0x10=1 → clear()。
COnTimeEvent::COnTimeEvent()
    : m_ontimeId(0),
      m_rewardState(1),
      m_rewardEndTime(0),
      m_itemNo(0),
      m_itemCount(0)
{
    m_rewardUsers.clear();
}

void COnTimeEvent::StartEvent()
{
    SetEventFlag(true);
}

void COnTimeEvent::EndEvent()
{
    cMyTrace trace("virtual void COnTimeEvent::EndEvent()", 0x4d, 0);
    trace("COnTimeEvent Event End!");
    // ORIG @0x081bb91a：SetEventFlag(false) 在 cMyTrace 构造前调用。
    SetEventFlag(false);
}

void COnTimeEvent::StartEvent(Word_Param param)
{
    (void)param;
    this->StartEvent();   // ORIG @0x081bb8ca：虚调 slot+0x8（本类 StartEvent）
    cMyTrace trace("virtual void COnTimeEvent::StartEvent(Word_Param)", 0x47, 0);
    trace("COnTimeEvent Event Start!");
}

// ===================== getter/setter =====================

void COnTimeEvent::set_ontime_id(int id) { m_ontimeId = id; }        // +0x0c
int  COnTimeEvent::get_ontime_id() { return m_ontimeId; }
void COnTimeEvent::set_reward_state(int state) { m_rewardState = state; }
int  COnTimeEvent::get_reward_state() { return m_rewardState; }

// ORIG @0x081bb994：*(time_t*)(this+0x54) = time(NULL) + param。
void COnTimeEvent::set_reward_end_time(int span)
{
    m_rewardEndTime = time(0) + span;
}

time_t COnTimeEvent::get_reward_end_time() { return m_rewardEndTime; }

void COnTimeEvent::set_ontime_reward_item(int no, int count)
{
    m_itemNo = no;
    m_itemCount = count;
}
int COnTimeEvent::get_ontime_reward_item_no() { return m_itemNo; }
int COnTimeEvent::get_ontime_reward_item_count() { return m_itemCount; }

unsigned int COnTimeEvent::get_ontime_reward_user_count()
{
    return m_rewardUsers.get_index_size();
}

// ===================== 业务流程 =====================

// ORIG @0x081bbce0（process_set_reward_start）。
void COnTimeEvent::process_set_reward_start(int ontimeId, int itemNo,
                                            int itemCount, int endSpan)
{
    set_reward_state(0);
    set_ontime_id(ontimeId);
    set_ontime_reward_item(itemNo, itemCount);
    set_reward_end_time(endSpan);
    m_rewardUsers.clear();
}

// ORIG @0x081bb9dc。双日志 + 已领过判定。
bool COnTimeEvent::insert_reward_user(CUser* user)
{
    if (user == 0)
        return false;

    char logDir[256];
    memset(logDir, 0, sizeof(logDir));
    sprintf(logDir, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());

    unsigned int characNo = user->get_charac_no(-1);
    char* accStr = NumberToString(user->get_acc_id(), 0);
    {
        CMyFileLog log("bool COnTimeEvent::insert_reward_user(CUser*)", 0x75);
        log(logDir, "On Time Event:On Request Reward m_id(%s) charac_no(%u)",
            accStr, characNo);
    }

    if (user->get_ontime_last_recv_idx() < get_ontime_id())
    {
        m_rewardUsers.insert(user->get_acc_id(), characNo, get_ontime_id());
        user->set_update_ontime_last_recv_idx();
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0);
        return true;
    }
    else
    {
        char errDir[256];
        memset(errDir, 0, sizeof(errDir));
        sprintf(errDir, "./log/%s/OnTimeEvent",
                G_CEnvironment()->get_file_name());

        unsigned int lastIdx = user->get_ontime_last_recv_idx();
        CMyFileLog log("bool COnTimeEvent::insert_reward_user(CUser*)", 0x89);
        log(errDir,
            "On Time Event:Reward(no.%u user-%u)error m_id(%s)charac_no(%u)",
            get_ontime_id(), lastIdx, NumberToString(user->get_acc_id(), 0),
            user->get_charac_no(-1));
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x12);
        return false;
    }
}

// ORIG @0x081bbc64。
bool COnTimeEvent::delete_reward_user(CUser* user)
{
    if (user == 0)
        return false;

    if (get_ontime_id() == user->get_ontime_last_recv_idx())
    {
        m_rewardUsers.delete_mark(user->get_acc_id());
        return true;
    }
    return false;
}

// ORIG @0x081bbd46：遍历 key 队列（上限 0x33 个/次），逐个投递奖励。
bool COnTimeEvent::process_send_reward_to_user()
{
    for (int processed = 0; processed < 0x33; ++processed)
    {
        std::deque<unsigned int>& indexDeque =
            const_cast<std::deque<unsigned int>&>(m_rewardUsers.get_index());
        if (indexDeque.empty())
            return true;

        unsigned int accId = indexDeque.front();
        std::map<unsigned int, RewardUserData>::const_iterator it =
            m_rewardUsers.get_map().find(accId);

        if (it != m_rewardUsers.get_map().end() && it->second.mark != 1)
        {
            CUser* user = G_GameWorld()->find_user_from_world_byaccid(accId);
            if (user != 0)
            {
                if (user->get_charac_no(-1) == it->second.b &&
                    user->get_state() > 2)
                {
                    user->InsertOnTimeEventReward(get_ontime_reward_item_no(),
                                                  get_ontime_reward_item_count());
                    DB_InsertOnTimeRewardUserLog::makeRequest(
                        (int)user->GetUID(), accId, get_ontime_id());
                }
            }
        }
        indexDeque.pop_front();
    }
    return true;
}
