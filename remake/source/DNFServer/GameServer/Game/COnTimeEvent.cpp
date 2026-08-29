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
{
    m_ontimeId = 0;
    m_rewardState = 1;
    m_rewardUsers.clear();
}

void COnTimeEvent::StartEvent()
{
    SetEventFlag(true);
}

void COnTimeEvent::EndEvent()
{
    // ORIG @0x081bb91a：SetEventFlag(false) 在 cMyTrace 构造前调用。
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x4d, 0);
    trace("COnTimeEvent Event End!");
}

void COnTimeEvent::StartEvent(Word_Param param)
{
    (void)param;
    this->StartEvent();   // ORIG @0x081bb8ca：虚调 slot+0x8（本类 StartEvent）
    cMyTrace trace(__PRETTY_FUNCTION__, 0x47, 0);
    trace("COnTimeEvent Event Start!");
}

// ===================== getter/setter =====================

void COnTimeEvent::set_ontime_id(int id) { m_ontimeId = id; }        // +0x0c
int  COnTimeEvent::get_ontime_id() { return m_ontimeId; }
void COnTimeEvent::set_reward_state(int state) { m_rewardState = state; }
int  COnTimeEvent::get_reward_state() { return m_rewardState; }

// ORIG @0x081bb994：*(time_t*)(this+0x54) = time(NULL) + param。
// ORIG 先把 time() 结果落 -0xc 栈槽再回读（time_t t = time(0); 两步）。
void COnTimeEvent::set_reward_end_time(int span)
{
    time_t t = time(0);
    m_rewardEndTime = t + span;
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
// 汇编要点：① 两个 256 数组为 { '\0' } 初始化（rep stos 内联，非 call memset）；
// ② 比较 setg（get_ontime_id() > last_recv_idx()，先求左操作数）；
// ③ insert 实参右→左内联求值 get_ontime_id→get_charac_no→get_acc_id；
// ④ insert 后先 set_ontime_last_recv_idx(get_ontime_id()) 再 set_update；
// ⑤ else 分支求值序 get_charac_no→get_acc_id→NumberToString→last_recv→ontime_id。
bool COnTimeEvent::insert_reward_user(CUser* user)
{
    if (user == 0)
        return false;

    char logDir[256] = { '\0' };
    sprintf(logDir, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());

    register unsigned int characNo = user->get_charac_no(-1);
    register char* accStr = NumberToString(user->get_acc_id(), 0);
    CMyFileLog log(__PRETTY_FUNCTION__, 0x75);
    log(logDir, "On Time Event:On Request Reward m_id(%s) charac_no(%u)",
        accStr, characNo);

    if (get_ontime_id() > user->get_ontime_last_recv_idx())
    {
        m_rewardUsers.insert(user->get_acc_id(), user->get_charac_no(-1),
                             get_ontime_id());
        user->set_ontime_last_recv_idx(get_ontime_id());
        user->set_update_ontime_last_recv_idx();
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0);
        return true;
    }
    else
    {
        char errDir[256] = { '\0' };
        sprintf(errDir, "./log/%s/OnTimeEvent",
                G_CEnvironment()->get_file_name());

        register unsigned int characNo2 = user->get_charac_no(-1);
        register char* accStr2 = NumberToString(user->get_acc_id(), 0);
        register unsigned int lastIdx = user->get_ontime_last_recv_idx();
        register unsigned int ontimeId = get_ontime_id();
        CMyFileLog log(__PRETTY_FUNCTION__, 0x89);
        log(errDir,
            "On Time Event:Reward(no.%u user-%u)error m_id(%s)charac_no(%u)",
            ontimeId, lastIdx, accStr2, characNo2);
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
// 汇编要点：① while (!m_index.empty()) 前置 jmp 循环头（P25 while 形态）；
// ② 循环尾 ++processed 先、pop_front 后、cmpl $0x32; jg break；
// ③ iterator 函数头默认构造（P28 声明序 it→user→processed）；
// ④ 条件嵌套 if（两个独立 je，非 && 汇聚）；⑤ 比较用 it->second.a（charac_no）。
bool COnTimeEvent::process_send_reward_to_user()
{
    std::map<unsigned int, RewardUserData>::const_iterator it;
    CUser* user = 0;
    int processed = 0;

    while (!m_rewardUsers.m_index.empty())
    {
        it = m_rewardUsers.m_map.find(m_rewardUsers.m_index.front());

        if (it != m_rewardUsers.m_map.end())
        {
            if (it->second.mark != 1)
            {
                if ((user = G_GameWorld()->find_user_from_world_byaccid(
                         it->first)) != 0)
                {
                    if (user->get_charac_no(-1) == it->second.a &&
                        user->get_state() > 2)
                    {
                        user->InsertOnTimeEventReward(
                            get_ontime_reward_item_no(),
                            get_ontime_reward_item_count());
                        DB_InsertOnTimeRewardUserLog::makeRequest(
                            (int)user->GetUID(), it->first, get_ontime_id());
                    }
                }
            }
        }
        ++processed;
        m_rewardUsers.m_index.pop_front();
        if (processed > 0x32)
            break;
    }
    return true;
}
