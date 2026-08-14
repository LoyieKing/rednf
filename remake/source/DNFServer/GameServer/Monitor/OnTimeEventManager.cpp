// df_monitor_r — OnTimeEventManager（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "OnTimeEventManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "Arad_MomijiEvent.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"

struct ST_Req_Ontime_Event_Idx_Update
{
    char m_pad[0xa];
    unsigned int m_itemIndex;  // +0xa
    unsigned int m_itemCount;  // +0xe
    unsigned int m_no;         // +0x12
} __attribute__((packed));

COnTimeEventManager::COnTimeEventManager()
{
    m_app = 0;
    m_eventIdx = 0;
    m_hasIdxFlag = 0;
    m_field40 = 0;
    Clear();
}

COnTimeEventManager::~COnTimeEventManager() {}

void COnTimeEventManager::AttachApp(CApplication* app)
{
    m_app = app;
    m_itemIndex = 0;
    m_itemCount = 0;
    time_t t = time(0);
    register CTaskScheduler::CTask* task = new COnTimeEventIdxLoad(t + 10, 0, this);
    m_app->GetTaskScheduler()->AddTask(task);
}

bool COnTimeEventManager::IsCurState(ENUM_ONTIME_EVENT_STATE state)
{
    return (char)(m_state2c == state);
}

void COnTimeEventManager::ChangeState(ENUM_ONTIME_EVENT_STATE state)
{
    if ((int)state < 0) return;
    if ((int)state > 3) return;
    m_state2c = state;
}

void COnTimeEventManager::SetEventIdx(unsigned int idx)
{
    m_eventIdx = idx;
    m_hasIdxFlag = 1;
}

void COnTimeEventManager::SendContinueTimeToGS()
{
    if (m_app != 0)
    {
        Packet_MTG_OntimeEvent_RewardStart pkt;
        pkt.m_eventIdx = (unsigned int)m_eventIdx;
        pkt.m_itemIndex = m_itemIndex;
        pkt.m_itemCount = m_itemCount;
        pkt.m_remainSec = m_eventDurationMin * 0x3c;
        m_app->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
    }
}

void COnTimeEventManager::UpdateEventIdx()
{
    m_eventIdx = m_eventIdx + 1;
    SendEventIdxToDBMW();
}

unsigned int COnTimeEventManager::GetEvent_Idx() { return (unsigned int)m_eventIdx; }

void COnTimeEventManager::Clear()
{
    m_rewardList.Clear();
    m_startTime = 0;
    m_rewardStartTime = 0;
    m_eventStart = 0;
    m_eventDurationMin = 0;
    m_state2c = ONTIME_EVENT_STATE_REWARD;
}

int CRewardUserList::Insert(unsigned int key)
{
    if (!isExist(key))
    {
        m_map.insert(std::make_pair(key, 0));
        return 1;
    }
    return -1;
}

bool CRewardUserList::isExist(unsigned int key)
{
    std::map<unsigned int, int>::const_iterator it = m_map.find(key);
    if (it == m_map.end())
    {
        return 0;
    }
    return 1;
}

void CRewardUserList::Clear()
{
    m_map.clear();
}

void COnTimeEventManager::SetEventItem(unsigned int idx, unsigned int cnt)
{
    m_itemIndex = idx;
    m_itemCount = cnt;
}

void COnTimeEventManager::StartEvent()
{
    StartEvent(m_eventStart, m_eventDurationMin);
}

void COnTimeEventManager::StartEvent(unsigned int a, unsigned int b)
{
    if (b >= a)
    {
        return;
    }
    unsigned int t = (unsigned int)time(0);
    m_eventStart = (int)a;
    m_eventDurationMin = (int)b;
    m_startTime = (int)t;
    ChangeState(ONTIME_EVENT_STATE_START);
    register CTaskScheduler::CTask* task =
        new COnTimeEventRewardStartTrigger(t, 0, this);
    m_app->GetTaskScheduler()->AddTask(task);
}

void COnTimeEventManager::EndEvent()
{
    if (IsCurState(ONTIME_EVENT_STATE_NONE))
    {
        Packet_MTG_OntimeEvent_RewardEnd pkt;
        m_app->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
    }
    Clear();
}

void COnTimeEventManager::GetCurEventItemByDBMW(unsigned int a, unsigned int b)
{
    m_eventStart = (int)a;
    m_eventDurationMin = (int)b;
    Packet_Req_Ontime_Event_Item pkt;
    if (m_app == 0)
    {
        return;
    }
    CTcpDBServer* db = m_app->Get_ServerHandler()->GetTcpDBServer();
    if (db != 0)
    {
        char* buf = db->makePacketHeader(0x2345, 10);
        char* buf2 = buf;
        db->SendToServer(buf2);
    }
}

void COnTimeEventManager::SendEventIdxToDBMW()
{
    if (m_app != 0)
    {
        CTcpDBServer* db = m_app->Get_ServerHandler()->GetTcpDBServer();
        if (db != 0)
        {
            char* buf = db->makePacketHeader(0x2347, 0x16);
            ST_Req_Ontime_Event_Idx_Update* buf2 = (ST_Req_Ontime_Event_Idx_Update*)buf;
            buf2->m_no = (unsigned int)m_eventIdx;
            buf2->m_itemIndex = m_itemIndex;
            buf2->m_itemCount = m_itemCount;
            db->SendToServer((char*)buf2);
        }
    }
}

int COnTimeEventManager::GetCurIdxByDBMW()
{
    if (m_hasIdxFlag != 0)
    {
        return 1;
    }
    if (m_app != 0)
    {
        Packet_Req_Ontime_Event_Idx pkt;
        m_app->Get_ServerHandler()->SendToDB(&pkt);
        DNF_LOG_SCOPE_LINE(0x164, "./log/OnTimeEvent", "Get_ServerHandler()->SendToDB(packet);");
        time_t t = time(0);
        register CTaskScheduler::CTask* task = new COnTimeEventIdxLoad(t + 10, 0, this);
        m_app->GetTaskScheduler()->AddTask(task);
        return 1;
    }
    return -1;
}

void COnTimeEventManager::OnRewardStart()
{
    register time_t t0 = time(0);
    CMyFileLog log(__FUNCTION__, 0x82);
    log("./log/OnTimeEvent", "On Time Event : On Reward Start Trigger On(%d)", t0);
    if (IsCurState(ONTIME_EVENT_STATE_REWARD))
    {
        puts("On Time Event : Event Off Trigger");
        Clear();
    }
    else
    {
        if (IsCurState(ONTIME_EVENT_STATE_NONE))
        {
        }
        else
        {
            ChangeState(ONTIME_EVENT_STATE_NONE);
            UpdateEventIdx();
            register int t = (int)time(0);
            m_rewardStartTime = t;
            register CTaskScheduler::CTask* task =
                new COnTimeEventRewardEndTrigger((unsigned int)(m_eventDurationMin * 0x3c + t), 0, this);
            m_app->GetTaskScheduler()->AddTask(task);
            register int idx = GetEvent_Idx();
            CMyFileLog log2(__FUNCTION__, 0xa7);
            log2("./log/OnTimeEvent",
                 "On Time Event : On Reward Start Trigger Process Success curidx(%d)", idx);
        }
    }
}

void COnTimeEventManager::OnRewardEnd()
{
    register time_t t0 = time(0);
    CMyFileLog log(__FUNCTION__, 0xae);
    log("./log/OnTimeEvent", "On Time Event : On Reward End Trigger On(%d)\n", t0);
    if (IsCurState(ONTIME_EVENT_STATE_REWARD))
    {
        puts("On Time Event : Event Off Trigger");
        Clear();
    }
    else
    {
        if (IsCurState(ONTIME_EVENT_STATE_START))
        {
        }
        else
        {
            ChangeState(ONTIME_EVENT_STATE_START);
            Packet_MTG_OntimeEvent_RewardEnd pkt;
            m_app->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
            CMyFileLog log2(__FUNCTION__, 0xd9);
            log2("./log/OnTimeEvent", "On Time Event : On Reward End Trigger Process Success");
        }
    }
}

COnTimeEventRewardEndTrigger::COnTimeEventRewardEndTrigger(unsigned int time,
                                                           unsigned int flag,
                                                           COnTimeEventManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

COnTimeEventRewardEndTrigger::~COnTimeEventRewardEndTrigger() {}

COnTimeEventRewardStartTrigger::COnTimeEventRewardStartTrigger(unsigned int time,
                                                               unsigned int flag,
                                                               COnTimeEventManager* mgr) throw()
    : CTask(time, flag)
{
    m_mgr = mgr;
}

COnTimeEventRewardStartTrigger::~COnTimeEventRewardStartTrigger() {}

COnTimeEventIdxLoad::COnTimeEventIdxLoad(unsigned int time, unsigned int flag,
                                         COnTimeEventManager* mgr) throw()
    : CTask(time, flag)
{
    m_mgr = mgr;
}

COnTimeEventIdxLoad::~COnTimeEventIdxLoad() {}

void COnTimeEventRewardStartTrigger::_DoExecute()
{
    m_mgr->OnRewardStart();
}

void COnTimeEventIdxLoad::_DoExecute()
{
    m_mgr->GetCurIdxByDBMW();
}

void COnTimeEventRewardEndTrigger::_DoExecute()
{
    m_mgr->OnRewardEnd();
}

void COnTimeEventManager::EventRewardOff()
{
    if (!IsCurState(ONTIME_EVENT_STATE_REWARD))
    {
        ChangeState(ONTIME_EVENT_STATE_START);
    }
}
