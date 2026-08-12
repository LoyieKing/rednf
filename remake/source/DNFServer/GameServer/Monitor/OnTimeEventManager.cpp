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

COnTimeEventManager::COnTimeEventManager()
{
    m_app = 0;
    m_field30 = 0;
    m_field34 = 0;
    m_field40 = 0;
    Clear();
}

COnTimeEventManager::~COnTimeEventManager() {}

void COnTimeEventManager::AttachApp(CApplication* app)
{
    m_app = app;
    m_field38 = 0;
    m_field3c = 0;
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
    m_field30 = idx;
    m_field34 = 1;
}

void COnTimeEventManager::SendContinueTimeToGS()
{
    if (m_app != 0)
    {
        Packet_MTG_OntimeEvent_RewardStart pkt;
        pkt.m_eventIdx = (unsigned int)m_field30;
        pkt.m_field12 = m_field38;
        pkt.m_field16 = m_field3c;
        pkt.m_fieldE = m_field28 * 0x3c;
        m_app->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
    }
}

void COnTimeEventManager::UpdateEventIdx()
{
    m_field30 = m_field30 + 1;
    SendEventIdxToDBMW();
}

unsigned int COnTimeEventManager::GetEvent_Idx() { return (unsigned int)m_field30; }

void COnTimeEventManager::Clear()
{
    m_rewardList.Clear();
    m_field1c = 0;
    m_field20 = 0;
    m_field24 = 0;
    m_field28 = 0;
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
    m_field38 = idx;
    m_field3c = cnt;
}

void COnTimeEventManager::StartEvent()
{
    StartEvent(m_field24, m_field28);
}

void COnTimeEventManager::StartEvent(unsigned int a, unsigned int b)
{
    if (b >= a)
    {
        return;
    }
    unsigned int t = (unsigned int)time(0);
    m_field24 = (int)a;
    m_field28 = (int)b;
    m_field1c = (int)t;
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
    m_field24 = (int)a;
    m_field28 = (int)b;
    Packet_Req_Ontime_Event_Item pkt;
    if (m_app == 0)
    {
        return;
    }
    CTcpDBServer* db = m_app->Get_ServerHandler()->GetTcpDBServer();
    if (db == 0)
    {
        return;
    }
    char* buf = db->makePacketHeader(0x2345, 10);
    char* buf2 = buf;
    db->SendToServer(buf2);
}

void COnTimeEventManager::SendEventIdxToDBMW()
{
    if (m_app != 0)
    {
        CTcpDBServer* db = m_app->Get_ServerHandler()->GetTcpDBServer();
        if (db != 0)
        {
            char* buf = db->makePacketHeader(0x2347, 0x16);
            char* buf2 = buf;
            *(unsigned int*)(buf2 + 0x12) = (unsigned int)m_field30;
            *(unsigned int*)(buf2 + 10) = m_field38;
            *(unsigned int*)(buf2 + 0xe) = m_field3c;
            db->SendToServer(buf2);
        }
    }
}

int COnTimeEventManager::GetCurIdxByDBMW()
{
    if (m_field34 != 0)
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
    CMyFileLog log(__FUNCTION__, 0x82);
    log("./log/OnTimeEvent", "On Time Event : On Reward Start Trigger On(%d)", time(0));
    if (IsCurState(ONTIME_EVENT_STATE_REWARD))
    {
        puts("On Time Event : Event Off Trigger");
        Clear();
    }
    else
    {
        if (IsCurState(ONTIME_EVENT_STATE_NONE) == 0)
        {
            ChangeState(ONTIME_EVENT_STATE_NONE);
            UpdateEventIdx();
            int t = (int)time(0);
            m_field20 = t;
            register CTaskScheduler::CTask* task =
                new COnTimeEventRewardEndTrigger((unsigned int)(m_field28 * 0x3c + t), 0, this);
            m_app->GetTaskScheduler()->AddTask(task);
            CMyFileLog log2(__FUNCTION__, 0xa7);
            log2("./log/OnTimeEvent",
                 "On Time Event : On Reward Start Trigger Process Success curidx(%d)", GetEvent_Idx());
        }
    }
}

void COnTimeEventManager::OnRewardEnd()
{
    CMyFileLog log(__FUNCTION__, 0xae);
    log("./log/OnTimeEvent", "On Time Event : On Reward End Trigger On(%d)\n", time(0));
    if (IsCurState(ONTIME_EVENT_STATE_REWARD))
    {
        puts("On Time Event : Event Off Trigger");
        Clear();
    }
    else
    {
        if (IsCurState(ONTIME_EVENT_STATE_START) == 0)
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
                                                               COnTimeEventManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

COnTimeEventRewardStartTrigger::~COnTimeEventRewardStartTrigger() {}

COnTimeEventIdxLoad::COnTimeEventIdxLoad(unsigned int time, unsigned int flag,
                                         COnTimeEventManager* mgr)
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
