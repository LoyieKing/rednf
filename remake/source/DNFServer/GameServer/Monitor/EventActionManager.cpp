// df_monitor_r — EventActionManager（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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

#include "EventActionManager.h"
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
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "OnTimeEventManager.h"

// ORIG 里 Packet_Monitor_Event_Start 的 +0xe 字段是按 EventParam(4 字节聚合) 复制的，
// GCC 4.4 对 4 字节聚合赋值生成「先取 LHS 地址」的序列（uint 成员赋值则相反：先取 RHS）。
// 本地视图复刻该偏移与聚合赋值语义，避免改动共享头 DNFPacketTranslater.h。
struct Packet_Monitor_Event_StartParamView
{
    char m_pad[0xe];
    EventParam m_eventParam;
};

CBaseEventAction::~CBaseEventAction() {}

void CBaseEventAction::onStartAction(EventParam& param) {}

void CBaseEventAction::onEndAction() {}

void CBaseEventAction::SetEventID(int id)
{
    m_eventId = id;
}

void CBaseEventAction::sendEventAckUpdate(int flag)
{
    Packet_Manager_Event_Trigger_Ack pkt;
    pkt.m_eventId = (unsigned int)m_eventId;
    pkt.m_flag = (unsigned int)flag;
    pkt.m_group =
        (unsigned int)((CApplication*)CApplicationInstance())->Get_ServerHandler()->GetServerGroupNo() & 0xff;
    ((CApplication*)CApplicationInstance())->Get_ServerHandler()->SendToDB(&pkt);
}

void CBaseEventAction::OnStartEvent(EventParam& param)
{
    sendEventAckUpdate(2);
    onStartAction(param);
}

void CBaseEventAction::OnEndEvent()
{
    sendEventAckUpdate(4);
    onEndAction();
}

CNullEventAction::CNullEventAction() throw() {}

CNullEventAction::~CNullEventAction() {}

void CNullEventAction::onStartAction(EventParam& param)
{
    puts("Test Event Action : On Start Null Event Action");
}

void CNullEventAction::onEndAction()
{
    puts("Test Event Action : On End Null Event Action");
}

COnTimeEventAction::COnTimeEventAction() throw() {}

COnTimeEventAction::~COnTimeEventAction() {}

void COnTimeEventAction::onStartAction(EventParam& param)
{
    DNF_LOG_SCOPE_LINE(0xa7,"./log/OnTimeEvent", "Test Event Action : On Start On Time Event Action %d,%d",
        (unsigned int)param.m_a, (unsigned int)param.m_b);
    COnTimeEventManager* mgr = ((CApplication*)CApplicationInstance())->GetOnTimeEventManager();
    if (mgr == 0)
    {
        return;
    }
    mgr->GetCurEventItemByDBMW((unsigned int)param.m_a, (unsigned int)param.m_b);
}

void COnTimeEventAction::onEndAction()
{
    DNF_LOG_SCOPE_LINE(0xc2, "./log/OnTimeEvent", "Test Event Action : On End On Time Event Action");
    COnTimeEventManager* mgr = ((CApplication*)CApplicationInstance())->GetOnTimeEventManager();
    if (mgr == 0)
    {
        return;
    }
    mgr->EndEvent();
}

CEventActionManager::CEventActionManager()
{
    init();
}

CEventActionManager::~CEventActionManager()
{
    destroy();
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0122 已消除）
void CEventActionManager::init()
{
    for (int i = 0; i < 0xa6; i++)
    {
        CNullEventAction* a = new CNullEventAction;
        a->SetEventID(i);
        m_actions[i] = a;
    }
    COnTimeEventAction* ot = new COnTimeEventAction;
    ot->SetEventID(0x33);
    m_actions[0x33] = ot;
    m_actions[0x9b] = new momiji_event::EventAction;
}

void CEventActionManager::destroy()
{
    for (int i = 0; i < 0xa6; i++)
    {
        if (m_actions[i] != 0)
        {
            delete m_actions[i];
        }
    }
}

void CEventActionManager::OnStartAction(Packet_Monitor_Event_Start* pkt)
{
    unsigned int code = pkt->m_eventCode;
    if (code < 0xa6)
    {
        union { unsigned int u; EventParam e; } param;
        param.u = pkt->m_eventParam;
        m_actions[code]->OnStartEvent(param.e);
        ((Packet_Monitor_Event_StartParamView*)pkt)->m_eventParam = param.e;
    }
    return;
}

void CEventActionManager::OnEndAction(unsigned int code)
{
    if (code < 0xa6)
    {
        m_actions[code]->OnEndEvent();
    }
    return;
}

CBaseEventAction* CEventActionManager::GetEventAction(int code)
{
    if (code < 0 || 0xa5 < code)
    {
        return 0;
    }
    return m_actions[code];
}
